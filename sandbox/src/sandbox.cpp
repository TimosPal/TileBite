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

inline float quickRandFloat(float min = -1.0f, float max = 1.0f) {
    return min + (max - min) * (rand() / float(RAND_MAX));
}

struct VelocityComponent {
    float vx, vy;
};

class UnitSystem : public ISystem {
public:
    float spawnTimer = 0.0f;
    float currentAngle = 0.0f;
    const float angleStep = 10.0f;  // radians per spawn
    const float circleRadius = 0.5f;
    const float speed = 0.5f;
    const int maxSpawns = 10000;
    int spawnCount = 0;

    void update(float deltaTime) override
    {
        // Move and bounce units
        getWorld()->query<TransformComponent, VelocityComponent>().each([deltaTime](TransformComponent* t, VelocityComponent* v) {
            t->Position.x += v->vx * deltaTime;
            t->Position.y += v->vy * deltaTime;

            bool bounced = false;
            if (t->Position.x < -1.0f || t->Position.x > 1.0f) {
                t->Position.x = std::clamp(t->Position.x, -1.0f, 1.0f);
                v->vx *= -1.0f;
				bounced = true;
            }
            if (t->Position.y < -1.0f || t->Position.y > 1.0f) {
                t->Position.y = std::clamp(t->Position.y, -1.0f, 1.0f);
                v->vy *= -1.0f;
                bounced = true;
            }

			if (bounced) {
                float r = quickRandFloat(-0.2f, 0.2f);
                t->Size.y -= t->Size.y * r;
                t->Size.x -= t->Size.x * r;
			}

            t->Rotation += 1 * deltaTime;
        });

        // Spawn logic every 0.01 seconds
        spawnTimer += deltaTime;
        if (spawnTimer >= 0.001f && spawnCount < maxSpawns) {
            spawnTimer = 0.0f;
            spawnCount++;

            if (spawnCount % 1000 == 0) LOG_INFO("Count: {}", spawnCount);

            // Spawn at center
            float x = 0.0f;
            float y = 0.0f;

            // Velocity radially outward
            float vx = speed * cos(currentAngle);
            float vy = speed * sin(currentAngle);

            // Random color
            float r = quickRandFloat(0.0f, 1.0f);
            float g = quickRandFloat(0.0f, 1.0f);
            float b = quickRandFloat(0.0f, 1.0f);

            float size = 0;

            float textureRNG = quickRandFloat(0.0f, 1.0f);
            ID textureID;
            if (textureRNG < 0.3f)
            {
                textureID = 0;
                size = 0.15f;
            }
            else if (textureRNG < 0.5f)
            {
                textureID = getAssetsManager()->getTexture("ball");
                size = 0.2f;
            }
            else if (textureRNG < 0.8f)
            {
                textureID = getAssetsManager()->getTexture("bee");
                size = 0.1f;
            }
            else
            {
                textureID = 999;
                size = 0.01f;
            }

            ID unit = getWorld()->createEntity();
            getWorld()->addComponents(
                unit,
                SpriteComponent{ glm::vec4(r, g, b, 1.0f), textureID},
                TransformComponent{ glm::vec2(x, y), glm::vec2(size, size)},
                VelocityComponent{ vx, vy }
            );
            
            currentAngle += angleStep * deltaTime;
            if (currentAngle > 2.0f * 3.1415926f)
                currentAngle -= 2.0f * 3.1415926f;
        }
    }
};

class OrbitSystem : public ISystem {
public:
    float spawnTimer = 0.0f;
    int spawnCount = 0;
    const int maxSpawns = 30000;
    float time = 0;

    void update(float deltaTime) override
    {
        time += deltaTime;

        // Update orbits
        getWorld()->query<TransformComponent, VelocityComponent>().each([this, deltaTime](TransformComponent* t, VelocityComponent* v) {
            float angle = atan2(t->Position.y, t->Position.x);
            float radius = sqrt(t->Position.x * t->Position.x + t->Position.y * t->Position.y);

            angle += v->vx * deltaTime; // vx used as angular speed

            radius += 0.05f * sin(time + radius * 5.0f) * deltaTime; // pulsating orbit

            t->Position.x = radius * cos(angle);
            t->Position.y = radius * sin(angle);

            float pulse = 1.0f + 0.3f * sin(time * 4.0f + radius * 10.0f);
            t->Size.x = t->Size.y = v->vy * pulse; // vy used as base size

            t->Rotation += 0.5f * deltaTime;
        });

        // Spawn more if needed
        spawnTimer += deltaTime;
        if (spawnTimer > 0.01f && spawnCount < maxSpawns) {
            spawnTimer = 0.0f;
            spawnCount++;

            float angle = quickRandFloat(0.0f, 2.0f * 3.1415926f);
            float radius = quickRandFloat(0.2f, 0.9f);
            float size = quickRandFloat(0.02f, 0.06f);
            float speed = quickRandFloat(0.5f, 2.0f);

            float r = quickRandFloat(0.2f, 1.0f);
            float g = quickRandFloat(0.2f, 1.0f);
            float b = quickRandFloat(0.2f, 1.0f);

            ID textureID = (quickRandFloat() < 0.5f)
                ? getAssetsManager()->getTexture("bee")
                : getAssetsManager()->getTexture("ball");

            ID unit = getWorld()->createEntity();
            getWorld()->addComponents(
                unit,
                SpriteComponent{ glm::vec4(r, g, b, 1.0f), textureID },
                TransformComponent{ glm::vec2(radius * cos(angle), radius * sin(angle)), glm::vec2(size, size) },
                VelocityComponent{ speed, size } // vx = angular speed, vy = size
            );
        }
    }
};

class GameLayer : public Layer {
public:
    void onAttach() override
    {
        //addSystem(std::make_unique<OrbitSystem>());
        addSystem(std::make_unique<UnitSystem>());
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(800, 600, "Balls!"); }

    void setup() override
    {
        getAssetsManager().createTexture("bee", std::string(ResourcePaths::ImagesDir) + "./bee.png");
        getAssetsManager().createTexture("ball", std::string(ResourcePaths::ImagesDir) + "./ball.png");

        pushLayer(std::make_unique<GameLayer>());
    }
};

CREATE_ENGINE_APPLICATION(MyApp)