#ifndef GL_RENDERER_2D_HPP
#define GL_RENDERER_2D_HPP

#include "core/pch.hpp"
#include "renderer/Renderer2D.hpp"
#include "renderer/backend/openGL/GLResourceHub.hpp"
#include "renderer/backend/openGL/GLMesh.hpp"
#include "renderer/backend/openGL/GLGPUAssets.hpp"

namespace TileBite {

// TODO: Make frag shader have same value by meta programming (?)
constexpr uint32_t maxTextures = 128; // Should match fragment shader texture array size
constexpr uint32_t maxQuadsPerBatch = 1024;
constexpr uint32_t quadsIndicesCount = 6 * maxQuadsPerBatch;
constexpr uint32_t verticesPerQuad = 4;
constexpr uint32_t indicesPerQuad = 6;
constexpr uint32_t maxLinesPerBatch = 1024;
constexpr uint32_t verticesPerLine = 2;

class GLRenderer2D : public Renderer2D {
public:
	GLRenderer2D(SystemResourceHub& systemResourceHub);

	virtual bool init() override;
	virtual bool terminate() override;

	virtual void clearScreen() override;
	virtual void render(CameraController& camera) override;

	virtual void setViewportSize(uint32_t width, uint32_t height) override;

	virtual IGPUAssets& getGPUAssets() override { return m_gpuAssets; }

private:
	GLResourceHub m_resourceHub;
	GLGPUAssets m_gpuAssets;

	std::unordered_map<ID, std::unique_ptr<GLMesh>> m_tilemapBuffers;

	std::unique_ptr<GLMesh> m_spritesBatch;
	std::unique_ptr<GLMesh> m_linesBatch;
	std::vector<uint8_t> m_spriteVertexData;
	std::vector<uint8_t> m_linesVertexData;

	ResourceHandle<GLProgram> m_spriteProgramHandle;
	ResourceHandle<GLProgram> m_tilemapProgramHandle;
	ResourceHandle<GLProgram> m_lineProgramHandle;
	ResourceHandle<GLTexture> m_fallbackTexture;

	void setupShaders();
	void setupBuffers();
	void setupTextures();
	void drawSpritesBatch(uint32_t& quadsCount, uint32_t& bytes, int& drawCalls);
	void bindTextureToSlot(ID textureID, uint8_t slot);

	uint8_t numberOfGPUSlots() const;

	void renderSpriteQuads(CameraController& camera);
	void renderTilemaps(CameraController& camera);
	void renderLines(CameraController& camera);
};

} // TileBite

#endif // !GL_RENDERER_2D_HPP