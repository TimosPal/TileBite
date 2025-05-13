#ifndef RENDERER_2D_HPP
#define RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/SpriteQuad.hpp"
#include "renderer/DrawCommand2D.hpp"
#include "resources/types/SystemResourceHub.hpp"

namespace Engine {

class Renderer2D {
public:
	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

    virtual void clearScreen() {}
    virtual void render() {}

	void drawQuad(SpriteQuad&& spriteQuad)
	{
		m_drawCommands.emplace_back(
			DrawCommand2D{ DrawCommand2DType::Quad, std::move(spriteQuad) }
		);
	}

	static std::unique_ptr<Renderer2D> createRenderer2D(SystemResourceHub& systemResourceHub);

protected:
	std::vector<DrawCommand2D> m_drawCommands;
};

} // Engine

#endif // !RENDERER_2D_HPP