#pragma once

#include "Scene.hpp"
#include "Prefab.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <utility>

class ProjectComponentManager
{
  public:
    // Adds the ProjectAssets resource and the Position deserializer. Create it after the plugins
    // are added: they register the deserializers of their own components
    ProjectComponentManager(Scene &scene, std::filesystem::path projectFolder);

    void addToScene(Scene &scene, Entity entity, const nlohmann::json &component) const;
    void addToPrefab(Prefab &prefab, Scene &scene, const nlohmann::json &component) const;
};
