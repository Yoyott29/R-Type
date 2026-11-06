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
#include "Prefab.hpp"
#include "PrefabsManager.hpp"
#include "Script.hpp"
#include "RatPlayerController.hpp"

int main(int argc, char **argv)
{
    try {
        const std::filesystem::path projectPath =
            ProjectComponentManager::findProjectFile("Rat.gringo");
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
        scene.addResource<PrefabsManager>();
        application.setRenderer("RaylibRenderer", "Rat");
        application.addPlugin("3D");
        application.addPlugin("Physics3D");
        application.addPlugin("InputHandler");
        application.addPlugin("Scripts");
        const auto applyParameters = [](const std::shared_ptr<GringoEntityScript> &script,
                                        const nlohmann::json &component) {
            for (const auto &property : component.value("properties", nlohmann::json::array())) {
                const std::string name = property.value("name", "");
                if (name != "Path")
                    script->setParameter(name, property.value("value", ""));
            }
        };
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
                    if (path == "NONE")
                        continue;
                    if (path == "RatPlayerController.hpp") {
                        auto instance = std::make_shared<RatPlayerController>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
                    components.addToScene(scene, entity, component);
                }
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
                    if (path == "RatPlayerController.hpp") {
                        auto instance = std::make_shared<RatPlayerController>();
                        applyParameters(instance, component);
                        prefab.addComponent(Script{instance, false});
                    }
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
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
