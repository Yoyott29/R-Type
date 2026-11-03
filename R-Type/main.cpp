#include "Application.hpp"
#include "ProjectComponentManager.hpp"
#include "ProjectSettings.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "Prefab.hpp"
#include "PrefabsManager.hpp"
#include "Script.hpp"
#include "DestroyOffScreenScript.hpp"
#include "EnemyScript.hpp"
#include "EnemySpawnerScript.hpp"
#include "SpaceSpawnerScript.hpp"

int main(int argc, char **argv)
{
    try {
        const std::filesystem::path projectPath =
            ProjectComponentManager::findProjectFile("R-Type.gringo");
        std::ifstream file(projectPath);
        if (!file)
            throw std::runtime_error("Could not open project file: " + projectPath.string());
        nlohmann::json root;
        file >> root;
        Application application(std::vector<std::string>(argv + 1, argv + argc));
        Scene &scene = application._scene;
        scene.addResource<ProjectSettings>(ProjectSettings::fromJson(
            root.value("pluginSettings", nlohmann::json::object())));
        scene.addResource<PrefabsManager>();
        application.setRenderer("RaylibRenderer", "R-Type");
        application.addPlugin("2D");
        application.addPlugin("InputHandler");
        application.addPlugin("Scripts");
        application.addPlugin("Physics");
        application.addPlugin("Network");
        application.addPlugin("RTypeClient");
        ProjectComponentManager components(scene, projectPath.parent_path());
        for (const auto &entityData : root.value("entities", nlohmann::json::array())) {
            Entity entity = scene.createEntity();
            components.addEntityId(scene, entityData.value("id", -1), entity);
            for (const auto &component : entityData.value("components", nlohmann::json::array())) {
                if (component.value("name", "") == "Script") {
                    std::string path;
                    for (const auto &property : component.value("properties", nlohmann::json::array()))
                        if (property.value("name", "") == "Path")
                            path = property.value("value", "");
                    if (path == "DestroyOffScreenScript.hpp")
                        scene.addComponent(entity, Script{std::make_shared<DestroyOffScreenScript>(), false});
                    else if (path == "EnemyScript.hpp")
                        scene.addComponent(entity, Script{std::make_shared<EnemyScript>(), false});
                    else if (path == "EnemySpawnerScript.hpp")
                        scene.addComponent(entity, Script{std::make_shared<EnemySpawnerScript>(), false});
                    else if (path == "SpaceSpawnerScript.hpp")
                        scene.addComponent(entity, Script{std::make_shared<SpaceSpawnerScript>(), false});
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else
                    components.addToScene(scene, entity, component);
            }
        }
        components.linkChildren(scene);
        for (const auto &prefabData : root.value("prefabs", nlohmann::json::array())) {
            Prefab prefab;
            for (const auto &component : prefabData.value("components", nlohmann::json::array())) {
                if (component.value("name", "") == "Script") {
                    std::string path;
                    for (const auto &property : component.value("properties", nlohmann::json::array()))
                        if (property.value("name", "") == "Path")
                            path = property.value("value", "");
                    if (path == "DestroyOffScreenScript.hpp")
                        prefab.addComponent(Script{std::make_shared<DestroyOffScreenScript>(), false});
                    else if (path == "EnemyScript.hpp")
                        prefab.addComponent(Script{std::make_shared<EnemyScript>(), false});
                    else if (path == "EnemySpawnerScript.hpp")
                        prefab.addComponent(Script{std::make_shared<EnemySpawnerScript>(), false});
                    else if (path == "SpaceSpawnerScript.hpp")
                        prefab.addComponent(Script{std::make_shared<SpaceSpawnerScript>(), false});
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else
                    components.addToPrefab(prefab, scene, component);
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
