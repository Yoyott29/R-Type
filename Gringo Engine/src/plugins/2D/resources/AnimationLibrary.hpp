/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AnimationLibrary
*/

#pragma once
#include "EngineError.hpp"
#include "ProjectAssets.hpp"
#include "Scene.hpp"
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

// One .anim file (made by the editor's Animation panel):
//   {"texture": "run.png", "frameWidth": 32, "frameHeight": 32, "frames": 8, "fps": 10,
//    "loop": true}
// Frames are frameWidth x frameHeight cells of the texture, read left to right then top to
// bottom, "columns" per row (default: frames, a horizontal strip), from cell "start" (default 0)
struct AnimationClip {
    std::string texturePath;
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;
    int frames = 1;
    int columns = 1;
    int start = 0;
    float fps = 10.0f;
    bool loop = true;
};

// Scene resource: every .anim file read so far, by resolved path, so each is parsed once
class AnimationLibrary
{
  public:
    const AnimationClip &get(Scene &scene, const std::string &path)
    {
        auto found = _clips.find(path);
        if (found != _clips.end())
            return found->second;
        return _clips.emplace(path, load(scene, path)).first->second;
    }

  private:
    AnimationClip load(Scene &scene, const std::string &path) const
    {
        std::ifstream file(path);
        if (!file)
            throw EngineError("Could not open animation file: " + path);
        nlohmann::json json;
        try {
            file >> json;
        } catch (const nlohmann::json::exception &error) {
            throw EngineError("Invalid animation file " + path + ": " + error.what());
        }
        AnimationClip clip;
        clip.texturePath = ProjectAssets::resolve(scene, json.value("texture", ""));
        clip.frameWidth = json.value("frameWidth", 0.0f);
        clip.frameHeight = json.value("frameHeight", 0.0f);
        clip.frames = json.value("frames", 1);
        clip.columns = json.value("columns", clip.frames);
        clip.start = json.value("start", 0);
        clip.fps = json.value("fps", 10.0f);
        clip.loop = json.value("loop", true);
        if (clip.texturePath.empty() || clip.frameWidth <= 0.0f || clip.frameHeight <= 0.0f ||
            clip.frames <= 0 || clip.columns <= 0 || clip.start < 0 || clip.fps < 0.0f)
            throw EngineError("Invalid animation file: " + path);
        return clip;
    }

    std::map<std::string, AnimationClip> _clips;
};
