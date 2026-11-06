#include "Application.hpp"
#include "BuildVariantName.hpp"
#include "ProjectComponentManager.hpp"
#include "ProjectSettings.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    try {
        const std::filesystem::path projectPath =
            ProjectComponentManager::findProjectFile("TrackNTrap.gringo");
        std::ifstream file(projectPath);
        if (!file)
            throw std::runtime_error("Could not open project file: " + projectPath.string());
        nlohmann::json root;
        file >> root;
        Application application(std::vector<std::string>(argv + 1, argv + argc));
        Scene &scene = application._scene;
        scene.addResource<ProjectSettings>(ProjectSettings::fromJson(
            root.value("pluginSettings", nlohmann::json::object())));
        scene.addResource<BuildVariantName>(BuildVariantName{""});
        application.setRenderer("RaylibRenderer", "TrackNTrap");
        application.addPlugin("2D");
        application.addPlugin("UI");
        application.addPlugin("3D");
        application.addPlugin("Physics3D");
        ProjectComponentManager components(scene, projectPath.parent_path());
        for (const auto &entityData : root.value("entities", nlohmann::json::array())) {
            Entity entity = scene.createEntity();
            components.addEntityId(scene, entityData.value("id", -1), entity);
            for (const auto &component : entityData.value("components", nlohmann::json::array())) {
                {
                    components.addToScene(scene, entity, component);
                }
            }
        }
        components.linkChildren(scene);
        for (const auto &prefabData : root.value("prefabs", nlohmann::json::array())) {
            Prefab prefab;
            for (const auto &component : prefabData.value("components", nlohmann::json::array())) {
                {
                    components.addToPrefab(prefab, scene, component);
                }
            }
            application._scene.getResource<PrefabsManager>().createPrefab(prefabData.value("name", ""), prefab);
        }
        application.run();
    } catch (const std::exception &error) {
        std::cerr << "Runtime error: " << error.what() << '\n';
        return 84;
    }
    return 0;
}
