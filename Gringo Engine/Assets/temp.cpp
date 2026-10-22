/*
** EPITECH PROJECT, 2026
** Test
** File description:
** main
*/

#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

struct Component {
    std::string name;
    std::map<std::string, std::string> properties;
};

struct Entity {
    int id;
    std::string name;
    std::vector<Component> components;
};

void from_json(const nlohmann::json& j, Entity& e)
{
    j.at("id").get_to(e.id);
    j.at("name").get_to(e.name);

    const auto& comps = j.at("components");
    for (auto it = comps.begin(); it != comps.end(); ++it) {
        Component c;
        c.name = it.key();
        c.properties = it.value().get<std::map<std::string, std::string>>();
        e.components.push_back(c);
    }

    std::cout << "Entity loaded: " << e.name << std::endl;
    for (const auto& c : e.components) {
        std::cout << "  Component: " << c.name << std::endl;
        for (const auto& p : c.properties) {
            std::cout << "    " << p.first << ": " << p.second << std::endl;
        }
    }
}


void to_json(nlohmann::json& j, const Entity& e)
{
    nlohmann::json comps = nlohmann::json::object();

    for (const auto& c : e.components)
        comps[c.name] = c.properties;
    j = nlohmann::json{{"id", e.id},{"name", e.name},{"components", comps}};
}

void loadScene(const std::string& filename, std::vector<Entity>& entities)
{
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Cannot open " << filename << std::endl;
        return;
    }

    nlohmann::json json;
    file >> json;
    entities = json.at("entities").get<std::vector<Entity>>();
}

void saveScene(const std::string& filename, const std::vector<Entity>& entities)
{
    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Cannot open " << filename << std::endl;
        return;
    }

    nlohmann::json json;
    json["entities"] = entities;
    file << json.dump(4);
}


int main(int argc, char **argv)
{
    std::vector<Entity> entities = {
        Entity{1, "Player", {
            Component{"Position", {{"x", "100"}, {"y", "200"}}},
            Component{"Script",   {{"file", "scripts/player.cpp"}}},
        }},
        Entity{2, "Ship", {
            Component{"Sprite",   {{"path", "assets/ship.png"}}},
            Component{"Position", {{"x", "300"}, {"y", "50"}}},
        }},
        Entity{3, "Enemy", {
            Component{"Sprite",   {{"path", "assets/enemy.png"}}},
            Component{"Position", {{"x", "500"}, {"y", "120"}}},
            Component{"Script",   {{"file", "scripts/enemy.cpp"}}},
        }},
    };

    if (argv[1] == std::string("save"))
        saveScene("R-Type.gringo", entities);
    else if (argv[1] == std::string("load"))
        loadScene("R-Type.gringo", entities);

    return 0;
}