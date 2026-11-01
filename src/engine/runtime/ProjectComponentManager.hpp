#pragma once

#include "Scene.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <utility>

class ProjectComponentManager
{
  public:
    explicit ProjectComponentManager(std::filesystem::path projectFolder)
        : _projectFolder(std::move(projectFolder))
    {
    }

    void addToScene(Scene &scene, Entity entity, const nlohmann::json &component) const;

  private:
    static std::string propertyValue(const nlohmann::json &component, const std::string &name,
                                     const std::string &fallback);
    static float floatProperty(const nlohmann::json &component, const std::string &name,
                               float fallback);
    static int intProperty(const nlohmann::json &component, const std::string &name, int fallback);
    static bool boolProperty(const nlohmann::json &component, const std::string &name,
                             bool fallback);
    static ColorRGBA colorProperty(const nlohmann::json &component, const std::string &name,
                                   ColorRGBA fallback);
    std::string assetPath(const std::string &path) const;

    std::filesystem::path _projectFolder;
};
