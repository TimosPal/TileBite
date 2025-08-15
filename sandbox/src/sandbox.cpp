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

#include <FastNoiseLite.h>

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
            TransformComponent{ glm::vec2(0.0f, 0.1f), glm::vec2(0.3f, 0.3f) },
            SpriteComponent(glm::vec4(0.7f, 0.0f, 0.0f, 1.0f), 0),
            AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
        );
	}

    void update(float deltaTime) override
    {
        auto& world = getSceneManager().getActiveScene()->getWorld();
        auto tr = world.getComponent<TransformComponent>(box);
		
		glm::vec2 dir = ping ? glm::vec2(-1.0f, -0.2f) : glm::vec2(1.0f, 0.3f);
		dir += glm::vec2(0.0f, sin(tr->Position.x * 7.0f) * 1.0f);
        tr->setPosition(tr->Position + dir * deltaTime * 0.3f);

        if (ping && tr->Position.x < -2.5)
        {
            ping = false;
        }
        else if (!ping && tr->Position.x > 4)
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
                /*glm::vec2 min = data.Generic.Collider.Min;
				glm::vec2 max = data.Generic.Collider.Max;
				glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                renderer.drawLine({ min, max, color });
                renderer.drawLine({ glm::vec2(min.x, max.y), glm::vec2(max.x, min.y), color });
                renderer.drawSquare(min, max, color);*/

                if (data.type == CollisionData::Type::TilemapType)
                {
                    Tile newTile;
                    newTile.IsSolid = false;
                    newTile.Color = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), 0.0f);
                    world.getComponent<TilemapComponent>(data.Generic.id)->setTile(
                        newTile,
                        data.Tilemap.XTilemapIndex,
                        data.Tilemap.YTilemapIndex
                    );
                }
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
    FastNoiseLite baseNoise;
    FastNoiseLite detailNoise;
	float tileSize = 0.01f;

    void createTilemap(float x, float y, int size, int noiseOffset)
    {
        ID tilemap = getWorld().createEntity();

        int tilemapWidth = size;
        int tilemapHeight = size;
        std::vector<Tile> tiles(tilemapWidth * tilemapHeight);

        for (int tx = 0; tx < tilemapWidth; tx++)
        {
            // 1D terrain height for this column
            float n = 0.7f * baseNoise.GetNoise((float)tx + noiseOffset, 0.0f) +
                0.1f * detailNoise.GetNoise((float)tx + noiseOffset, 0.0f);
            int terrainHeight = static_cast<int>(0.5f * (n + 1.0f) * tilemapHeight);

            for (int ty = 0; ty < tilemapHeight; ty++)
            {
                Tile& tile = tiles[ty * tilemapWidth + tx];

                glm::vec4 color;
                bool solid = true;

                if (ty > terrainHeight)
                {
                    solid = false;
                    color = { 0.5f, 0.8f, 1.0f, 0.0f };
                }
                else
                {
                    if (ty > terrainHeight - 5) color = { 0.6f, 0.8f, 0.4f, 1.0f };
                    else if (ty > terrainHeight * 0.6f) color = { 0.7f, 0.6f, 0.4f, 1.0f };
                    else color = { 0.3f, 0.3f, 0.4f, 1.0f };
                }

                tile.uIndex = 0;
                tile.vIndex = 0;
                tile.Color = color;
                tile.IsSolid = solid;
            }
        }

        static int i = 0;
        std::string resourceName = "tileMapResource_" + std::to_string(i++);
        ResourceHandle<TilemapResource> tilemapHandle = getAssetsManager().createTilemapResource(
            resourceName,
            tiles,
            { tilemapWidth, tilemapHeight },
            { tileSize, tileSize },
            { 16, 16 },
            { 32, 32 },
            0
        );

        m_tilemapHandles.emplace_back(std::move(tilemapHandle));

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

        baseNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        baseNoise.SetFrequency(0.01f); // low-frequency hills

        detailNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        detailNoise.SetFrequency(0.05f); // medium-frequency detail

        float tilemapStartingX = 0.0f;
        int tilemapSize = 255;
        for (size_t i = 0; i < 3; i++)
        {
            createTilemap(tilemapStartingX, 0.0f, tilemapSize, tilemapSize * i);
            tilemapStartingX += tilemapSize * tileSize;
        }
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

        //getLayer(DebugLayer::getName())->enable();
        //getLayer(GraphicsLayer::getName())->disable();
    }
};

CREATE_ENGINE_APPLICATION(MyApp)