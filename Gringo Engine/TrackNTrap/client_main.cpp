#include "Application.hpp"
#include "BroomSpin.hpp"
#include "BuildVariantName.hpp"
#include "CameraOrbit.hpp"
#include "CheeseSpawner.hpp"
#include "CountdownScript.hpp"
#include "Hud.hpp"
#include "KnifeSpin.hpp"
#include "LobbyMenu.hpp"
#include "ObstacleSpawner.hpp"
#include "Prefab.hpp"
#include "PrefabsManager.hpp"
#include "ProjectComponentManager.hpp"
#include "ProjectSettings.hpp"
#include "RaceDirector.hpp"
#include "RatCamera.hpp"
#include "RatPlayerController.hpp"
#include "Script.hpp"
#include "SpatulaSlap.hpp"
#include "StartButton.hpp"
#include "TrapPlacer.hpp"
#include "UIButton.hpp"
#include "nlohmann/json.hpp"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
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
        scene.addResource<ProjectSettings>(
            ProjectSettings::fromJson(root.value("pluginSettings", nlohmann::json::object())));
        scene.addResource<BuildVariantName>(BuildVariantName{"client"});
        scene.addResource<PrefabsManager>();
        application.setRenderer("RaylibRenderer", "TrackNTrap");
        application.addPlugin("2D");
        application.addPlugin("UI");
        application.addPlugin("3D");
        application.addPlugin("Physics3D");
        application.addPlugin("Scripts");
        application.addPlugin("InputHandler");
        application.addPlugin("Audio");
        application.addPlugin("Network");
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
                    for (const auto &property :
                         component.value("properties", nlohmann::json::array()))
                        if (property.value("name", "") == "Path")
                            path = property.value("value", "");
                    if (path == "NONE")
                        continue;
                    if (path == "BroomSpin.hpp") {
                        auto instance = std::make_shared<BroomSpin>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "CameraOrbit.hpp") {
                        auto instance = std::make_shared<CameraOrbit>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "CheeseSpawner.hpp") {
                        auto instance = std::make_shared<CheeseSpawner>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "CountdownScript.hpp") {
                        auto instance = std::make_shared<CountdownScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "Hud.hpp") {
                        auto instance = std::make_shared<Hud>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "KnifeSpin.hpp") {
                        auto instance = std::make_shared<KnifeSpin>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "LobbyMenu.hpp") {
                        auto instance = std::make_shared<LobbyMenu>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "ObstacleSpawner.hpp") {
                        auto instance = std::make_shared<ObstacleSpawner>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "RaceDirector.hpp") {
                        auto instance = std::make_shared<RaceDirector>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "RatCamera.hpp") {
                        auto instance = std::make_shared<RatCamera>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "RatPlayerController.hpp") {
                        auto instance = std::make_shared<RatPlayerController>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "SpatulaSlap.hpp") {
                        auto instance = std::make_shared<SpatulaSlap>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else if (path == "TrapPlacer.hpp") {
                        auto instance = std::make_shared<TrapPlacer>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    } else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
                    components.addToScene(scene, entity, component);
                    if (component.value("name", "") == "Button") {
                        std::string path;
                        for (const auto &property :
                             component.value("properties", nlohmann::json::array()))
                            if (property.value("name", "") == "Script")
                                path = property.value("value", "");
                        if (path == "StartButton.hpp")
                            scene.getComponent<UIButton>(entity).script =
                                makeButtonScript<StartButton>(component);
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
                    for (const auto &property :
                         component.value("properties", nlohmann::json::array()))
                        if (property.value("name", "") == "Path")
                            path = property.value("value", "");
                    if (path == "BroomSpin.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<BroomSpin>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "CameraOrbit.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<CameraOrbit>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "CheeseSpawner.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<CheeseSpawner>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "CountdownScript.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<CountdownScript>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "Hud.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<Hud>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "KnifeSpin.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<KnifeSpin>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "LobbyMenu.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<LobbyMenu>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "ObstacleSpawner.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<ObstacleSpawner>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "RaceDirector.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<RaceDirector>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "RatCamera.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<RatCamera>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "RatPlayerController.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<RatPlayerController>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "SpatulaSlap.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<SpatulaSlap>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else if (path == "TrapPlacer.hpp") {

                        prefab.addComponent(std::function<void(Scene &, Entity)>(
                            [component, applyParameters](Scene &target, Entity copy) {
                                auto instance = std::make_shared<TrapPlacer>();
                                applyParameters(instance, component);
                                target.addComponent(copy, Script{instance, false});
                            }));
                    } else
                        throw std::runtime_error("Unknown script: " + path);
                } else {
                    components.addToPrefab(prefab, scene, component);
                    if (component.value("name", "") == "Button") {
                        std::string path;
                        for (const auto &property :
                             component.value("properties", nlohmann::json::array()))
                            if (property.value("name", "") == "Script")
                                path = property.value("value", "");
                        if (path == "StartButton.hpp")
                            prefab.addComponent(std::function<void(Scene &, Entity)>(
                                [component](Scene &target, Entity instance) {
                                    target.getComponent<UIButton>(instance).script =
                                        makeButtonScript<StartButton>(component);
                                }));
                    }
                }
            }
            application._scene.getResource<PrefabsManager>().createPrefab(
                prefabData.value("name", ""), prefab);
        }
        application.run();
    } catch (const std::exception &error) {
        std::cerr << "Runtime error: " << error.what() << '\n';
        return 84;
    }
    return 0;
}
