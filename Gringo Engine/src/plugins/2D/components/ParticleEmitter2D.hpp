/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter2D
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include "Shape.hpp"
#include <variant>
#include <vector>

struct Particle2D {
    float x;
    float y;
    float dx;
    float dy;
    float life;
};

// Spawns `density` particles per second around the entity's Transform, once started.
// From a script: scene.getComponent<ParticleEmitter2D>(entity).start() / .stop()
struct ParticleEmitter2D {
    float density = 10.0f;
    std::variant<BoxShape, CircleShape> shape = CircleShape{2.0f};
    float lifetime = 1.0f;
    float speed = 100.0f;
    ColorRGBA color{255, 255, 255, 255};

    std::vector<Particle2D> particles;
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

template <> struct ComponentDescription<ParticleEmitter2D> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"ParticleEmitter2D",
                {
                    {"Density", valueType::Float, "10"},
                    {"Shape", valueType::Choice, "Circle", {"Box", "Circle"}},
                    {"Width", valueType::Float, "4", {}, {"Shape", "Box"}},
                    {"Height", valueType::Float, "4", {}, {"Shape", "Box"}},
                    {"Radius", valueType::Float, "2", {}, {"Shape", "Circle"}},
                    {"Lifetime", valueType::Float, "1"},
                    {"Speed", valueType::Float, "100"},
                    {"Color", valueType::Color, "White"},
                },
                {70, 56, 51, 255},
                ""};
    }
};
