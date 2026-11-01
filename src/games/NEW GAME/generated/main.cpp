#include "Application.hpp"
#include "ProjectComponentManager.hpp"
#include "ScriptSystem.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "PlayerScript.hpp"

namespace {
std::shared_ptr<GringoEntityScript> createScript(const std::string &path)
{
    if (path == "PlayerScript.hpp")
        return std::make_shared<PlayerScript>();
    throw std::runtime_error("Unknown script: " + path);
}

std::string scriptPath(const nlohmann::json &component)
{
    for (const auto &property : component.value("properties", nlohmann::json::array()))
        if (property.value("name", "") == "Path")
            return property.value("value", "");
    return {};
}

void loadProject(Application &application, const std::string &path)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not open project file: " + path);
    nlohmann::json root;
    file >> root;
    Scene &scene = application._scene;
    ProjectComponentManager components(std::filesystem::path(path).parent_path());
    for (const auto &entityData : root.value("entities", nlohmann::json::array())) {
        Entity entity = scene.createEntity();
        for (const auto &component : entityData.value("components", nlohmann::json::array())) {
            if (component.value("name", "") == "Script")
                scene.addComponent(entity, Script{createScript(scriptPath(component)), false});
            else
                components.addToScene(scene, entity, component);
        }
    }
}
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 84;
    try {
        Application application;
        application.addPlugin("RayLibRenderer");
        application.addPlugin("Physics");
        application._scene.registerSystem<ScriptSystem>(FixedUpdate);
        ComponentSet requirements;
        requirements.set(application._scene.getComponentType<Script>());
        application._scene.setSystemComponentSet<ScriptSystem>(requirements);
        loadProject(application, argv[1]);
        application.run();
    } catch (const std::exception &error) {
        std::cerr << "Runtime error: " << error.what() << '\\n';
        return 84;
    }
    return 0;
}
