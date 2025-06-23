#ifndef RENDERER_2D_HPP
#define RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/SpriteQuad.hpp"
#include "renderer/DrawCommand2D.hpp"
#include "resources/SystemResourceHub.hpp"
#include "renderer/IGPUAssets.hpp"
#include "renderer/TextureSlotManager.hpp"
#include "renderer/camera/CameraController.hpp"

namespace Engine {

class Renderer2D {
public:
	virtual bool init() { return false; }
	virtual bool terminate() { return false; }

    virtual void clearScreen() {}
    virtual void render(CameraController& camera) {}

	virtual IGPUAssets& getGPUAssets() = 0;

	void drawQuad(SpriteQuad&& spriteQuad)
	{
		m_spriteDrawCommands.emplace_back(std::move(spriteQuad));
	}

	void drawQuadMesh(QuadMesh&& quadMesh)
	{
		m_quadMeshesDrawCommands.emplace_back(std::move(quadMesh));
	}

	virtual void setViewportSize(uint32_t width, uint32_t height) {}

	static std::unique_ptr<Renderer2D> createRenderer2D(SystemResourceHub& systemResourceHub);

protected:
	std::vector<SpriteQuad> m_spriteDrawCommands;
	std::vector<QuadMesh> m_quadMeshesDrawCommands;

	TextureSlotManager m_textureSlotManager;
};

} // Engine

#endif // !RENDERER_2D_HPP