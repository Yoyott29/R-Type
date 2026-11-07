/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Parser
*/

#include "Parser.hpp"
#include <algorithm>
#include <array>
#include <set>
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

nlohmann::json Parser::serializeComponents(const std::vector<Component> &components)
{
    nlohmann::json componentsJson = nlohmann::json::array();

    for (const auto &component : components) {
        nlohmann::json propertiesJson = nlohmann::json::array();
        for (const auto &property : component.properties) {
            propertiesJson.push_back({{"id", property.id},
                                      {"name", property.name},
                                      {"value", property.value},
                                      {"type", static_cast<int>(property.type)}});
        }
        componentsJson.push_back({{"name", component.name}, {"properties", propertiesJson}});
    }
    return componentsJson;
}

std::vector<Component> Parser::deserializeComponents(const nlohmann::json &json)
{
    std::vector<Component> components;

    for (const auto &componentJson : json.value("components", nlohmann::json::array())) {
        Component component;
        component.name = componentJson.value("name", "");

        for (const auto &propertyJson :
             componentJson.value("properties", nlohmann::json::array())) {
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

void Parser::saveFile(const std::string &, const SceneObjects &sceneObjects)
{
    nlohmann::json root;
    root["projectName"] = sceneObjects.projectName;
    root["pluginSettings"] = sceneObjects.pluginSettings;
    root["plugins"] = sceneObjects.activePlugins;
    root["renderer"] = sceneObjects.renderer;
    root["entities"] = nlohmann::json::array();
    root["prefabs"] = nlohmann::json::array();

    for (const auto &entity : sceneObjects._entities) {
        root["entities"].push_back({{"id", entity.id},
                                    {"name", entity.name},
                                    {"components", serializeComponents(entity.components)}});
    }
    for (const auto &prefab : sceneObjects._prefabs) {
        root["prefabs"].push_back({{"id", prefab.id},
                                   {"name", prefab.name},
                                   {"components", serializeComponents(prefab.components)}});
    }

    const std::filesystem::path requestedPath =
        sceneObjects.projectFilePath.empty() ? executableDirectory() / sceneObjects.projectName /
                                                   (sceneObjects.projectName + ".gringo")
                                             : sceneObjects.projectFilePath;
    const std::filesystem::path gamePath =
        requestedPath.extension() == ".gringo"
            ? requestedPath
            : requestedPath / (requestedPath.filename().string() + ".gringo");
    const std::filesystem::path gameFolder = gamePath.parent_path();
    std::filesystem::create_directories(gameFolder);
    std::filesystem::create_directories(gameFolder / "scripts");
    std::filesystem::create_directories(gameFolder / "assets");
    copyDefaultFont(gameFolder / "assets");
    std::ofstream file(gamePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << gamePath << std::endl;
        return;
    }
    file << root.dump(4);
    file.close();
    _lastSavedPath = gamePath;
}

SceneObjects Parser::loadFile(const std::string &filePath)
{
    SceneObjects result;
    const std::filesystem::path requestedPath = filePath;
    std::filesystem::path fileName = requestedPath;
    if (fileName.extension() == ".gringo")
        fileName.replace_extension();

    std::filesystem::path gamePath = requestedPath;
    if (!gamePath.is_absolute())
        gamePath = std::filesystem::current_path() / gamePath;

    if (!std::filesystem::is_regular_file(gamePath)) {
        const std::filesystem::path legacyPath =
            std::filesystem::current_path() / fileName / (fileName.filename().string() + ".gringo");
        if (std::filesystem::is_regular_file(legacyPath))
            gamePath = legacyPath;
    }

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
    result.projectFilePath = std::filesystem::absolute(gamePath).lexically_normal();
    result.pluginSettings =
        root.value("pluginSettings", std::map<std::string, std::map<std::string, std::string>>{});
    result.activePlugins = root.value("plugins", std::vector<std::string>{});
    result.renderer = root.value("renderer", "");

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

    const std::filesystem::path currentDirectory = std::filesystem::current_path();
    const std::array<std::filesystem::path, 2> searchRoots = {currentDirectory,
                                                              currentDirectory.parent_path()};
    std::set<std::string> discoveredFiles;

    const auto addProjectFile = [&](const std::filesystem::path &projectFile) {
        const std::string canonicalPath =
            std::filesystem::absolute(projectFile).lexically_normal().generic_string();
        if (!discoveredFiles.insert(canonicalPath).second)
            return;

        savedFiles.push_back(
            std::filesystem::relative(projectFile, currentDirectory).generic_string());
    };

    for (const auto &root : searchRoots) {
        if (!std::filesystem::is_directory(root))
            continue;

        for (const auto &entry : std::filesystem::directory_iterator(root)) {
            if (entry.is_regular_file() && entry.path().extension() == ".gringo") {
                addProjectFile(entry.path());
                continue;
            }
            if (!entry.is_directory())
                continue;

            for (const auto &projectFile : std::filesystem::directory_iterator(entry.path())) {
                if (projectFile.is_regular_file() && projectFile.path().extension() == ".gringo") {
                    addProjectFile(projectFile.path());
                }
            }
        }
    }
    return savedFiles;
}

void Parser::copyDefaultFont(const std::filesystem::path &assetsFolder)
{
    // The texts' default font must be in the project for its build (and the game) to find it
    const std::filesystem::path font = assetsFolder / DEFAULT_FONT;
    std::error_code error;
    if (std::filesystem::exists(font, error))
        return;
    std::filesystem::copy_file(executableDirectory() / "src" / "editor" / "Assets" / "Font.ttf",
                               font, error);
    if (error)
        std::cerr << "Could not copy the default font to " << font << ": " << error.message()
                  << std::endl;
}

std::filesystem::path Parser::executableDirectory()
{
#ifdef _WIN32
    std::array<char, MAX_PATH> buffer{};
    const DWORD length = GetModuleFileNameA(nullptr, buffer.data(), buffer.size());
    if (length == 0 || length == buffer.size())
        return std::filesystem::current_path();
    return std::filesystem::path(buffer.data(), buffer.data() + length).parent_path();
#else
    std::array<char, PATH_MAX> buffer{};
    const ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length <= 0)
        return std::filesystem::current_path();
    buffer[static_cast<std::size_t>(length)] = '\0';
    return std::filesystem::path(buffer.data()).parent_path();
#endif
}

std::filesystem::path Parser::getProjectFolder(const std::filesystem::path &projectFilePath,
                                               const std::string &projectName)
{
    const std::filesystem::path requestedPath =
        projectFilePath.empty() ? executableDirectory() / projectName / (projectName + ".gringo")
                                : projectFilePath;
    const std::filesystem::path gamePath =
        requestedPath.extension() == ".gringo"
            ? requestedPath
            : requestedPath / (requestedPath.filename().string() + ".gringo");

    return gamePath.parent_path();
}

std::vector<Property> Parser::readScriptParameters(const std::filesystem::path &scriptFile)
{
    std::vector<Property> parameters;
    std::ifstream input(scriptFile);

    // The line to find starts with e.g. "PlayerScript_params"
    const std::string marker = scriptFile.stem().string() + "_params";
    std::string line;

    while (std::getline(input, line)) {
        if (line.find(marker) == std::string::npos)
            continue;

        // Take what is between { and }: a long list can be wrapped on several lines
        std::string list = line;
        while (list.find('}') == std::string::npos && std::getline(input, line))
            list += line;
        std::size_t open = list.find('{');
        std::size_t close = list.find('}', open);
        if (open == std::string::npos || close == std::string::npos)
            return parameters;

        // Split on commas, then drop spaces and quotes from each name
        std::istringstream names(list.substr(open + 1, close - open - 1));
        std::string name;
        while (std::getline(names, name, ',')) {
            name.erase(std::remove_if(
                           name.begin(), name.end(),
                           [](char c) { return c == ' ' || c == '\t' || c == '"' || c == '\r'; }),
                       name.end());
            if (!name.empty())
                parameters.push_back({0, name, "0", valueType::String}); // id set by the caller
        }
        break; // first matching line only
    }
    return parameters;
}
