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
#include "UIButton.hpp"
#include "CameraOrbit.hpp"
#include "CountdownScript.hpp"
#include "KnifeSpin.hpp"
#include "PlayButton.hpp"
#include "RatPlayerController.hpp"

template <typename ButtonScript>
static std::shared_ptr<IGringoButtonScript> makeButtonScript(const nlohmann::json &component)
{
    auto script = std::make_shared<ButtonScript>();
    for (const auto &property : component.value("properties", nlohmann::json::array())) {
        const std::string name = property.value("name", "");
        if (name != "Script")
            script->setParameter(name, property.value("value", ""));
    }
    return script;
}

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
        scene.addResource<PrefabsManager>();
        application.setRenderer("RaylibRenderer", "TrackNTrap");
        application.addPlugin("2D");
        application.addPlugin("UI");
        application.addPlugin("3D");
        application.addPlugin("Physics3D");
        application.addPlugin("Scripts");
        application.addPlugin("InputHandler");
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
                    if (path == "CameraOrbit.hpp") {
                        auto instance = std::make_shared<CameraOrbit>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "CountdownScript.hpp") {
                        auto instance = std::make_shared<CountdownScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "KnifeSpin.hpp") {
                        auto instance = std::make_shared<KnifeSpin>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "RatPlayerController.hpp") {
                        auto instance = std::make_shared<RatPlayerController>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
                    components.addToScene(scene, entity, component);
                    if (component.value("name", "") == "Button") {
                        std::string path;
                        for (const auto &property : component.value("properties", nlohmann::json::array()))
                            if (property.value("name", "") == "Script")
                                path = property.value("value", "");
                        if (path == "PlayButton.hpp")
                            scene.getComponent<UIButton>(entity).script = makeButtonScript<PlayButton>(component);
                    }
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
                    if (path == "CameraOrbit.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<CameraOrbit>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "CountdownScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<CountdownScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "KnifeSpin.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<KnifeSpin>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "RatPlayerController.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<RatPlayerController>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
                    components.addToPrefab(prefab, scene, component);
                    if (component.value("name", "") == "Button") {
                        std::string path;
                        for (const auto &property : component.value("properties", nlohmann::json::array()))
                            if (property.value("name", "") == "Script")
                                path = property.value("value", "");
                        if (path == "PlayButton.hpp")
                            prefab.addComponent(std::function<void(Scene &, Entity)>([component](Scene &target, Entity instance) { target.getComponent<UIButton>(instance).script = makeButtonScript<PlayButton>(component); }));
                    }
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
