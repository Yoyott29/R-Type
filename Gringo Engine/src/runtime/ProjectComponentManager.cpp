#include "ProjectComponentManager.hpp"
#include "ComponentJson.hpp"
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
