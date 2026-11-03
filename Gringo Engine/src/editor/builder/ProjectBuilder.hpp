#pragma once

#include "Structs.hpp"
#include <filesystem>
#include <initializer_list>
#include <set>
#include <string>
#include <vector>

class ProjectBuilder
{
  public:
    static std::string build(const std::string &projectPath, const SceneObjects &sceneObjects,
                             const std::vector<ComponentDescriptor> &descriptors);

  private:
    static std::string shellQuote(const std::string &value);
    static std::string cppStringLiteral(const std::string &value);
    static std::string targetName(const std::string &projectName);
    static std::string scriptPath(const Component &component);
    static std::string className(const std::string &path);
    static std::set<std::string> collectScripts(const SceneObjects &sceneObjects);
    // The project has a renderer, and it is in Gringo Engine/renderers
    static void requireRenderer(const SceneObjects &sceneObjects);
    static void requireComponentPlugins(const SceneObjects &sceneObjects,
                                        const std::vector<ComponentDescriptor> &descriptors);
    static bool scriptsInclude(const std::filesystem::path &projectFolder,
                               const std::set<std::string> &scripts,
                               std::initializer_list<const char *> headers);
    static void writeGeneratedMain(const std::filesystem::path &sourcePath,
                                   const std::filesystem::path &projectFile,
                                   const std::filesystem::path &projectFolder,
                                   const SceneObjects &sceneObjects);
};
