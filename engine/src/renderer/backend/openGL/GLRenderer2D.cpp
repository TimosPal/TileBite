#include "renderer/backend/openGL/GLRenderer2D.hpp"

#include "utilities/Logger.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"
#include "renderer/backend/openGL/GLProgram.hpp"
#include "core/ResourceRegistry.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"
#include "utilities/misc.hpp"

// NOTE: if other APIs also require a loader per window backend, this may need
// better abstraction.
#if defined(WINDOW_BACKEND_GLFW)
#include <GLFW/glfw3.h>
bool loadFunctions()
{
	return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}
#else
bool loadFunctions()
{
	return true;
}
#endif

namespace Engine {

static void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	const char* sourceStr = "Unknown";
	switch (source) {
	case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: sourceStr = "Other"; break;
	}

	const char* typeStr = "Unknown";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: typeStr = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: typeStr = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: typeStr = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: typeStr = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: typeStr = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: typeStr = "Other"; break;
	}

	const char* severityStr = "Unknown";
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: severityStr = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: severityStr = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW: severityStr = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: severityStr = "Notification"; break;
	}

	LOG_CRITICAL(
		"[OpenGL Debug]\n"
		"ID: {}\n"
		"Message: {}\n"
		"Source: {}\n"
		"Type: {}\n"
		"Severity: {}\n",
		id, message, sourceStr, typeStr, severityStr
	);
}

GLRenderer2D::GLRenderer2D(SystemResourceHub& systemResourceHub)
	: 
	m_resourceHub(systemResourceHub),
	m_gpuAssets(m_resourceHub)
{}

uint8_t GLRenderer2D::numberOfGPUSlots() const
{
	static const uint8_t cachedSlots = []() {
		GLint slots = 0;
		GL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &slots));
		return static_cast<uint8_t>(slots);
	}();
	return cachedSlots;
}

bool GLRenderer2D::init()
{
	// TODO:
	// 1) Abstract clear color
	// 2) Blending options
	// 3) Maybe abstract Fallback texture and defaultSprite to generic resources (No special use)

	if (!loadFunctions())
	{
		LOG_ERROR("Failed to initialize window functions");
		return false;
	}

	// Log OpenGL version
	const GLubyte* version;
	GL_RET(glGetString(GL_VERSION), version);
	std::string versionString(reinterpret_cast<const char*>(version));
	LOG_INFO("Init glad {}", versionString);

	// Debuging context.
	int flags;
	GL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		LOG_INFO("OpenGL Debug Context Enabled");

		GL(glEnable(GL_DEBUG_OUTPUT));
		GL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
		GL(glDebugMessageCallback(glDebugOutput, nullptr));
		GL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));
	}
	else
	{
		LOG_WARNING("OpenGL Debug Context Not Available");
	}

	GL(glClearColor(0.2f, 0.2f, 0.2f, 0.0f));

	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Load renderer resources
	bool resResourceHubInit = m_resourceHub.init();
	ASSERT(resResourceHubInit, "Resource hub not init");
	
	setupShaders();
	setupBuffers();
	setupTextures();

	LOG_INFO("");

	return true;
}

void GLRenderer2D::setupShaders()
{
	LOG_INFO("[Setting up shaders]");

	m_spriteProgramHandle = m_resourceHub.getManager<GLProgram>().getResource(ResourceNames::SpriteShader);
	m_spriteProgramHandle.watch();
	m_spriteProgramHandle.load();

	m_tilemapProgramHandle = m_resourceHub.getManager<GLProgram>().getResource(ResourceNames::TilemapShader);
	m_tilemapProgramHandle.watch();
	m_tilemapProgramHandle.load();

	m_lineProgramHandle = m_resourceHub.getManager<GLProgram>().getResource(ResourceNames::LineShader);
	m_lineProgramHandle.watch();
	m_lineProgramHandle.load();

	// Setup a uniform texture sampler array in the fragment shader
	// Each slot corresponds to an array index
	// eg: slot 0 -> uTextures[0]
	auto* spriteProgram = m_spriteProgramHandle.getResource();
	const uint8_t textureSlots = numberOfGPUSlots();
	std::vector<int> samplers(textureSlots);
	for (uint8_t i = 0; i < textureSlots; ++i) samplers[i] = i;
	spriteProgram->setUniform("uTextures", samplers.data(), textureSlots);
}

void GLRenderer2D::setupBuffers()
{
	LOG_INFO("[Setting up buffers]");

	// When dealing with the quad batch, index data are always the same so we can precompute them and
	// upload them once.
	auto indexData = makeIndices(quadsIndicesCount, verticesPerQuad, indicesPerQuad, maxQuadsPerBatch);

	// Sprite
	VertexLayout spriteLayout;
	spriteLayout.add(VertexAttribute("aPos", ShaderAttributeType::Float2));
	spriteLayout.add(VertexAttribute("aColor", ShaderAttributeType::Float4));
	spriteLayout.add(VertexAttribute("aUV", ShaderAttributeType::Float2));
	spriteLayout.add(VertexAttribute("aTextureIndex", ShaderAttributeType::Float));
	m_spritesBatch = std::make_unique<GLMesh>(
		spriteLayout.getStride() * verticesPerQuad * maxQuadsPerBatch,
		quadsIndicesCount
	);

	auto* spriteProgram = m_spriteProgramHandle.getResource();
	m_spritesBatch->setupAttributes(spriteLayout, spriteProgram->getGLID());
	m_spritesBatch->setIndexData(indexData.data(), indexData.size());
	m_spriteVertexData.resize(maxQuadsPerBatch * verticesPerQuad * spriteLayout.getStride());

	// Tilemap
	// tilemap layout is created upon request in renderQuadMeshes

	// Line
	VertexLayout lineLayout;
	lineLayout.add(VertexAttribute("aPos", ShaderAttributeType::Float2));
	lineLayout.add(VertexAttribute("aColor", ShaderAttributeType::Float4));
	m_linesBatch = std::make_unique<GLMesh>(
		lineLayout.getStride() * verticesPerLine * maxLinesPerBatch,
		0 // Not using index buffer for line drawing (EBO will not be init)
	);

	auto* lineProgram = m_lineProgramHandle.getResource();
	m_linesBatch->setupAttributes(lineLayout, lineProgram->getGLID());
	m_linesVertexData.resize(maxLinesPerBatch * verticesPerLine * lineLayout.getStride());
}

void GLRenderer2D::setupTextures()
{
	LOG_INFO("[Setting up engine textures]");

	m_gpuAssets.makeTexturePersistent(ResourceNames::DefaultSpriteTexture);

	// Fallback texture (Missing texture)
	m_fallbackTexture = m_resourceHub.getManager<GLTexture>().getResource(ResourceNames::FallbackTexture);
	m_fallbackTexture.watch();
	m_fallbackTexture.load();

	m_textureSlotManager.setNumberOfSlots(numberOfGPUSlots());
	LOG_INFO("GPU Texture slots: {}", m_textureSlotManager.getNumberOfSlots());
}

bool GLRenderer2D::terminate()
{
	m_spriteProgramHandle.unwatch();
	m_spriteProgramHandle.unload();

	m_tilemapProgramHandle.unwatch();
	m_tilemapProgramHandle.unload();

	m_lineProgramHandle.unwatch();
	m_lineProgramHandle.unload();

	m_fallbackTexture.unwatch();
	m_fallbackTexture.unload();

	m_gpuAssets.clear();

	bool destroyedResourceHub = m_resourceHub.destroy();
	return destroyedResourceHub;
} 

void GLRenderer2D::clearScreen()
{
	GL(glClear(GL_COLOR_BUFFER_BIT));
}

void GLRenderer2D::drawSpritesBatch(uint32_t& quadsCount, uint32_t& bytes, int& drawCalls)
{
	m_spritesBatch->bind();
	m_spritesBatch->setVertexData(m_spriteVertexData.data(), bytes);

	if (m_textureSlotManager.isDirty())
	{
		// Update uniform buffer to correspond to the current texture slots
		auto* spriteProgram = m_spriteProgramHandle.getResource();
		auto mapping = m_textureSlotManager.createTextureMapping(maxTextures);
		spriteProgram->setUniform("uTexSlotMap", mapping.data(), maxTextures);

		m_textureSlotManager.setDirty(false);
	}

	m_spritesBatch->drawIndexed(quadsCount * 6);

	quadsCount = 0;
	bytes = 0;
	drawCalls++;
}

void GLRenderer2D::bindTextureToSlot(ID textureID, uint8_t slot)
{
	GL(glActiveTexture(GL_TEXTURE0 + slot));
	auto textureHandle = m_resourceHub.getManager<GLTexture>().getResource(textureID);
	if (textureHandle.isValid())
		textureHandle.getResource()->bind();
	else
		m_fallbackTexture.getResource()->bind();

	m_textureSlotManager.setDirty(true);
}

void GLRenderer2D::render(CameraController& camera)
{
	renderTilemaps(camera);
	renderSpriteQuads(camera);
	renderLines(camera);
}

void GLRenderer2D::renderLines(CameraController& camera)
{
	// TODO: Optimize line rendering by batching

	GLProgram* program = m_lineProgramHandle.getResource();
	program->use();
	camera.recalculate();
	program->setUniform("uViewProjection", camera.getViewProjectionMatrix());

	int drawCalls = 0;
	for (const auto& command : m_lineDrawCommands)
	{
		auto lineVertices = makeLineVerticesColored(command.Start, command.End, command.Color);
		m_linesBatch->bind();
		m_linesBatch->setVertexData(lineVertices.data(), lineVertices.size() * sizeof(float));
		m_linesBatch->drawLines(2);

		drawCalls++;
	}

	m_lineDrawCommands.clear();
}

void GLRenderer2D::renderTilemaps(CameraController& camera)
{
	GLProgram* program = m_tilemapProgramHandle.getResource();
	program->use();
	camera.recalculate();
	program->setUniform("uViewProjection", camera.getViewProjectionMatrix());

	m_textureSlotManager.makeDisabled();

	int drawCalls = 0;
	for (const auto& command : m_tilemapDrawCommands)
	{
		int quadsCount = command.TilemapResource->getWidth() * command.TilemapResource->getHeight();
		
		ID meshID = command.TilemapResource->getInstanceID();
		auto [it, inserted] = m_tilemapBuffers.try_emplace(meshID, nullptr);
		if (inserted) {
			VertexLayout tilemapLayout;
			tilemapLayout.add(VertexAttribute("aPackedXYIndexUV", ShaderAttributeType::UInt));
			tilemapLayout.add(VertexAttribute("aPackedRGBA", ShaderAttributeType::UInt));
			it->second = std::make_unique<GLMesh>(
				tilemapLayout.getStride() * verticesPerQuad * quadsCount,
				6 * quadsCount // max quads per batch * indices per quad
			);

			auto indexData = makeIndices(quadsCount * 6, verticesPerQuad, indicesPerQuad, quadsCount);
			auto* tilemapProgram = m_tilemapProgramHandle.getResource();
			it->second->setupAttributes(tilemapLayout, tilemapProgram->getGLID());
			it->second->setIndexData(indexData.data(), indexData.size());
		}

		auto& mesh = it->second;
		command.TilemapResource->mergeBytesChanges(); // Optimize changes by merging overlapping ones
		if (command.TilemapResource->hasByteChanges())
		{
			auto& vertices = command.TilemapResource->getData();
			uint8_t* vertexBytes = reinterpret_cast<uint8_t*>(vertices.data());
			for(auto change : command.TilemapResource->getBytesChanges())
			{
				// Update only the changed bytes in the vertex buffer
				mesh->setSubVertexData
				(
					vertexBytes + change.Offset,
					change.Size,
					change.Offset
				);
			}

			command.TilemapResource->resetChangesList();
		}

		bool isAssigned = false;
		ID atlasID = command.TilemapResource->getAtlasID();
		auto textureSlot = m_textureSlotManager.getTextureToSlotID(atlasID, isAssigned);
		if (!isAssigned)
		{
			textureSlot = m_textureSlotManager.getLeastUsedSlot();
			m_textureSlotManager.addSlot(textureSlot, atlasID);
			bindTextureToSlot(atlasID, textureSlot);

			auto mapping = m_textureSlotManager.createTextureMapping(maxTextures);
			program->setUniform("uTexSlotMap", mapping.data(), maxTextures);
			m_textureSlotManager.setDirty(false);
		}
		m_textureSlotManager.incrementSlotCounter(textureSlot);

		program->setUniform("uTilemapID", atlasID);
		program->setUniform("uTilemapDim", command.TilemapResource->getAtlasDim());
		program->setUniform("uWorldTileSize", command.TilemapResource->getWorldTileSize());
		program->setUniform("uTextureTileSize", command.TilemapResource->getAtlasTileSize());
		program->setUniform("uOffset", command.TransformComp->Position);

		mesh->drawIndexed(quadsCount * 6);
		
		drawCalls++;
	}

	m_tilemapDrawCommands.clear();
}

void GLRenderer2D::renderSpriteQuads(CameraController& camera)
{
	GLProgram* program = m_spriteProgramHandle.getResource();
	program->use();
	camera.recalculate();
	program->setUniform("uViewProjection", camera.getViewProjectionMatrix());

	int drawCalls = 0;

	uint32_t vertexPos = 0;
	uint32_t quadsCount = 0;

	uint8_t previousTextureSlot = 0;
	ID previousTextureID = 0;
	bool firstFrame = true;

	m_textureSlotManager.makeDisabled();

	// Sort by Texture ID to optimize drawCalls (less texture swapping)
	std::sort(m_spriteDrawCommands.begin(), m_spriteDrawCommands.end(), [](const SpriteQuad& a, const SpriteQuad& b) {
		return a.SpriteComp->TextureID < b.SpriteComp->TextureID;
	});
	for (const auto& command : m_spriteDrawCommands)
	{
		ID currentTextureID = command.SpriteComp->TextureID;

		uint8_t textureSlot;
		bool batchTextureSlotChange = false;
		bool newSlotAdded = false;
		bool isAssigned;

		if (currentTextureID == previousTextureID && !firstFrame)
		{
			textureSlot = previousTextureSlot;
			isAssigned = true;
		}
		else
		{
			textureSlot = m_textureSlotManager.getTextureToSlotID(currentTextureID, isAssigned);
		}
		if (!isAssigned)
		{
			// We have to add a new slot for the new texture ID.
			// (Else nothing needs to be done, texture is already correctly set for current batch)
			textureSlot = m_textureSlotManager.getLeastUsedSlot();
			if (m_textureSlotManager.isSlotActive(textureSlot))
			{
				// If the slot is active and we need to replace said texture, a flush should occure
				// sice this texture slot is used by the current batch.
				batchTextureSlotChange = true;
			}
			newSlotAdded = true;
		}

		bool maxQuadsReached = quadsCount == maxQuadsPerBatch;
		bool shouldFlush = maxQuadsReached || batchTextureSlotChange;
		if (shouldFlush) drawSpritesBatch(quadsCount, vertexPos, drawCalls);
		if (newSlotAdded)
		{
			// Change slot state after drawing potentional batch so mapping is correct
			m_textureSlotManager.addSlot(textureSlot, currentTextureID);
			bindTextureToSlot(currentTextureID, textureSlot);
		}

		auto vertices = makeSpriteQuadVertices(command.TransformComp, command.SpriteComp);
		int verticesSizeInBytes = vertices.size() * sizeof(float);
		memcpy(m_spriteVertexData.data() + vertexPos, vertices.data(), verticesSizeInBytes);
		vertexPos += verticesSizeInBytes;
		quadsCount++;
		m_textureSlotManager.incrementSlotCounter(textureSlot);

		previousTextureID = currentTextureID;
		previousTextureSlot = textureSlot;
		firstFrame = false;
	}

	// Render last remaining batch if it contains data
	if (quadsCount) drawSpritesBatch(quadsCount, vertexPos, drawCalls);

	//LOG_INFO("DrawCalls: {}", drawCalls);
	m_spriteDrawCommands.clear();
}

void GLRenderer2D::setViewportSize(uint32_t width, uint32_t height)
{
	GL(glViewport(0, 0, width, height));
}

} // Engine