#pragma once
#include "ComponentDescriptor.hpp"

struct CameraFollow3D {
    float distance = 8.0f;
    float height = 3.0f;
    float targetHeight = 1.0f;
};

template <> struct ComponentDescription<CameraFollow3D> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"CameraFollow3D",
                {{"Distance", valueType::Float, "8"},
                 {"Height", valueType::Float, "3"},
                 {"TargetHeight", valueType::Float, "1"}},
                {59, 75, 96, 255},
                "3D"};
    }
};
