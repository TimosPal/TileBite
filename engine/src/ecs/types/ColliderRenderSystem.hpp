#ifndef COLLIDER_RENDER_SYSTEM_HPP
#define COLLIDER_RENDER_SYSTEM_HPP

#include "ecs/ISystem.hpp"
#include "ecs/types/EngineComponents.hpp"
#include "renderer/Renderer2D.hpp"
#include "utilities/misc.hpp"

#include "physics/aabb.hpp"

namespace Engine {

class ColliderRenderSystem : public ISystem {
public:
	ColliderRenderSystem(std::shared_ptr<Renderer2D> renderer2D) : m_renderer2D(renderer2D) {}

	virtual void update(float deltaTime) override
	{
		const auto& colliders = getSceneManager().getActiveScene()->getPhysicsEngine().getAllColliders();
		for(const auto& colliderPair : colliders)
		{
			glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green color for AABB
			m_renderer2D->drawSquare(colliderPair.second.Min, colliderPair.second.Max, color);
		}
	}

private:
	std::shared_ptr<Renderer2D> m_renderer2D;
};

} // Engine

#endif // !COLLIDER_RENDER_SYSTEM_HPP
