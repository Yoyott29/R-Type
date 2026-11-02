/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Parser
*/

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "Structs.hpp"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

class Parser
{
  public:
    Parser() = default;
    ~Parser() = default;

    void saveFile(const std::string &filePath, const SceneObjects &sceneObjects);
    SceneObjects loadFile(const std::string &filePath);
    std::vector<std::string> getSavedFiles();
    const std::filesystem::path &getLastSavedPath() const { return _lastSavedPath; }
    static std::filesystem::path getProjectFolder(const std::filesystem::path &projectFilePath,
                                                  const std::string &projectName);

  private:
    static nlohmann::json serializeComponents(const std::vector<Component> &components);
    static std::vector<Component> deserializeComponents(const nlohmann::json &json);
    static std::filesystem::path executableDirectory();
    std::filesystem::path _lastSavedPath;
};

#endif /* !PARSER_HPP_ */
