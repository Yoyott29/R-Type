#include "ProjectComponentManager.hpp"
#include "ComponentJson.hpp"
#include "Position.hpp"
#include "ProjectAssets.hpp"

// Plugin components are not handled here: each plugin registers its own deserializer in
// registerComponents(), which runs when the plugin is added to the application
ProjectComponentManager::ProjectComponentManager(Scene &scene,
                                                 std::filesystem::path projectFolder)
{
    scene.addResource<ProjectAssets>(ProjectAssets{std::move(projectFolder)});
    scene.registerComponent<Position>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity, Position{ComponentJson::floatValue(data, "PositionX", 0.0f),
                                             ComponentJson::floatValue(data, "PositionY", 0.0f)});
    });
}

void ProjectComponentManager::addToScene(Scene &scene, Entity entity,
                                         const nlohmann::json &component) const
{
    if (component.value("name", "") == "Script")
        return;
    scene.addComponentFromJson(entity, component);
}

void ProjectComponentManager::addToPrefab(Prefab &prefab, Scene &scene,
                                          const nlohmann::json &component) const
{
    if (component.value("name", "") == "Script")
        return;
    prefab.addComponent(scene.componentInitializer(component));
}
