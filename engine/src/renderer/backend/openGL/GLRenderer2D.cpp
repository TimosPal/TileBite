#include "renderer/backend/openGL/GLRenderer2D.hpp"

#include "utilities/Logger.hpp"
#include "renderer/backend/openGL/GLWrapper.hpp"

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

bool GLRenderer2D::init()
{
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

	// Clear color TODO: abstract
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	return true;
}

bool GLRenderer2D::terminate()
{
	return true;
}

void GLRenderer2D::clearScreen()
{
	GL(glClear(GL_COLOR_BUFFER_BIT));
}

void GLRenderer2D::render()
{

}

} // Engine