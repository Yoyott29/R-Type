#pragma once

#include "Prefab.hpp"
#include "Scene.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <optional>
#include <utility>

// Scene resource: the game entity created for each editor entity id (the "id" in .gringo files),
// so ChildOf can find its parent
struct EditorEntityIds {
    std::map<int, Entity> entities;

    // The entity of an editor id, e.g. a script's "Target" parameter. Empty if there is none
    std::optional<Entity> find(int editorId) const
    {
        const auto entity = entities.find(editorId);
        if (entity == entities.end())
            return std::nullopt;
        return entity->second;
    }
};

class ProjectComponentManager
{
  public:
    // Adds the ProjectAssets resource and the Position deserializer. Create it after the plugins
    // are added: they register the deserializers of their own components
    ProjectComponentManager(Scene &scene, std::filesystem::path projectFolder);

    // The game's .gringo file, next to its executable: <exe folder>/<fileName>, or
    // <exe folder>/<project>/<fileName> (R-Type's client is built beside its R-Type folder)
    static std::filesystem::path findProjectFile(const std::string &fileName);

    void addToScene(Scene &scene, Entity entity, const nlohmann::json &component) const;
    // The entity created for the editor entity editorId
    void addEntityId(Scene &scene, int editorId, Entity entity) const;
    // Once every entity is loaded: gives each ChildOf its parent and puts children in place
    void linkChildren(Scene &scene) const;
    void addToPrefab(Prefab &prefab, Scene &scene, const nlohmann::json &component) const;
    // Components with these names are skipped by addToScene and addToPrefab
    void ignore(std::set<std::string> names);

  private:
    bool isSkipped(const nlohmann::json &component) const;

    std::set<std::string> _ignored;
};
