#include "renderer/Renderer2D.hpp"

// NOTE: Using OpenGL as the current rendering backend.
#if defined(RENDERER_OPENGL)
    #include "renderer/backend/openGL/GLRenderer2D.hpp"
    using Renderer2DBackend = Engine::GLRenderer2D;
#else
    #error Unsupported Rendering API
#endif

namespace Engine {

std::unique_ptr<Renderer2D> Renderer2D::createRenderer2D()
{
    return std::make_unique<Renderer2DBackend>();
}

} // Engine 