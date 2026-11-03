#include "ProjectComponentManager.hpp"
#include "ChildOf.hpp"
#include "ComponentJson.hpp"
#include "HierarchySystem.hpp"
#include "Position.hpp"
#include "PluginLoader.hpp"
#include "ProjectAssets.hpp"
#include <stdexcept>

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
    scene.addResource<EditorEntityIds>();
    // A parent loaded after its child is found later, by linkChildren. A prefab instantiated
    // during the game finds its parent (a scene entity) right away
    scene.registerComponent<ChildOf>([](Scene &target, Entity entity, const nlohmann::json &data) {
        ChildOf link;
        link.parentId = ComponentJson::intValue(data, "Parent", -1);
        const auto &ids = target.getResource<EditorEntityIds>().entities;
        auto parent = ids.find(link.parentId);
        if (parent != ids.end())
            link.parent = parent->second;
        target.addComponent(entity, link);
    });
}

void ProjectComponentManager::addEntityId(Scene &scene, int editorId, Entity entity) const
{
    scene.getResource<EditorEntityIds>().entities[editorId] = entity;
}

void ProjectComponentManager::linkChildren(Scene &scene) const
{
    const auto &ids = scene.getResource<EditorEntityIds>().entities;
    for (Entity child : scene.view<ChildOf>()) {
        ChildOf &link = scene.getComponent<ChildOf>(child);
        auto parent = ids.find(link.parentId);
        if (link.parent == NO_PARENT && parent != ids.end())
            link.parent = parent->second;
    }
    HierarchySystem::followParents(scene);
}

std::filesystem::path ProjectComponentManager::findProjectFile(const std::string &fileName)
{
    const std::filesystem::path folder = PluginLoader::executableDirectory();
    const std::filesystem::path besideExecutable = folder / fileName;
    const std::filesystem::path inProjectFolder =
        folder / std::filesystem::path(fileName).stem() / fileName;

    if (std::filesystem::is_regular_file(besideExecutable))
        return besideExecutable;
    if (std::filesystem::is_regular_file(inProjectFolder))
        return inProjectFolder;
    throw std::runtime_error("Could not find " + fileName + " next to the executable in " +
                             folder.string());
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
