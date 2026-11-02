#include "Application.hpp"
#include "ProjectComponentManager.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "Prefab.hpp"
#include "PrefabsManager.hpp"
#include "NetworkConfig.hpp"
#include <asio.hpp>
#include <charconv>
#include <cstring>
#include <optional>
#include "ScriptSystem.hpp"
#include "DestroyOffScreenScript.hpp"
#include "EnemyScript.hpp"
#include "EnemySpawnerScript.hpp"
#include "SpaceSpawnerScript.hpp"

int main(int argc, char **argv)
{
    std::string ip = "127.0.0.1";
    uint16_t port = 4242;
    if (argc > 3)
        return 84;
    if (argc >= 2) {
        const char *argument = argc == 3 ? argv[2] : argv[1];
        unsigned int parsedPort = 0;
        const char *end = argument + std::strlen(argument);
        auto [pointer, error] = std::from_chars(argument, end, parsedPort);
        if (error != std::errc() || pointer != end || parsedPort == 0 || parsedPort > 65535)
            return 84;
        port = static_cast<uint16_t>(parsedPort);
    }
    if (argc == 3) {
        asio::error_code error;
        asio::ip::make_address_v4(argv[1], error);
        if (error)
            return 84;
        ip = argv[1];
    }
    const std::string projectPath = "/home/yoyott/Desktop/Epitech/Tek3/Semestre1/ING/R-Type-Mirror/R-Type/R-Type.gringo";
    try {
        Application application;
        application._scene.addResource<PrefabsManager>();
        application._scene.addResource<NetworkConfig>(NetworkConfig::client(ip, port));
        application.addPlugin("RayLibRenderer");
        application.addPlugin("Physics");
        application.addPlugin("RTypeClient");
        application.addPlugin("Network");
        application._scene.registerSystem<ScriptSystem>(FixedUpdate);
        ComponentSet requirements;
        requirements.set(application._scene.getComponentType<Script>());
        application._scene.setSystemComponentSet<ScriptSystem>(requirements);
        std::ifstream file(projectPath);
        if (!file)
            throw std::runtime_error("Could not open project file: " + projectPath);
        nlohmann::json root;
        file >> root;
        Scene &scene = application._scene;
        ProjectComponentManager components(scene, std::filesystem::path(projectPath).parent_path());
        for (const auto &entityData : root.value("entities", nlohmann::json::array())) {
            Entity entity = scene.createEntity();
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
