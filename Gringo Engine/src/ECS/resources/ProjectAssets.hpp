/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ProjectAssets
*/

#pragma once
#include "Scene.hpp"
#include <filesystem>
#include <string>

// Scene resource holding the folder of the loaded .gringo project, so component deserializers can
// turn "assets/Player.png" into a path that works from anywhere
struct ProjectAssets {
    std::filesystem::path projectFolder;

    std::string resolve(const std::string &path) const
    {
        if (path.empty() || std::filesystem::path(path).is_absolute())
            return path;
        std::filesystem::path relativePath = path;
        if (relativePath.begin() != relativePath.end() &&
            (*relativePath.begin() == "assets" || *relativePath.begin() == "Assets"))
            relativePath = relativePath.lexically_relative(*relativePath.begin());
        return (projectFolder / "assets" / relativePath).string();
    }

    // Resolves through the scene's ProjectAssets, or returns the path unchanged without one
    static std::string resolve(Scene &scene, const std::string &path)
    {
        return scene.hasResource<ProjectAssets>() ? scene.getResource<ProjectAssets>().resolve(path)
                                                  : path;
    }
};
