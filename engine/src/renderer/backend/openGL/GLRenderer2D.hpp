#ifndef GL_RENDERER_2D_HPP
#define GL_RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"
#include "renderer/backend/openGL/GLMesh.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"

namespace Engine {

constexpr uint32_t maxQuadsPerBatch = 1024;
constexpr uint32_t quadsIndicesCount = 6 * maxQuadsPerBatch;
constexpr uint32_t verticesPerQuad = 4;
constexpr uint32_t indicesPerQuad = 6;

class GLRenderer2D : public Renderer2D {
public:
	GLRenderer2D(SystemResourceHub& systemResourceHub);

	virtual bool init() override;
	virtual bool terminate() override;

	virtual void clearScreen() override;
	virtual void render(OrthographicCamera& camera) override;

	virtual IGPUAssets& getGPUAssets() override { return m_gpuAssets; }

private:
	GLResourceHub m_resourceHub;
	GLGPUAssets m_gpuAssets;

	std::unique_ptr<GLMesh> m_spritesBatch;
	std::vector<uint8_t> m_spriteBatchVertexData;

	ResourceHandle<GLProgram> m_spriteProgramHandle;
	ResourceHandle<GLTexture> m_fallbackTexture;

	void setupShaders();
	void setupBuffers();
	void setupTextures();
	void drawBatch(uint32_t& quadsCount, uint32_t& bytes, int& drawCalls);
	void sortDrawCommands();
	void bindTextureToSlot(ID textureID, uint8_t slot);

	uint8_t numberOfGPUSlots() const;
};

} // Engine

#endif // !GL_RENDERER_2D_HPP