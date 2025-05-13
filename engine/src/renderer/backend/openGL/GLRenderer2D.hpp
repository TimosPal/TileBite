#ifndef GL_RENDERER_2D_HPP
#define GL_RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"

namespace Engine {

class GLRenderer2D : public Renderer2D {
public:
	GLRenderer2D(SystemResourceHub& systemResourceHub)
		: m_resourceHub(systemResourceHub)
	{}

	virtual bool init() override;
	virtual bool terminate() override;

	virtual void clearScreen() override;
	virtual void render() override;

private:
	GLResourceHub m_resourceHub;
};

} // Engine

#endif // !GL_RENDERER_2D_HPP