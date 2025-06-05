#include "renderer/backend/openGL/GLRenderer2D.hpp"

#include "utilities/Logger.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"
#include "renderer/backend/openGL/GLProgram.hpp"
#include "core/ResourceRegistry.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"

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

	// When dealing with the quad batch, index data are always the same so we can precompute them and
	// upload them once.
	std::vector<uint32_t> indexData(quadsIndicesCount);
	for (size_t i = 0; i < maxQuadsPerBatch; ++i)
	{
		uint32_t offset = i * verticesPerQuad;
		size_t idx = i * indicesPerQuad;

		indexData[idx + 0] = offset + 0;
		indexData[idx + 1] = offset + 1;
		indexData[idx + 2] = offset + 2;
		indexData[idx + 3] = offset + 2;
		indexData[idx + 4] = offset + 3;
		indexData[idx + 5] = offset + 0;
	}
	m_spritesBatch->setIndexData(indexData.data(), indexData.size());

	m_spriteBatchVertexData.resize(maxQuadsPerBatch * verticesPerQuad * spriteLayout.getStride());
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

void GLRenderer2D::drawBatch(uint32_t& quadsCount, uint32_t& bytes, int& drawCalls)
{
	m_spritesBatch->bind();
	m_spritesBatch->setVertexData(m_spriteBatchVertexData.data(), bytes);

	if (m_textureSlotManager.isDirty())
	{
		// Update uniform buffer to correspond to the current texture slots
		auto* spriteProgram = m_spriteProgramHandle.getResource();
		auto mapping = m_textureSlotManager.createTextureMapping(maxTextures);
		spriteProgram->setUniform("uTexSlotMap", mapping.data(), maxTextures);

		m_textureSlotManager.setDirty(false);
	}

	m_spritesBatch->draw(quadsCount * 6);

	quadsCount = 0;
	bytes = 0;
	drawCalls++;
}

void GLRenderer2D::sortDrawCommands()
{
	// Sort by Texture ID to optimize drawCalls (less texture swapping)
	std::sort(m_drawCommands.begin(), m_drawCommands.end(), [](const DrawCommand2D& a, const DrawCommand2D& b) {
		if (a.type != b.type)
			return a.type < b.type;
		if (a.type == DrawCommand2DType::Quad)
			return a.spriteQuad.SpriteComp->TextureID < b.spriteQuad.SpriteComp->TextureID;
		return false;
	});
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
	int drawCalls = 0;

	uint32_t vertexPos = 0;
	uint32_t quadsCount = 0;

	uint8_t previousTextureSlot = 0;
	ID previousTextureID = 0;
	bool firstFrame = true;

	// TODO: supoprt draw commands that dont use this program.
	// In the future, we may need to bind a shader based on the batched draw commands.
	GLProgram* program =  m_spriteProgramHandle.getResource();
	program->use();
	camera.recalculate();
	program->setUniform("uViewProjection", camera.getViewProjectionMatrix());

	sortDrawCommands();
	m_textureSlotManager.makeDisabled();

	for (const auto& command : m_drawCommands)
	{
		// Currently supporting only 2d quads
		if (command.type != DrawCommand2DType::Quad) continue;

		ID currentTextureID = command.spriteQuad.SpriteComp->TextureID;

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
		if (shouldFlush) drawBatch(quadsCount, vertexPos, drawCalls);
		if (newSlotAdded)
		{
			// Change slot state after drawing potentional batch so mapping is correct
			m_textureSlotManager.addSlot(textureSlot, currentTextureID);
			bindTextureToSlot(currentTextureID, textureSlot);
		}

		auto& pos = command.spriteQuad.TransformComp->Position;
		auto& size = command.spriteQuad.TransformComp->Size;
		float angle = command.spriteQuad.TransformComp->Rotation;

		// Small explanaiton of the following math:
		// We apply transformations in the following order
		// (Matrices are too slow for cpu calculations!)
		// 1) scale (0.5 due to the local quad size)
		// 2) vertex rotated by angle (in radians)
		// Using formulas:
		// [x' = x cos(angle) - y sin(angle)]
		// [y' = x sin(angle) + y cos(angle)]
		// 3) vertex moved to position

		float cosA = cos(angle);
		float sinA = sin(angle);
		float hw = size.x * 0.5f;
		float hh = size.y * 0.5f;

		// Quad corners after scale + rotation + translation
		float topLeftX = pos.x + (-hw * cosA - hh * sinA);
		float topLeftY = pos.y + (-hw * sinA + hh * cosA);

		float topRightX = pos.x + (hw * cosA - hh * sinA);
		float topRightY = pos.y + (hw * sinA + hh * cosA);

		float bottomRightX = pos.x + (hw * cosA - (-hh) * sinA);
		float bottomRightY = pos.y + (hw * sinA + (-hh) * cosA);

		float bottomLeftX = pos.x + (-hw * cosA - (-hh) * sinA);
		float bottomLeftY = pos.y + (-hw * sinA + (-hh) * cosA);

		float r = command.spriteQuad.SpriteComp->Color.r;
		float g = command.spriteQuad.SpriteComp->Color.g;
		float b = command.spriteQuad.SpriteComp->Color.b;
		float a = command.spriteQuad.SpriteComp->Color.a;

		float u0 = command.spriteQuad.SpriteComp->UVRect.x;
		float v0 = command.spriteQuad.SpriteComp->UVRect.y;
		float u1 = command.spriteQuad.SpriteComp->UVRect.z;
		float v1 = command.spriteQuad.SpriteComp->UVRect.w;

		float quadVerts[] = {
			// pos						  // color      // uv     // texture
			topLeftX,     topLeftY,       r, g, b, a,   u0, v0,   currentTextureID,
			topRightX,    topRightY,      r, g, b, a,   u1, v0,   currentTextureID,
			bottomRightX, bottomRightY,   r, g, b, a,   u1, v1,   currentTextureID,
			bottomLeftX,  bottomLeftY,    r, g, b, a,   u0, v1,   currentTextureID
		};

		memcpy(m_spriteBatchVertexData.data() + vertexPos, quadVerts, sizeof(quadVerts));
		vertexPos += sizeof(quadVerts);
		quadsCount++;
		m_textureSlotManager.incrementSlotCounter(textureSlot);

		previousTextureID = currentTextureID;
		previousTextureSlot = textureSlot;
		firstFrame = false;
	}

	// Render last remaining batch if it contains data
	if(quadsCount) drawBatch(quadsCount, vertexPos, drawCalls);

	//LOG_INFO("DrawCalls: {}", drawCalls);
	m_drawCommands.clear();
}

void GLRenderer2D::setViewportSize(uint32_t width, uint32_t height)
{
	GL(glViewport(0, 0, width, height));
}

} // Engine