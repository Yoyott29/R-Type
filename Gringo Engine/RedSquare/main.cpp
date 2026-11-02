#include "Application.hpp"
#include "ProjectComponentManager.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

int main(int argc, char **argv)
{
    const std::string projectPath = argc == 1 ? "/home/yoyott/Desktop/Epitech/Tek3/Semestre1/ING/R-Type-Mirror/Gringo Engine/RedSquare/RedSquare.gringo" : argv[1];
    if (argc > 2)
        return 84;
    try {
        Application application;
        application.addPlugin("RayLibRenderer");
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
                    components.addToScene(scene, entity, component);
            }
        }
        for (const auto &prefabData : root.value("prefabs", nlohmann::json::array())) {
            Prefab prefab;
            for (const auto &component : prefabData.value("components", nlohmann::json::array())) {
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
