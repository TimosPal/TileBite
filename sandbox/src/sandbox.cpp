#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>

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

#include <physics/Ray2D.hpp>
#include <FastNoiseLite.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace Engine;

/*------------------------------------------------------------------------------
    Small helpers
------------------------------------------------------------------------------*/

static inline float smooth_to(float current, float target, float sharpness, float dt) {
    // Exponential smoothing toward target; higher sharpness -> faster convergence
    return current + (target - current) * (1.0f - std::exp(-sharpness * dt));
}

static glm::vec4 hsv_to_rgba(float h, float s, float v, float a = 1.0f) {
    // h in [0,1), s,v in [0,1]
    h = std::fmod(std::max(0.0f, h), 1.0f) * 6.0f;
    int i = int(h);
    float f = h - float(i);
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    glm::vec3 rgb;
    switch (i) {
        case 0: rgb = {v, t, p}; break;
        case 1: rgb = {q, v, p}; break;
        case 2: rgb = {p, v, t}; break;
        case 3: rgb = {p, q, v}; break;
        case 4: rgb = {t, p, v}; break;
        default: rgb = {v, p, q}; break;
    }
    return {rgb.r, rgb.g, rgb.b, a};
}

/*------------------------------------------------------------------------------
    MovingBoxSystem
    - Spawns a batch of boxes with AABB + Sprite + Transform
    - Optional noise-based wandering + color pulsing
    - Draws a sweeping ray + hit debug
    - Toggles and spawn/clear using hotkeys
    - Draws a subtle world-space grid
------------------------------------------------------------------------------*/

class MovingBoxSystem : public ISystem {
private:
    struct BoxMeta {
        glm::vec2 basePos{};
        float jitter = 0.0f;      // per-box phase
        glm::vec4 baseColor{0.7f, 0.0f, 0.0f, 1.0f};
    };

    float timer = 0.0f;
    std::mt19937 rng{std::random_device{}()};

    // noise configuration (for wandering movement)
    FastNoiseLite noise;

    // spawned entities and metadata (kept in sync)
    std::vector<ID> boxes;
    std::vector<BoxMeta> meta;

    // toggles
    bool drawGrid = true;
    bool drawRay = true;
    bool pulse = true;
    bool noiseMotion = true;

    // one-frame latch helpers for keys
    struct KeyLatch {
        bool wasDown = false;
        bool pressed(const InputManager& im, KeyCodes key) {
            bool now = im.isKeyDown(key);
            bool fire = now && !wasDown;
            wasDown = now;
            return fire;
        }
    };
    KeyLatch latchSpawn, latchClear, latchGrid, latchRay, latchPulse, latchNoise;

    // spawn helpers
    glm::vec2 random_pos() {
        return glm::vec2(
            quickRandFloat(-1.0f, 1.0f),
            quickRandFloat(-1.0f, 1.0f)
        );
    }
    glm::vec4 random_color() {
        float h = quickRandFloat(0.0f, 1.0f);
        return hsv_to_rgba(h, 0.75f, 0.9f, 1.0f);
    }

    void spawn_boxes(World& world, size_t count, float size = 0.1f) {
        for (size_t i = 0; i < count; ++i) {
            const glm::vec2 p = random_pos();
            const glm::vec4 c = random_color();

            ID box = world.createEntity();
            world.addComponents(
                box,
                TransformComponent{ p, glm::vec2(size, size) },
                SpriteComponent(c, 0),
                AABBComponent({ -0.5f, -0.5f }, { 0.5f, 0.5f })
            );

            boxes.push_back(box);
            meta.push_back(BoxMeta{
                .basePos = p,
                .jitter  = quickRandFloat(0.0f, 1000.0f),
                .baseColor = c
            });
        }
    }

    void clear_boxes(World& world) {
        for (ID id : boxes) {
            world.destroyEntity(id);
        }
        boxes.clear();
        meta.clear();
    }

    void draw_world_grid(Renderer& r) {
        // Simple grid in world space around origin
        const float range = 2.0f;
        const float step  = 0.2f;
        const glm::vec4 major(0.25f, 0.25f, 0.25f, 1.0f);
        const glm::vec4 minor(0.17f, 0.17f, 0.17f, 1.0f);

        // axes
        r.drawLine(Line{ { -range, 0.0f }, { range, 0.0f }, glm::vec4(0.35f, 0.35f, 0.35f, 1.0f) });
        r.drawLine(Line{ { 0.0f, -range }, { 0.0f, range }, glm::vec4(0.35f, 0.35f, 0.35f, 1.0f) });

        // grid
        for (float x = -range; x <= range + 1e-5f; x += step) {
            glm::vec4 col = (std::abs(std::fmod(x, 1.0f)) < 1e-3f) ? major : minor;
            r.drawLine(Line{ { x, -range }, { x, range }, col });
        }
        for (float y = -range; y <= range + 1e-5f; y += step) {
            glm::vec4 col = (std::abs(std::fmod(y, 1.0f)) < 1e-3f) ? major : minor;
            r.drawLine(Line{ { -range, y }, { range, y }, col });
        }
    }

public:
    void onAttach() override {
        auto& world = getSceneManager().getActiveScene()->getWorld();

        // set up noise
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetFrequency(0.15f);
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFractalOctaves(3);

        // initial content
        spawn_boxes(world, 15, 0.10f);
    }

    void update(float deltaTime) override {
        timer += deltaTime;

        // toggles & actions
        const auto& im = getInputManager();
        auto& world = getSceneManager().getActiveScene()->getWorld();
        if (latchGrid.pressed(im, KeyCodes::KEY_F1))  { drawGrid    = !drawGrid;    LOG_INFO("Grid: {}",    drawGrid ? "on" : "off"); }
        if (latchRay.pressed(im,  KeyCodes::KEY_F2))  { drawRay     = !drawRay;     LOG_INFO("Ray: {}",     drawRay ? "on" : "off"); }
        if (latchNoise.pressed(im,KeyCodes::KEY_F3))  { noiseMotion = !noiseMotion; LOG_INFO("Noise: {}",   noiseMotion ? "on" : "off"); }
        if (latchPulse.pressed(im,KeyCodes::KEY_F4))  { pulse       = !pulse;       LOG_INFO("Pulse: {}",   pulse ? "on" : "off"); }
        if (latchSpawn.pressed(im,KeyCodes::KEY_SPACE)) { spawn_boxes(world, 5); }
        if (latchClear.pressed(im,KeyCodes::KEY_BACKSPACE)) { clear_boxes(world); }

        auto& renderer = getRenderer();
        if (drawGrid) draw_world_grid(renderer);

        // animate boxes (noise wander + color pulse)
        if (!boxes.empty()) {
            const float amp = 0.35f;
            const float speed = 0.7f;

            for (size_t i = 0; i < boxes.size(); ++i) {
                ID id = boxes[i];

                // Defensive check: skip if entity was removed externally
                if (!world.isAlive(id)) continue;

                auto& tr = world.getComponent<TransformComponent>(id);
                auto& sp = world.getComponent<SpriteComponent>(id);

                if (noiseMotion) {
                    float nx = noise.GetNoise((timer * speed) + meta[i].jitter, 23.7f);
                    float ny = noise.GetNoise(11.3f, (timer * speed) + meta[i].jitter);
                    glm::vec2 wobble = glm::vec2(nx, ny) * amp;
                    tr.position = meta[i].basePos + wobble;
                }

                if (pulse) {
                    // Pulse brightness between 70% and 100% with a slight per-box phase
                    float s = 0.85f + 0.15f * std::sin(timer * 2.0f + meta[i].jitter * 0.1f);
                    sp.color = glm::vec4(meta[i].baseColor.r * s,
                                         meta[i].baseColor.g * s,
                                         meta[i].baseColor.b * s,
                                         1.0f);
                }
            }
        }

        // animated sweeping ray + debug
        if (drawRay) {
            Ray2D ray(glm::vec2(-2.0f, 0.0f),
                      glm::vec2(1.0f, std::sin(timer * 0.3f) * 0.6f),
                      1.5f);

            renderer.drawLine(
                Line { ray.at(0.0f), ray.at(ray.getMaxT()), glm::vec4(1.0f) }
            );

            PhysicsEngine& physics = getSceneManager().getActiveScene()->getPhysicsEngine();
            std::optional<RayHitData> hit = physics.raycastClosest(ray);
            if (hit.has_value()) {
                const RayHitData& h = hit.value();
                // green for hit span, blue up to first hit
                renderer.drawLine(Line{ ray.at(h.tmin), ray.at(h.tmax), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) });
                renderer.drawLine(Line{ ray.at(0.0f),   ray.at(h.tmin), glm::vec4(0.0f, 0.4f, 1.0f, 1.0f) });
                // mark hit endpoints
                renderer.drawSquare(ray.at(h.tmin) - 0.01f, ray.at(h.tmin) + 0.01f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                renderer.drawSquare(ray.at(h.tmax) - 0.01f, ray.at(h.tmax) + 0.01f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
            }
        }
    }
};

/*------------------------------------------------------------------------------
    CameraSystem
    - Smooth WASD panning with acceleration/damping
    - Shift = fast, Ctrl = slow
    - Scroll wheel -> target zoom with smooth interpolation and clamping
------------------------------------------------------------------------------*/

class CameraSystem : public ISystem {
    glm::vec2 velocity{0.0f};
    float targetZoom = 1.0f;

    // Tunables
    float accel   = 8.0f;   // higher -> snappier
    float damping = 8.0f;   // higher -> more damping
    float baseSpeed = 1.8f; // world units per second
    float zoomSharpness = 10.0f;

public:
    void onAttach() override
    {
        auto cam = getSceneManager().getActiveScene()->getCameraController();
        targetZoom = cam->getZoom();
    }

    void update(float deltaTime) override
    {
        auto cam = getSceneManager().getActiveScene()->getCameraController();
        glm::vec2 pos = cam->getPosition();

        // directional input
        glm::vec2 axis(0.0f);
        const auto& im = getInputManager();
        if (im.isKeyDown(KeyCodes::KEY_A)) axis.x -= 1.0f;
        if (im.isKeyDown(KeyCodes::KEY_D)) axis.x += 1.0f;
        if (im.isKeyDown(KeyCodes::KEY_W)) axis.y += 1.0f;
        if (im.isKeyDown(KeyCodes::KEY_S)) axis.y -= 1.0f;

        // speed modifiers
        float speedMul = 1.0f;
        if (im.isKeyDown(KeyCodes::KEY_LEFT_SHIFT) || im.isKeyDown(KeyCodes::KEY_RIGHT_SHIFT))
            speedMul *= 2.0f;
        if (im.isKeyDown(KeyCodes::KEY_LEFT_CONTROL) || im.isKeyDown(KeyCodes::KEY_RIGHT_CONTROL))
            speedMul *= 0.5f;

        // compute target velocity in world space
        glm::vec2 targetVel = glm::normalize(axis) * (axis == glm::vec2(0) ? 0.0f : baseSpeed * speedMul);

        // critically damped-ish smoothing toward target velocity
        velocity.x = smooth_to(velocity.x, targetVel.x, accel, deltaTime);
        velocity.y = smooth_to(velocity.y, targetVel.y, accel, deltaTime);
        velocity   = velocity * std::exp(-damping * std::max(0.0f, deltaTime));

        // integrate camera
        pos += velocity * deltaTime;
        cam->setPosition(pos);

        // zoom control (smooth toward target)
        float scrollY = im.getMouseScrollY();
        if (scrollY != 0.0f) {
            float delta = scrollY * 0.9f; // sensitivity
            targetZoom = std::clamp(targetZoom + delta, 0.5f, 3.0f);
        }

        float z = cam->getZoom();
        z = smooth_to(z, targetZoom, zoomSharpness, deltaTime);
        cam->setZoom(z);
    }
};

/*------------------------------------------------------------------------------
    Scene & Layer glue
------------------------------------------------------------------------------*/

class MainScene : public Scene {
    void onLoad() override
    {
        auto cameraController = std::make_shared<CameraController>();
        setCameraController(cameraController);
    }
};

class GameLayer : public Layer {
public:
    void onAttach() override
    {
        auto scene = getSceneManager().createScene<MainScene>("MainScene");
        getSceneManager().setActiveScene(scene);

        getSystemManager().addSystem(std::make_unique<CameraSystem>());
        getSystemManager().addSystem(std::make_unique<MovingBoxSystem>());
    }

    // Optional: demonstrate event hook (kept simple for portability)
    void onEvent(Event& e) override
    {
        // Example: close on Escape via event stream (engine might already do this)
        if (e.isInCategory(EventCategoryKeyboard)) {
            auto* ke = dynamic_cast<KeyPressedEvent*>(&e);
            if (ke && ke->getKeyCode() == KeyCodes::KEY_ESCAPE) {
                LOG_INFO("Escape pressed (event), request close.");
            }
        }
    }
};

class MyApp : public Engine::EngineApp {
    AppConfig config() override { return AppConfig(1024, 720, "sandbox-plus"); }

    void setup() override
    {
        pushLayer(std::make_unique<GameLayer>());
        LOG_INFO("Sandbox started. Hotkeys: [F1] Grid, [F2] Ray, [F3] Noise, [F4] Pulse, [Space] Spawn 5, [Backspace] Clear");
    }
};

CREATE_ENGINE_APPLICATION(MyApp)
