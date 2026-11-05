#include "CommandLineBuild.hpp"
#include "ComponentCatalog.hpp"
#include "Parser.hpp"
#include "ProjectBuilder.hpp"
#include <exception>
#include <filesystem>
#include <iostream>

int buildFromCommandLine(const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1) {
        std::cerr << "Usage: Gringo --build <file.gringo>\n";
        return 84;
    }
    const std::string path = std::filesystem::absolute(arguments[0]).string();
    try {
        const SceneObjects scene = Parser{}.loadFile(path);
        if (scene.projectName.empty()) {
            std::cerr << "Could not load " << path << '\n';
            return 84;
        }
        const ComponentCatalog catalog;
        ProjectBuilder::build(path, scene, catalog.getDescriptors());
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 84;
    }
    return 0;
}
