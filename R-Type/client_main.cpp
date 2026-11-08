#include "Application.hpp"
#include "BuildVariantName.hpp"
#include "ProjectComponentManager.hpp"
#include "ProjectSettings.hpp"
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
#include "Prefab.hpp"
#include "PrefabsManager.hpp"
#include "Script.hpp"
#include "UIButton.hpp"
#include "BulletChargeScript.hpp"
#include "BulletScript.hpp"
#include "ColorFilterButtonScript.hpp"
#include "DestroyOffScreenScript.hpp"
#include "EnemyBulletScript.hpp"
#include "EnemyScript.hpp"
#include "EnemyShootScript.hpp"
#include "EnemySpawnerScript.hpp"
#include "ExplosionScript.hpp"
#include "GameOverScript.hpp"
#include "IncreaseScore.hpp"
#include "LobbyMenuScript.hpp"
#include "LobbyServerScript.hpp"
#include "LobbySlotScript.hpp"
#include "PlayerScript.hpp"
#include "PowArmorSpawnerScript.hpp"
#include "PowerUpScript.hpp"
#include "ScoreboardScript.hpp"
#include "SpaceSpawnerScript.hpp"
#include "StartButtonScript.hpp"

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
        scene.addResource<BuildVariantName>(BuildVariantName{"client"});
        scene.addResource<PrefabsManager>();
        application.setRenderer("RaylibRenderer", "R-Type");
        application.addPlugin("Physics2D");
        application.addPlugin("2D");
        application.addPlugin("Scripts");
        application.addPlugin("InputHandler");
        application.addPlugin("Network");
        application.addPlugin("Audio");
        application.addPlugin("UI");
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
                    if (path == "BulletChargeScript.hpp") {
                        auto instance = std::make_shared<BulletChargeScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "BulletScript.hpp") {
                        auto instance = std::make_shared<BulletScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "DestroyOffScreenScript.hpp") {
                        auto instance = std::make_shared<DestroyOffScreenScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "EnemyBulletScript.hpp") {
                        auto instance = std::make_shared<EnemyBulletScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "EnemyScript.hpp") {
                        auto instance = std::make_shared<EnemyScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "EnemyShootScript.hpp") {
                        auto instance = std::make_shared<EnemyShootScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "EnemySpawnerScript.hpp") {
                        auto instance = std::make_shared<EnemySpawnerScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "ExplosionScript.hpp") {
                        auto instance = std::make_shared<ExplosionScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "GameOverScript.hpp") {
                        auto instance = std::make_shared<GameOverScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "IncreaseScore.hpp") {
                        auto instance = std::make_shared<IncreaseScore>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "LobbyMenuScript.hpp") {
                        auto instance = std::make_shared<LobbyMenuScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "LobbyServerScript.hpp") {
                        auto instance = std::make_shared<LobbyServerScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "LobbySlotScript.hpp") {
                        auto instance = std::make_shared<LobbySlotScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "PlayerScript.hpp") {
                        auto instance = std::make_shared<PlayerScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "PowArmorSpawnerScript.hpp") {
                        auto instance = std::make_shared<PowArmorSpawnerScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "PowerUpScript.hpp") {
                        auto instance = std::make_shared<PowerUpScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "ScoreboardScript.hpp") {
                        auto instance = std::make_shared<ScoreboardScript>();
                        applyParameters(instance, component);
                        scene.addComponent(entity, Script{instance, false});
                    }
                    else if (path == "SpaceSpawnerScript.hpp") {
                        auto instance = std::make_shared<SpaceSpawnerScript>();
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
                        if (path == "ColorFilterButtonScript.hpp")
                            scene.getComponent<UIButton>(entity).script = makeButtonScript<ColorFilterButtonScript>(component);
                        if (path == "StartButtonScript.hpp")
                            scene.getComponent<UIButton>(entity).script = makeButtonScript<StartButtonScript>(component);
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
                    if (path == "BulletChargeScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<BulletChargeScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "BulletScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<BulletScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "DestroyOffScreenScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<DestroyOffScreenScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "EnemyBulletScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<EnemyBulletScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "EnemyScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<EnemyScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "EnemyShootScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<EnemyShootScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "EnemySpawnerScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<EnemySpawnerScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "ExplosionScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<ExplosionScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "GameOverScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<GameOverScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "IncreaseScore.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<IncreaseScore>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "LobbyMenuScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<LobbyMenuScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "LobbyServerScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<LobbyServerScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "LobbySlotScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<LobbySlotScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "PlayerScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<PlayerScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "PowArmorSpawnerScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<PowArmorSpawnerScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "PowerUpScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<PowerUpScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "ScoreboardScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<ScoreboardScript>();
                            applyParameters(instance, component);
                            target.addComponent(copy, Script{instance, false});
                        }));
                    }
                    else if (path == "SpaceSpawnerScript.hpp") {
                        // Each copy of the prefab gets its own script
                        prefab.addComponent(std::function<void(Scene &, Entity)>([component, applyParameters](Scene &target, Entity copy) {
                            auto instance = std::make_shared<SpaceSpawnerScript>();
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
                        if (path == "ColorFilterButtonScript.hpp")
                            prefab.addComponent(std::function<void(Scene &, Entity)>([component](Scene &target, Entity instance) { target.getComponent<UIButton>(instance).script = makeButtonScript<ColorFilterButtonScript>(component); }));
                        if (path == "StartButtonScript.hpp")
                            prefab.addComponent(std::function<void(Scene &, Entity)>([component](Scene &target, Entity instance) { target.getComponent<UIButton>(instance).script = makeButtonScript<StartButtonScript>(component); }));
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
