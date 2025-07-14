#include <iostream>
#include <cmath>
#include <random>

#include <core/EntryPoint.hpp>
#include <core/EngineApp.hpp>

#include <utilities/Logger.hpp>
#include <events/types/WindowCloseEvent.hpp>
#include <layers/Layer.hpp>
#include <ecs/types/EngineComponents.hpp>
#include <core/ResourceRegistry.hpp>
#include <physics/AABB.hpp>

using namespace Engine;

struct RigidBody {};

class GravitySystem : public ISystem {
public:
    void update(float deltaTime) override
    {
        auto currScene = getSceneManager()->getActiveScene();
        auto& physicsEngine = currScene->getPhysicsEngine();
		auto& world = currScene->getWorld();

        world.query<AABB, TransformComponent, SpriteComponent, RigidBody>().each([&](ID entityID, AABB* aabb, TransformComponent* transform, SpriteComponent* sprite, RigidBody* rb) {
            bool falling = true;
            
            glm::vec2 min = aabb->Min * transform->Size + transform->Position;
            glm::vec2 max = aabb->Max * transform->Size + transform->Position;
			AABB WorldSpaceAABB{ min, max };

            auto d = physicsEngine.queryCollisions(WorldSpaceAABB);
            for (auto i : d)
            {
                if (i.id == entityID) continue;
                falling = false;
                break;
            }

            if(falling)
            {
                transform->Position.y -= 1.0f * deltaTime;
			}
		});
    }
};

class BoxSpawnerSystem : public ISystem {
public:

    float timer = 0.0f;
    void update(float deltaTime) override
    {
        timer += deltaTime;
        if (timer > 0.05f)
        {
            auto& world = getSceneManager()->getActiveScene()->getWorld();
            auto entID = world.createEntity();

			float rX = quickRandFloat(-0.8f, 0.8f);
            float rY = 1.0f;

            glm::vec4 rRGB = glm::vec4(
                quickRandFloat(0.0f, 1.0f),
                quickRandFloat(0.0f, 1.0f),
                quickRandFloat(0.0f, 1.0f),
                1.0f
			);

            auto res = getAssetsManager()->getTextureResource("Bee");

            world.addComponents(entID,
                TransformComponent{ {rX, rY}, {0.04f, 0.04f}, 0.0f },
                SpriteComponent{ rRGB, res->getID()},
                AABB({ -0.5f, -0.5f }, { 0.5f, 0.5f }),
                RigidBody{}
            );

            timer = 0.0;
        }

    }
};

class MainScene : public Scene {
    std::unique_ptr<IResourceHandle> beeTex;

    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);
        setCameraController(cameraController);

		auto floor = getWorld().createEntity();
        getWorld().addComponents(floor,
            TransformComponent{ {0.0f, -0.9f}, {2.0f, 0.3f}, 0.0f },
			SpriteComponent{ {1, 1, 1, 1}, 0 },
			AABB({ -0.5f, -0.5f }, { 0.5f, 0.5f })
        );

        beeTex = getAssetsManager()->getTextureResource("Bee");
        beeTex->watch();
        beeTex->load();

		LOG_INFO("Loaded bee texture: {}", beeTex->isLoaded());

		addSystem(std::make_unique<BoxSpawnerSystem>());
		addSystem(std::make_unique<GravitySystem>());
    }
};

class GameLayer : public Layer {
public:

    void onAttach() override
    {
        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "Testing demos"); }

    void setup() override
    {
        // load bee texture
        getAssetsManager().createTextureResource("Bee", ResourcePaths::ImagesDir + std::string("bee.png"));

        pushLayer(std::make_unique<GameLayer>());

        //getLayer(DebugLayer::getName())->enable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)