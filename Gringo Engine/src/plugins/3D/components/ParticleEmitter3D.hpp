/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter3D
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include <variant>
#include <vector>

struct Particle3D {
    float x;
    float y;
    float z;
    float dx;
    float dy;
    float dz;
    float life;
};

struct CubeShape {
    float width;
    float height;
    float depth;
};

struct SphereShape {
    float radius;
};

// Spawns `density` particles per second anywhere within `spawnRadius` of the entity's Transform,
// once started: each one leaves in a random direction at up to `speed`, drawn as the shape,
// fading out over its lifetime.
// From a script: scene.getComponent<ParticleEmitter3D>(entity).start() / .stop()
struct ParticleEmitter3D {
    float density = 10.0f;
    std::variant<CubeShape, SphereShape> shape = CubeShape{0.2f, 0.2f, 0.2f};
    float lifetime = 1.0f;
    float speed = 1.0f;
    float spawnRadius = 0.0f;
    ColorRGBA color{255, 255, 255, 255};

    std::vector<Particle3D> particles;
    bool emitting = false;
    float spawnAccumulator = 0.0f;

    void start() { emitting = true; }
    void stop()
    {
        emitting = false;
        spawnAccumulator = 0.0f;
    }
    bool isEmitting() const { return emitting; }
};

template <> struct ComponentDescription<ParticleEmitter3D> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"ParticleEmitter3D",
                {
                    {"Density", valueType::Float, "10"},
                    {"Shape", valueType::Choice, "Cube", {"Cube", "Sphere"}},
                    {"Width", valueType::Float, "0.2", {}, {"Shape", "Cube"}},
                    {"Height", valueType::Float, "0.2", {}, {"Shape", "Cube"}},
                    {"Depth", valueType::Float, "0.2", {}, {"Shape", "Cube"}},
                    {"Radius", valueType::Float, "0.1", {}, {"Shape", "Sphere"}},
                    {"Lifetime", valueType::Float, "1"},
                    {"Speed", valueType::Float, "1"},
                    {"SpawnRadius", valueType::Float, "0"},
                    {"Color", valueType::Color, "White"},
                },
                {70, 56, 51, 255},
                "3D"};
    }
};
