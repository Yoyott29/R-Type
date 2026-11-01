/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Parser
*/

#include "Parser.hpp"

nlohmann::json Parser::serializeComponents(const std::vector<Component> &components)
{
    nlohmann::json componentsJson = nlohmann::json::array();

    for (const auto &component : components) {
        nlohmann::json propertiesJson = nlohmann::json::array();
        for (const auto &property : component.properties) {
            propertiesJson.push_back({
                {"id", property.id},
                {"name", property.name},
                {"value", property.value},
                {"type", static_cast<int>(property.type)}
            });
        }
        componentsJson.push_back({
            {"name", component.name},
            {"properties", propertiesJson}
        });
    }
    return componentsJson;
}

std::vector<Component> Parser::deserializeComponents(const nlohmann::json &json)
{
    std::vector<Component> components;

    for (const auto &componentJson : json.value("components", nlohmann::json::array())) {
        Component component;
        component.name = componentJson.value("name", "");

        for (const auto &propertyJson : componentJson.value("properties", nlohmann::json::array())) {
            Property property;
            property.id = propertyJson.value("id", 0);
            property.name = propertyJson.value("name", "");
            property.value = propertyJson.value("value", decltype(property.value){});
            property.type = static_cast<decltype(property.type)>(propertyJson.value("type", 0));
            component.properties.push_back(std::move(property));
        }
        components.push_back(std::move(component));
    }
    return components;
}

void Parser::saveFile(const std::string &filePath, const SceneObjects &sceneObjects)
{
    nlohmann::json root;
    root["projectName"] = sceneObjects.projectName;
    root["entities"] = nlohmann::json::array();
    root["prefabs"] = nlohmann::json::array();

    for (const auto &entity : sceneObjects._entities) {
        root["entities"].push_back({
            {"id", entity.id},
            {"name", entity.name},
            {"components", serializeComponents(entity.components)}
        });
    }
    for (const auto &prefab : sceneObjects._prefabs) {
        root["prefabs"].push_back({
            {"id", prefab.id},
            {"name", prefab.name},
            {"components", serializeComponents(prefab.components)}
        });
    }

    std::filesystem::path fileName = filePath;
    if (fileName.extension() == ".gringo")
        fileName.replace_extension();

    const std::filesystem::path gameFolder = std::filesystem::path(GAME_FOLDER) / fileName;
    const std::filesystem::path gamePath = gameFolder / (fileName.string() + ".gringo");
    std::filesystem::create_directories(gameFolder);
    std::ofstream file(gamePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << gamePath << std::endl;
        return;
    }
    file << root.dump(4);
    file.close();
}

SceneObjects Parser::loadFile(const std::string &filePath)
{
    SceneObjects result;
    std::filesystem::path fileName = filePath;
    if (fileName.extension() == ".gringo")
        fileName.replace_extension();

    const std::filesystem::path gamePath =
        std::filesystem::path(GAME_FOLDER) / fileName / (fileName.string() + ".gringo");
    std::ifstream file(gamePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << gamePath << std::endl;
        return result;
    }

    nlohmann::json root;

    try {
        file >> root;
    } catch (const nlohmann::json::parse_error &e) {
        std::cerr << "Failed to parse " << gamePath << ": " << e.what() << std::endl;
        return result;
    }

    result.projectName = root.value("projectName", filePath);
    
    for (const auto &entityJson : root.value("entities", nlohmann::json::array())) {
        Entity entity;
        entity.id = entityJson.value("id", 0);
        entity.name = entityJson.value("name", "");
        entity.components = deserializeComponents(entityJson);
        result._entities.push_back(std::move(entity));
    }

    for (const auto &prefabJson : root.value("prefabs", nlohmann::json::array())) {
        Prefab prefab;
        prefab.id = prefabJson.value("id", 0);
        prefab.name = prefabJson.value("name", "");
        prefab.components = deserializeComponents(prefabJson);
        result._prefabs.push_back(std::move(prefab));
    }

    return result;
}

std::vector<std::string> Parser::getSavedFiles()
{
    std::vector<std::string> savedFiles;

    if (!std::filesystem::exists(GAME_FOLDER)) {
        std::cerr << "Failed to open directory: " << GAME_FOLDER << std::endl;
        return savedFiles;
    }

    for (const auto &entry : std::filesystem::directory_iterator(GAME_FOLDER)) {
        if (!entry.is_directory())
            continue;

        const std::string name = entry.path().filename().string();
        const std::filesystem::path gameFile = entry.path() / (name + ".gringo");
        if (std::filesystem::is_regular_file(gameFile))
            savedFiles.push_back(name);
    }
    return savedFiles;
}
