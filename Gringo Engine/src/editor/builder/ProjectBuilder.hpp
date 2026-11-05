#pragma once

#include "BuildVariant.hpp"
#include "ComponentDescriptor.hpp"
#include <filesystem>
#include <initializer_list>
#include <set>
#include <string>
#include <vector>

// Not Structs.hpp: inspectPlugins is defined next to Application.hpp, whose Entity and Prefab
// are not the editor's
struct Component;
struct SceneObjects;

// What the active plugins tell the build: the executables to make (none: one plain executable)
// and the plugins that cannot run without a renderer
struct PluginTraits {
    std::vector<BuildVariant> variants;
    std::set<std::string> needRenderer;
};

class ProjectBuilder
{
  public:
    static std::string build(const std::string &projectPath, const SceneObjects &sceneObjects,
                             const std::vector<ComponentDescriptor> &descriptors);
    // Everything that makes the build impossible, one "Impossible to build <project>: ..." line
    // each: missing scripts and assets, ChildOf pointing to no entity, to itself, or in a loop
    static std::vector<std::string>
    buildErrors(const SceneObjects &sceneObjects, const std::filesystem::path &projectFolder,
                const std::vector<ComponentDescriptor> &descriptors);
    // Loads each active plugin. Throws when more than one of them declares build variants
    static PluginTraits inspectPlugins(const std::vector<std::string> &activePlugins);

  private:
    // File properties (scripts, textures...) pointing to a missing file
    static std::vector<std::string>
    missingFiles(const SceneObjects &sceneObjects, const std::filesystem::path &projectFolder,
                 const std::vector<ComponentDescriptor> &descriptors);
    static std::vector<std::string> hierarchyErrors(const SceneObjects &sceneObjects);
    // The Parent of a ChildOf component, -1 if there is none or it is not a number
    static int parentIdOf(const std::vector<Component> &components);
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
                               std::initializer_list<std::string> headers);
    static void writeGeneratedMain(const std::filesystem::path &sourcePath,
                                   const std::filesystem::path &projectFile,
                                   const std::filesystem::path &projectFolder,
                                   const SceneObjects &sceneObjects, const BuildVariant &variant,
                                   const std::set<std::string> &skippedPlugins,
                                   const std::set<std::string> &ignoredComponents);
};
