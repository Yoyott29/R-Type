/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Model3D
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

struct Model3D {
    std::string path;
    std::string primitive = "Model";
    float scale = 1.0f;
};

template <> struct ComponentDescription<Model3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"3DModel", {        {"Primitive", valueType::Choice, "Cube", {"Cube", "Sphere", "Model"}},
        {"ModelPath", valueType::File, NO_FILE, {"assets", ".obj", ".glb", ".gltf"},
         {"Primitive", "Model"}},
                            {"Scale", valueType::Float, "1"}},
                {75, 95, 65, 255}, "3D"};
    }
};
