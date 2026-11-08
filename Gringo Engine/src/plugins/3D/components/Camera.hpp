/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Camera
*/

#pragma once
#include "ComponentDescriptor.hpp"

struct Camera {
    float targetX = 0.0f;
    float targetY = 0.0f;
    float targetZ = 0.0f;
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;
    float fieldOfView = 45.0f;
    bool active = true;
};

template <> struct ComponentDescription<Camera> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"Camera",
                {{"TargetX", valueType::Float, "0"},
                 {"TargetY", valueType::Float, "0"},
                 {"TargetZ", valueType::Float, "0"},
                 {"UpX", valueType::Float, "0"},
                 {"UpY", valueType::Float, "1"},
                 {"UpZ", valueType::Float, "0"},
                 {"FieldOfView", valueType::Float, "45"},
                 {"Active", valueType::Bool, "True"}},
                {59, 75, 96, 255},
                "3D"};
    }
};
