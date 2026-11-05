#include "CommandLineBuild.hpp"
#include "ComponentCatalog.hpp"
#include "Parser.hpp"
#include "ProjectBuilder.hpp"
#include <exception>
#include <filesystem>
#include <iostream>

static constexpr int BUILD_FAILED_EXIT_CODE = 84;

int buildFromCommandLine(const std::vector<std::string> &arguments)
{
    if (arguments.size() != 1) {
        std::cerr << "Usage: Gringo --build <file.gringo>\n";
        return BUILD_FAILED_EXIT_CODE;
    }
    const std::string path = std::filesystem::absolute(arguments[0]).string();
    try {
        const SceneObjects scene = Parser{}.loadFile(path);
        if (scene.projectName.empty()) {
            std::cerr << "Could not load " << path << '\n';
            return BUILD_FAILED_EXIT_CODE;
        }
        const ComponentCatalog catalog;
        ProjectBuilder::build(path, scene, catalog.getDescriptors());
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return BUILD_FAILED_EXIT_CODE;
    }
    return 0;
}
