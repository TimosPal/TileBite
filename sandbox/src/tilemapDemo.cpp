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

using namespace Engine;

class CameraSystem : public ISystem {
public:
    float timer = 0;
    float cameraTimer = 0;

    void update(float deltaTime) override
    {
		timer += deltaTime;
		cameraTimer += deltaTime;

        if (cameraTimer < 15)
        {
            auto cam = getSceneManager()->getActiveScene()->getCameraController();
            float zoomSpeed = 1.1f;
            cam->setZoom(cam->getZoom() / std::pow(zoomSpeed, deltaTime));

            auto pos = cam->getPosition();
			pos.x += 0.3 * deltaTime;
			pos.y += 0.3 * deltaTime;
            cam->setPosition(pos);
        }

        if (timer > 0.0001f)
        {
            timer = 0;

            auto tilemapHandle = getAssetsManager()->getTilemapResource("tileMapResource_0_0");
			auto rngA = quickRandFloat(0.0f, 1.0f) < 0.4 ? 1 : 0.1;
            auto rngCol = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), rngA);

            auto rngX = quickRandFloat(0, 254.9 - 5);
            auto rngY = quickRandFloat(0, 254.9 - 5);

            glm::vec2 uvs[] = { {0,0}, {0,1}, {1,0}, {1,1} };
            auto uvRng = uvs[int(quickRandFloat(0.0f, 3.9f))];

            for (size_t i = 0; i < 5; i++)
            {
                for (size_t j = 0; j < 5; j++)
                {
                    tilemapHandle.getResource()->setTile(
                        Tile{ rngCol, (uint8_t)uvRng.x, (uint8_t)uvRng.y }, // Example tile with red color
                        rngX + i, // xIndex
                        rngY + j  // yIndex
                    );
                }
            }
        }
    }
};

class MainScene : public Scene {
    std::unique_ptr<IResourceHandle> m_tilemapTextureHandle;
    ResourceHandle<TilemapResource> m_tilemapHandle;

    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);
        setCameraController(cameraController);

        m_tilemapTextureHandle = getAssetsManager()->getTextureResource("tilemap");
        m_tilemapTextureHandle->watch();
        m_tilemapTextureHandle->load();

		addSystem(std::make_unique<CameraSystem>());

		glm::vec2 uvs[] = { {0,0}, {0,1}, {1,0}, {1,1} };

        float posX = 0;
        float posY = 0;
        for (size_t i = 0; i < 1; i++)
        {
            for (size_t j = 0; j < 1; j++)
            {
                int tilemapHeight = 255;
                int tilemapWidth = 255;
                std::vector<Tile> tiles;
                tiles.resize(tilemapHeight * tilemapWidth);
                for (size_t y = 0; y < tilemapHeight; y++)
                {
                    for (size_t x = 0; x < tilemapWidth; x++)
                    {
                        Tile& tile = tiles[y * tilemapWidth + x];

                        auto aC = 1; // quickRandFloat(0.0f, 1.0f) < 0.4 ? 1 : 0;
                        auto rngCol = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), aC);                        
						auto uvRng = uvs[int(quickRandFloat(0.0f, 3.9f))];
                      
						tile.uIndex = uvRng.x;
						tile.vIndex = uvRng.y;
                        tile.Color = rngCol;
                    }
                }
                std::string resourceName = "tileMapResource_" + std::to_string(i) + "_" + std::to_string(j);
                m_tilemapHandle = getAssetsManager()->createTilemapResource(
                    resourceName,
                    tiles,
                    { tilemapWidth, tilemapHeight },
                    { 0.1, 0.1 },
                    { 16, 16 },
                    {32, 32},
                    m_tilemapTextureHandle->getID()
                );

                ID tilemap = getWorld().createEntity();
                TilemapComponent tilemapComp;
                tilemapComp.TilemapResource = m_tilemapHandle.getResource();

                getWorld().addComponents(
                    tilemap,
                    TransformComponent{ glm::vec2(posX, posY), glm::vec2(1, 1) },
                    std::move(tilemapComp)
                );

                posX += 0.15f;
            }
            posX = -1;
            posY += 0.15f;
        }
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
        getAssetsManager().createTextureResource("tilemap", std::string(ResourcePaths::ImagesDir) + "./tilemap.png");
       
        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)