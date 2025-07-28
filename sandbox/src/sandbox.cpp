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
#include "ecs/types/BaseComponent.hpp"

using namespace Engine;

struct RigidBody : public BaseComponent {
    float life = 0.0f;
	float currLife = 0.0f;

    RigidBody(float l = 0.0f)
        : life(l), currLife(l) {
    }
};

class TestEvent : public Event {
    SETUP_ID(Event, TestEvent)
public:
    TestEvent() : Engine::Event(true) {}
};

class GravitySystem : public ISystem {
public:
    void onAttach() override
    {
        EventCallback<TestEvent> testCallback([&](TestEvent& event) {
            //LOG_INFO("Test event caught");
        });
        getCoreEventDispatcher().subscribe<TestEvent>(testCallback);
    }

    void update(float deltaTime) override
    {
        auto currScene = getSceneManager().getActiveScene();
        auto& physicsEngine = currScene->getPhysicsEngine();
		auto& world = currScene->getWorld();

        world.query<AABBComponent, TransformComponent, SpriteComponent, RigidBody>().each([&](ID entityID, AABBComponent* aabb, TransformComponent* transform, SpriteComponent* sprite, RigidBody* rb) {
            bool falling = true;
            
            glm::vec2 min = aabb->getMin() * transform->Size + transform->Position;
            glm::vec2 max = aabb->getMax() * transform->Size + transform->Position;
			AABB WorldSpaceAABBComponent{ min, max };

            auto d = physicsEngine.queryCollisions(WorldSpaceAABBComponent);
            for (const auto& id : d)
            {
                if (id == entityID) continue;
                auto tr = world.getComponent<TransformComponent>(id);
                if (tr->Position.y >= transform->Position.y) continue;

                falling = false;
                break;
            }

            if(falling)
            {
				transform->setPosition(transform->Position + glm::vec2(0.0f, -0.9f * deltaTime));
			}
            else
            {
				//rb->currLife -= 0.2 * deltaTime;
    //            transform->Size = transform->Size * (rb->currLife / rb->life);
    //            if (rb->currLife <= 0.0f)
    //            {
				//	world.removeEntity(entityID);
    //            }
            }
		});
    }
};

class BoxSpawnerSystem : public ISystem {
public:
    float timer = 0.0f;
    int c = 0;
    void update(float deltaTime) override
    {
        timer += deltaTime;
        if (timer > 0.02f)
        {
            c++;
            if(c % 1000 == 0) LOG_INFO("Creating box {}", c);
            // Push a test event to the event dispatcher
			// This is just for testing purposes, you can remove it if not needed
			pushEvent(std::make_unique<TestEvent>());

            auto& world = getSceneManager().getActiveScene()->getWorld();
            auto entID = world.createEntity();

			float rX = quickRandFloat(-1.0f, 1.0f);
            float rY = 1.0f;

            glm::vec4 rRGB = glm::vec4(
                quickRandFloat(0.0f, 1.0f),
                quickRandFloat(0.0f, 1.0f),
                quickRandFloat(0.0f, 1.0f),
                1.0f
			);

            auto res = getAssetsManager().getTextureResource("Bee");

            world.addComponents(entID,
                TransformComponent{ {rX, rY}, {0.02f, 0.02f}, 0.0f },
                SpriteComponent{ rRGB, 0},
                AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f }),
                RigidBody{ quickRandFloat(0, 1) * 100 }
            );

            timer = 0.0;
        }

    }
};

class BoxSpawnerSystem2 : public ISystem {
public:
    float timer = 0.0f;
    int c = 0;
    int cl = 5;
    bool adding = true;
	std::vector<ID> entities;

    void update(float deltaTime) override
    {
        timer += deltaTime;

        auto& world = getSceneManager().getActiveScene()->getWorld();
        if (adding)
        {
            if (timer > 0.4f)
            {
                auto ent = world.createEntity();

                // Gen random position size
                float margin = 0.1f;
                auto rngX = quickRandFloat(-1 + margin, 1 - margin);
                auto rngY = quickRandFloat(-1 + margin, 1 - margin);

                world.addComponents(ent,
                    TransformComponent{ {rngX, rngY}, {0.2f, 0.2f}, 0.0f },
                    SpriteComponent{ {1, 1, 1, 1}, 0 },
                    AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
                );

				entities.push_back(ent);
                c++;
                if (c >= cl) adding = false;
                timer = 0.0;
            }
        }
        else
        {
            if (timer > 0.2f)
            {
				world.removeEntity(entities.back());
				entities.pop_back();
                c--;
                if (c <= 0) adding = true;
                timer = 0.0;
            }
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
			AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
        );

        for (size_t i = 0; i < 0; i++)
        {
            auto ent = getWorld().createEntity();

			// Gen random position size
            float margin = 0.1f;
            auto rngX = quickRandFloat(-1 + margin, 1 - margin);
            auto rngY = quickRandFloat(-1 + margin, 1 - margin);

            getWorld().addComponents(ent,
                TransformComponent{ {rngX, rngY}, {0.2f, 0.2f}, 0.0f },
                SpriteComponent{ {1, 1, 1, 1}, 0 },
                AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
            );
        }

        beeTex = getAssetsManager().getTextureResource("Bee");
        beeTex->watch();
        beeTex->load();

		LOG_INFO("Loaded bee texture: {}", beeTex->isLoaded());
        
        getSystemManager().addSystem(std::make_unique<BoxSpawnerSystem>());
		getSystemManager().addSystem(std::make_unique<GravitySystem>());

        getSystemManager().addSystem(std::make_unique<BoxSpawnerSystem2>());
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

        getLayer(DebugLayer::getName())->enable();
        //getLayer(GraphicsLayer::getName())->disable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)