#ifndef GL_WRAPPER_HPP
#define GL_WRAPPER_HPP

#include <glad/glad.h>

#include "utilities/Logger.hpp"

namespace Engine {

#define GL_CHECK_ERROR() Engine::GLCheckError(__FILE__, __LINE__)

#ifndef DEBUG_MODE
    // No error checking in release mode
    #define GL_RET(func, returnValue) returnValue = func;
    #define GL(func) func;
#else
    #define GL_RET(func, returnValue) \
            do { \
                returnValue = func; \
                GL_CHECK_ERROR(); \
            } while(0)
    #define GL(func) \
            do { \
                func; \
                GL_CHECK_ERROR(); \
            } while(0)
#endif

inline void GLCheckError(const char* file, int line) {
    int count = 0;
    const int maxCount = 10;

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        if (count >= maxCount)
            break;

        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        LOG_ERROR("OpenGL {} at {}:{}", error, file, line);

        count++;
    }

    if (count >= maxCount)
    {
        LOG_CRITICAL("Max errors reached for current GL call");
    }
}

} // Engine

#endif // !GL_WRAPPER_HPP