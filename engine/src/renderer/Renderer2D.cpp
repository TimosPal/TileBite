#include "renderer/Renderer2D.hpp"
#include "resources/SystemResourceHub.hpp"

// NOTE: Using OpenGL as the current rendering backend.
#if defined(RENDERER_OPENGL)
    #include "renderer/backend/openGL/GLRenderer2D.hpp"
    using Renderer2DBackend = TileBite::GLRenderer2D;
#else
    #error Unsupported Rendering API
#endif

namespace TileBite {

// TODO: culling not considering rotations in camera
// Could use tree for faster iterating if needed (quadtree, bvh, etc)
// Currently culling is done with the bounding AABB of the OBB of sprite

std::unique_ptr<Renderer2D> Renderer2D::createRenderer2D(SystemResourceHub& systemResourceHub)
{
    return std::make_unique<Renderer2DBackend>(systemResourceHub);
}

bool Renderer2D::shouldCullSpriteQuad(const SpriteQuad& quad, const CameraController& camera) const
{
	// Calculate sprite AABB in world space
	// NOTE: quads are centered arround 0,0 with original size 0.5 per side.

	OBB worldOBB(quad.TransformComp->getPosition(), quad.TransformComp->getSize(), quad.TransformComp->getRotation());
	AABB spriteAABB = worldOBB.getBoundingBox();
	return !camera.isInsideFrustum(spriteAABB);
}

bool Renderer2D::shouldCullTilemap(const TilemapMesh& mesh, const CameraController& camera) const
{
	// Calculate tilemap AABB in world space
	glm::vec2 tilemapMin = mesh.TransformComp->getPosition();
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

void Renderer2D::drawCircle(glm::vec2 center, float radius, glm::vec4 color, int segments) {
	float step = 2.0f * glm::pi<float>() / segments;
	for (int i = 0; i < segments; i++) {
		float a0 = i * step;
		float a1 = (i + 1) * step;

		glm::vec2 p0 = center + radius * glm::vec2(cos(a0), sin(a0));
		glm::vec2 p1 = center + radius * glm::vec2(cos(a1), sin(a1));

		drawLine(Line{ p0, p1, color });
	}
}

void Renderer2D::drawSquare(glm::vec2 min, glm::vec2 max, const glm::vec4& color)
{
	glm::vec2 topLeft = { min.x, max.y };
	glm::vec2 topRight = { max.x, max.y };
	glm::vec2 bottomLeft = { min.x, min.y };
	glm::vec2 bottomRight = { max.x, min.y };
	drawLine({ topLeft, topRight, color });
	drawLine({ topRight, bottomRight, color });
	drawLine({ bottomRight, bottomLeft, color });
	drawLine({ bottomLeft, topLeft, color });
}

void Renderer2D::drawSquare(const std::array<glm::vec2, 4>& corners, const glm::vec4& color)
{
	// corners order: bottom-left, bottom-right, top-right, top-left
	drawLine({ corners[0], corners[1], color });
	drawLine({ corners[1], corners[2], color });
	drawLine({ corners[2], corners[3], color });
	drawLine({ corners[3], corners[0], color });
}

} // TileBite 