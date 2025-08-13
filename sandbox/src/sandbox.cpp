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
#include <events/types/MouseEvent.hpp>
#include <events/types/KeyEvent.hpp>
#include <window/KeyCodes.hpp>

using namespace Engine;

class MovingBoxSystem : public ISystem {
private:
    ID box;
    bool ping = false;
public:
    void onAttach() override
    {
        auto& world = getSceneManager().getActiveScene()->getWorld();
        box = world.createEntity();
        world.addComponents(
            box,
            TransformComponent{ glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.2f) },
            SpriteComponent(glm::vec4(0.7f, 0.0f, 0.0f, 1.0f), 0),
            AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
        );
	}

    void update(float deltaTime) override
    {
        auto& world = getSceneManager().getActiveScene()->getWorld();
        auto tr = world.getComponent<TransformComponent>(box);
		
		glm::vec2 dir = ping ? glm::vec2(-1.0f, 0.0f) : glm::vec2(1.0f, 0.0f);
        tr->setPosition(tr->Position + dir * deltaTime * 0.3f);

        if (ping && tr->Position.x < -2.5)
        {
            ping = false;
        }
        else if (!ping && tr->Position.x > 1)
        {
            ping = true;
        }

		AABBComponent* aabb = world.getComponent<AABBComponent>(box);

        glm::vec2 min = aabb->getMin() * tr->Size + tr->Position;
        glm::vec2 max = aabb->getMax() * tr->Size + tr->Position;
        AABB WorldSpaceAABBComponent{ min, max };
		PhysicsEngine& physicsEngine = getSceneManager().getActiveScene()->getPhysicsEngine();
        auto collisionData = physicsEngine.queryCollisions(WorldSpaceAABBComponent, box);

		auto& renderer = getRenderer();
        if (!collisionData.empty())
        {
            for (auto& data : collisionData)
            {
                glm::vec2 min = data.Collider.Min;
				glm::vec2 max = data.Collider.Max;
                renderer.drawLine({ min, max, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) });
                renderer.drawLine({ glm::vec2(min.x, max.y), glm::vec2(max.x, min.y), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) });
            }
        }
    }
};

class CameraSystem : public ISystem {
public:
    void update(float deltaTime) override
    {
        auto cam = getSceneManager().getActiveScene()->getCameraController();
		auto camPos = cam->getPosition();
	    glm::vec2 baseVec = glm::vec2(0.0f, 0.0f);
        if(getInputManager().isKeyDown(KeyCodes::KEY_A))
        {
			baseVec += glm::vec2(-1.0f, 0.0f);
		}
        if(getInputManager().isKeyDown(KeyCodes::KEY_D))
		{
            baseVec += glm::vec2(1.0f, 0.0f);
        }
        if (getInputManager().isKeyDown(KeyCodes::KEY_W))
        {
            baseVec += glm::vec2(0.0f, 1.0f);
        }
        if (getInputManager().isKeyDown(KeyCodes::KEY_S))
        {
            baseVec += glm::vec2(0.0f, -1.0f);
        }
        cam->setPosition(camPos + baseVec * deltaTime * 1.0f);

        float scrollY = getInputManager().getMouseScrollY();
        if(scrollY != 0.0f)
        {
            float delta = scrollY * 15.0f * deltaTime;
            float newZoom = std::clamp(cam->getZoom() + delta, 0.5f, 3.0f);
            cam->setZoom(newZoom);
		}
    }
};

class MainScene : public Scene {
    std::vector<ResourceHandle<TilemapResource>> m_tilemapHandles;

    void createTilemap(float x, float y, int size)
    {
        int tilemapHeight = size;
        int tilemapWidth = size;
        std::vector<Tile> tiles;
        tiles.resize(tilemapHeight * tilemapWidth);
        for (size_t y = 0; y < tilemapHeight; y++)
        {
            for (size_t x = 0; x < tilemapWidth; x++)
            {
                Tile& tile = tiles[y * tilemapWidth + x];

                auto aC = quickRandFloat(0.0f, 1.0f) < 0.5 ? 1 : 0;
                auto rngCol = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), aC);

                tile.uIndex = 0;
                tile.vIndex = 0;
                tile.Color = rngCol;

                /*if (aC == 1)
                {
                    ID collider = getWorld().createEntity();
                    getWorld().addComponents(
                        collider,
                        TransformComponent{ glm::vec2(x * 0.1f, y * 0.1f), glm::vec2(1.0f, 1.0f) },
                        AABBComponent{ glm::vec2(0,0), glm::vec2(0.1f, 0.1f) }
                    );
                }*/
            }
        }

        static int i = 0;
        std::string resourceName = "tileMapResource_" + std::to_string(i++);
        ResourceHandle<TilemapResource> tilemapHandle = getAssetsManager().createTilemapResource(
            resourceName,
            tiles,
            { tilemapWidth, tilemapHeight },
            { 0.1, 0.1 },
            { 16, 16 },
            { 32, 32 },
            0
        );

        m_tilemapHandles.emplace_back(std::move(tilemapHandle));

        ID tilemap = getWorld().createEntity();
        TilemapComponent tilemapComp;
        tilemapComp.TilemapResourcePtr = m_tilemapHandles.back().getResource();

        getWorld().addComponents(
            tilemap,
            TransformComponent{ glm::vec2(x, y) },
            std::move(tilemapComp)
        );
    }

    void onLoad() override
    {
		float aspect = (float)getWindow().getWidth() / getWindow().getHeight();
        auto cameraController = std::make_shared<CameraController>();
        setCameraController(cameraController);

		createTilemap(0.0f, 0.0f, 7);
		createTilemap(-0.8f, 0.0f, 6);
        createTilemap(-1.6f, 0.0f, 5);
        createTilemap(-2.2f, 0.0f, 4);
    }
};

class GameLayer : public Layer {
public: 
    void onAttach() override
    {
        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);

        EventCallback<KeyPressedEvent> e([&](KeyPressedEvent& event) {
            LOG_INFO("Key: {}", event.getKeyCode());
		});
        getCoreEventDispatcher().subscribe(e);

		getSystemManager().addSystem(std::make_unique<CameraSystem>());
		getSystemManager().addSystem(std::make_unique<MovingBoxSystem>());
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "sandbox"); }

    void setup() override
    {     
        pushLayer(std::make_unique<GameLayer>());
        
        getLayer(DebugLayer::getName())->enable();
        //getLayer(GraphicsLayer::getName())->disable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)