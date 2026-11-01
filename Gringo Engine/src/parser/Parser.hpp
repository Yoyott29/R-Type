/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Parser
*/

#ifndef PARSER_HPP_
#define PARSER_HPP_

#define GAME_FOLDER "gringo files/"

#include <string>
#include "Structs.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

class Parser {
    public:
        Parser() = default;
        ~Parser() = default;

        void saveFile(const std::string &filePath, const SceneObjects &sceneObjects);
        SceneObjects loadFile(const std::string &filePath);
        std::vector<std::string> getSavedFiles();

    private:
        static nlohmann::json serializeComponents(const std::vector<Component> &components);
        static std::vector<Component> deserializeComponents(const nlohmann::json &json);
};

#endif /* !PARSER_HPP_ */
