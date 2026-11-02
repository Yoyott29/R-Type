#pragma once

#include "Scene.hpp"
#include "Prefab.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <utility>

class ProjectComponentManager
{
  public:
    ProjectComponentManager(Scene &scene, std::filesystem::path projectFolder);

    void addToScene(Scene &scene, Entity entity, const nlohmann::json &component) const;
    void addToPrefab(Prefab &prefab, Scene &scene, const nlohmann::json &component) const;

  private:
    std::string assetPath(const std::string &path) const;

    std::filesystem::path _projectFolder;
};
