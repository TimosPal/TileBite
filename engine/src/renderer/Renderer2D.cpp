#include "renderer/Renderer2D.hpp"
#include "resources/SystemResourceHub.hpp"

// NOTE: Using OpenGL as the current rendering backend.
#if defined(RENDERER_OPENGL)
    #include "renderer/backend/openGL/GLRenderer2D.hpp"
    using Renderer2DBackend = Engine::GLRenderer2D;
#else
    #error Unsupported Rendering API
#endif

namespace Engine {

// TODO: culling not considering rotations

std::unique_ptr<Renderer2D> Renderer2D::createRenderer2D(SystemResourceHub& systemResourceHub)
{
    return std::make_unique<Renderer2DBackend>(systemResourceHub);
}

bool Renderer2D::shouldCullSpriteQuad(const SpriteQuad& quad, const CameraController& camera) const
{
	// Calculate sprite AABB in world space
	// NOTE: quads are centered arround 0,0 with original size 0.5 per side.
	glm::vec2 spriteMin = quad.TransformComp->Position - glm::vec2(0.5) * quad.TransformComp->Size;
	glm::vec2 spriteMax = quad.TransformComp->Position + glm::vec2(0.5) * quad.TransformComp->Size;
	AABB spriteAABB{ spriteMin, spriteMax };
	return !camera.isInsideFrustum(spriteAABB);
}

bool Renderer2D::shouldCullTilemap(const TilemapMesh& mesh, const CameraController& camera) const
{
	// Calculate tilemap AABB in world space
	glm::vec2 tilemapMin = mesh.TransformComp->Position;
	glm::vec2 tilemapMax = tilemapMin + glm::vec2(
		mesh.TilemapResource->getWidth() * mesh.TilemapResource->getWorldTileSize().x,
		mesh.TilemapResource->getHeight() * mesh.TilemapResource->getWorldTileSize().y
	);
	AABB tilemapAABB{ tilemapMin, tilemapMax };
	return !camera.isInsideFrustum(tilemapAABB);
}

bool Renderer2D::shouldCullLine(const Line& line, const CameraController& camera) const
{
	// Calculate line AABB in world space
	glm::vec2 min = glm::min(line.Start, line.End);
	glm::vec2 max = glm::max(line.Start, line.End);
	AABB lineAABB{ min, max };
	return !camera.isInsideFrustum(lineAABB);
}

} // Engine 