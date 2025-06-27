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

class MainScene : public Scene {
    std::unique_ptr<IResourceHandle> m_ballHandle;
    std::unique_ptr<IResourceHandle> m_beeHandle;
    ResourceHandle<TilemapResource> m_tilemapHandle;

    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>(-1.0f, 1.0f, -1.0f, 1.0f);
        setCameraController(cameraController);

        m_ballHandle = getAssetsManager()->getTextureResource("ball");
        m_beeHandle = getAssetsManager()->getTextureResource("bee");

        m_ballHandle->watch();
        m_ballHandle->load();
        m_beeHandle->watch();
        m_beeHandle->load();

        ID texIDs[] = {
            m_ballHandle->getID(),
            m_beeHandle->getID(),
            0,
            4
        };

        float posX = -1;
        float posY = -1;
        for (size_t i = 0; i < 10; i++)
        {
            for (size_t j = 0; j < 10; j++)
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
                        auto rngCol = glm::vec4(quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), quickRandFloat(0.4f, 1.0f), 1.0f);
                        tile.uIndex = 0;
                        tile.vIndex = 0;
                        tile.Color = glm::u8vec4(rngCol * 255.0f);
                    }
                }
                std::string resourceName = "tileMapResource_" + std::to_string(i) + "_" + std::to_string(j);
                m_tilemapHandle = getAssetsManager()->createTilemapResource(resourceName, tiles, { tilemapWidth, tilemapHeight }, 1, 1, 0);

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
        getAssetsManager().createTextureResource("bee", std::string(ResourcePaths::ImagesDir) + "./bee.png");
        getAssetsManager().createTextureResource("ball", std::string(ResourcePaths::ImagesDir) + "./ball.png");

        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)