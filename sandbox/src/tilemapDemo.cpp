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
    void update(float deltaTime) override
    {
        auto cam = getSceneManager()->getActiveScene()->getCameraController();
        float zoomSpeed = 1.05f;
        cam->setZoom(cam->getZoom() / std::pow(zoomSpeed, deltaTime));
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

                        //ID texID = texIDs[int(quickRandFloat(0.0f, 3.9f))];
                        //auto rngCol = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), 1.0f);
                        auto rngCol = glm::vec4(1, 1, 1, 1.0f);
                        
						auto uvRng = uvs[int(quickRandFloat(0.0f, 3.9f))];
                      
						tile.uIndex = uvRng.x;
						tile.vIndex = uvRng.y;
                        tile.Color = glm::u8vec4(rngCol * 255.0f);
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