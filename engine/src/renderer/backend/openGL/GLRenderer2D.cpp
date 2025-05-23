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

	setupBuffers();

	// Fallback texture (Missing texture)
	m_fallbackTexture = m_resourceHub.getManager<GLTexture>().getResource(ResourceNames::FallbackTexture);
	m_fallbackTexture.watch();
	m_fallbackTexture.load();

	LOG_INFO("");

	return true;
}

void GLRenderer2D::setupBuffers()
{
	LOG_INFO("[Setting up buffers]");

	VertexLayout spriteLayout;
	spriteLayout.add(VertexAttribute("aPos", ShaderAttributeType::Float2));
	spriteLayout.add(VertexAttribute("aColor", ShaderAttributeType::Float4));
	spriteLayout.add(VertexAttribute("aUV", ShaderAttributeType::Float2));
	m_spritesBatch = std::make_unique<GLMesh>(
		spriteLayout.getStride() * verticesPerQuad * maxQuadsPerBatch,
		quadsIndicesCount
	);

	m_spriteProgramHandle = m_resourceHub.getManager<GLProgram>().getResource(ResourceNames::SpriteShader);
	m_spriteProgramHandle.watch();
	m_spriteProgramHandle.load();
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

void GLRenderer2D::drawBatch(uint32_t& quadsCount, uint32_t& bytes, ID textureID, int& drawCalls)
{
	// TODO: use all texture slots

	m_spritesBatch->bind();
	m_spritesBatch->setVertexData(m_spriteBatchVertexData.data(), bytes);
	m_spriteProgramHandle.getResource()->use();

	glActiveTexture(GL_TEXTURE0);

	auto textureHandle = m_resourceHub.getManager<GLTexture>().getResource(textureID);
	textureHandle.watch();
	textureHandle.load();
	if (textureHandle.isValid())
	{
		textureHandle.getResource()->bind();
	}
	else
	{
		m_fallbackTexture.getResource()->bind();
	}

	glUniform1i(glGetUniformLocation(m_spriteProgramHandle.getResource()->getGLID(), "uTexture"), 0);
	m_spritesBatch->draw(quadsCount * 6);

	textureHandle.unwatch();
	textureHandle.unload();

	quadsCount = 0;
	bytes = 0;
	drawCalls++;
}

void GLRenderer2D::render()
{
	int drawCalls = 0;

	uint32_t vertexPos = 0;
	uint32_t quadsCount = 0;
	ID previousTextureID = -1;

	// Sort by Texture ID to optimize drawCalls (less texture swapping)
	std::sort(m_drawCommands.begin(), m_drawCommands.end(), [](const DrawCommand2D& a, const DrawCommand2D& b) {
		if (a.type != b.type)
			return a.type < b.type;

		if (a.type == DrawCommand2DType::Quad)
			return a.spriteQuad.spriteID < b.spriteQuad.spriteID;

		return false;
	});

	for (const auto& command : m_drawCommands)
	{
		// Currently supporting only 2d quads
		if (command.type != DrawCommand2DType::Quad) continue;

		ID currentTextureID = command.spriteQuad.spriteID;

		bool maxQuadsReached = quadsCount == maxQuadsPerBatch;
		bool textureChange = quadsCount > 0 && previousTextureID != currentTextureID;
		bool shouldFlush = maxQuadsReached || textureChange;
		if (shouldFlush) drawBatch(quadsCount, vertexPos, previousTextureID, drawCalls);

		previousTextureID = currentTextureID;

		float x = command.spriteQuad.x;
		float y = command.spriteQuad.y;
			
		float w = command.spriteQuad.w;
		float h = command.spriteQuad.h;
			
		float r = command.spriteQuad.r;
		float g = command.spriteQuad.g;
		float b = command.spriteQuad.b;
		float a = command.spriteQuad.a;

		float u0 = command.spriteQuad.u0;
		float v0 = command.spriteQuad.v0;
		float u1 = command.spriteQuad.u1;
		float v1 = command.spriteQuad.v1;

		float quadVerts[] = {
			// pos          // color      // uv
			x,     y,       r, g, b, a,   u0, v0,
			x + w, y,       r, g, b, a,   u1, v0,
			x + w, y - h,   r, g, b, a,   u1, v1,
			x,     y - h,   r, g, b, a,   u0, v1,
		};

		memcpy(m_spriteBatchVertexData.data() + vertexPos, quadVerts, sizeof(quadVerts));
		vertexPos += sizeof(quadVerts);
		quadsCount++;
	}

	// Render last remaining batch if it contains data
	if(quadsCount) drawBatch(quadsCount, vertexPos, previousTextureID, drawCalls);

	// LOG_INFO("DrawCalls: {}", drawCalls);
	m_drawCommands.clear();
}

} // Engine