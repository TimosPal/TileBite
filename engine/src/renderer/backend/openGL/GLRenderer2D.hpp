#ifndef GL_RENDERER_2D_HPP
#define GL_RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/Renderer2D.hpp"

namespace Engine {

class GLRenderer2D : public Renderer2D {
public:
	virtual bool init() override;
	virtual bool terminate() override;

	virtual void clearScreen() override;
	virtual void render() override;
};

} // Engine

#endif // !GL_RENDERER_2D_HPP