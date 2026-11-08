/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Model3D
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include <string>

struct Model3D {
    std::string path;
    std::string primitive = "Model";
    std::string texturePath;
    ColorRGBA color{255, 255, 255, 255};
    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;
    float scale = 1.0f;
    int animation = -1;
    float animationSpeed = 1.0f;
    // Holds the animation on one point, 0 (start) to 1 (end), instead of playing it. -1 plays it
    float animationFrame = -1.0f;
};

template <> struct ComponentDescription<Model3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"3DModel",
                {{"Primitive", valueType::Choice, "Cube", {"Cube", "Sphere", "Model"}},
                 {"ModelPath",
                  valueType::File,
                  NO_FILE,
                  {"assets", ".obj", ".glb", ".gltf"},
                  {"Primitive", "Model"}},
                 {"TexturePath", valueType::File, NO_FILE, {"assets", ".png", ".jpg", ".jpeg"}},
                 {"Color", valueType::Color, "White"},
                 {"Width", valueType::Float, "1"},
                 {"Height", valueType::Float, "1"},
                 {"Depth", valueType::Float, "1"},
                 {"Scale", valueType::Float, "1"},
                 {"Animation", valueType::Int, "-1"},
                 {"AnimationSpeed", valueType::Float, "1"},
                 {"AnimationFrame", valueType::Float, "-1"}},
                {75, 95, 65, 255},
                "3D"};
    }
};
