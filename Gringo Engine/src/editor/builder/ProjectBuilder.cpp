#include "ProjectBuilder.hpp"
#include "PluginLoader.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <set>
#include <stdexcept>

std::string ProjectBuilder::shellQuote(const std::string &value)
{
    std::string result = "'";
    for (char character : value) {
        if (character == '\'')
            result += "'\\''";
        else
            result += character;
    }
    return result + "'";
}

std::string ProjectBuilder::cppStringLiteral(const std::string &value)
{
    std::string result = "\"";
    for (char character : value) {
        if (character == '\\' || character == '"')
            result += '\\';
        result += character;
    }
    return result + "\"";
}

std::string ProjectBuilder::targetName(const std::string &projectName)
{
    std::string result = "";
    for (char character : projectName)
        result += std::isalnum(static_cast<unsigned char>(character)) ? character : '_';
    return result;
}

std::string ProjectBuilder::scriptPath(const Component &component)
{
    for (const Property &property : component.properties)
        if (property.name == "Path")
            return property.value;
    return {};
}

std::string ProjectBuilder::className(const std::string &path)
{
    return std::filesystem::path(path).stem().string();
}

std::set<std::string> ProjectBuilder::collectScripts(const SceneObjects &sceneObjects)
{
    std::set<std::string> scripts;
    const auto collect = [&scripts](const std::vector<Component> &components) {
        for (const Component &component : components) {
            if (component.name == "Script" && !scriptPath(component).empty() &&
                scriptPath(component) != NO_FILE)
                scripts.insert(scriptPath(component));
        }
    };
    for (const Entity &entity : sceneObjects._entities)
        collect(entity.components);
    for (const Prefab &prefab : sceneObjects._prefabs)
        collect(prefab.components);
    return scripts;
}

void ProjectBuilder::requireRenderer(const SceneObjects &sceneObjects)
{
    if (sceneObjects.renderer.empty())
        throw std::runtime_error("Cannot build project: it has no renderer, choose one in Settings");
    if (!PluginLoader("renderers").isAvailable(sceneObjects.renderer))
        throw std::runtime_error("Cannot build project: renderer \"" + sceneObjects.renderer +
                                 "\" is not available");
}

void ProjectBuilder::requireComponentPlugins(const SceneObjects &sceneObjects,
                                             const std::vector<ComponentDescriptor> &descriptors)
{
    const auto check = [&](const std::vector<Component> &components) {
        for (const Component &component : components) {
            const auto descriptor = std::find_if(
                descriptors.begin(), descriptors.end(),
                [&component](const ComponentDescriptor &candidate) {
                    return candidate.name == component.name;
                });
            if (descriptor == descriptors.end())
                throw std::runtime_error("Cannot build project: no available plugin provides the "
                                         "component \"" + component.name + "\"");
            if (!descriptor->plugin.empty() &&
                std::find(sceneObjects.activePlugins.begin(), sceneObjects.activePlugins.end(),
                          descriptor->plugin) == sceneObjects.activePlugins.end())
                throw std::runtime_error("Cannot build project: " + descriptor->plugin +
                                         " must be active for the component " + component.name);
        }
    };
    for (const Entity &entity : sceneObjects._entities)
        check(entity.components);
    for (const Prefab &prefab : sceneObjects._prefabs)
        check(prefab.components);
}

bool ProjectBuilder::scriptsInclude(const std::filesystem::path &projectFolder,
                                    const std::set<std::string> &scripts,
                                    std::initializer_list<const char *> headers)
{
    for (const std::string &script : scripts) {
        std::ifstream input(projectFolder / "scripts" / script);
        const std::string source((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
        for (const char *header : headers)
            if (source.find(std::string{"#include \""} + header + '"') != std::string::npos)
                return true;
    }
    return false;
}

void ProjectBuilder::writeGeneratedMain(const std::filesystem::path &sourcePath,
                                        const std::filesystem::path &projectFile,
                                        const std::filesystem::path &projectFolder,
                                        const SceneObjects &sceneObjects)
{
    const std::set<std::string> scripts = collectScripts(sceneObjects);
    const bool usesPrefabs = !sceneObjects._prefabs.empty() ||
                             scriptsInclude(projectFolder, scripts, {"PrefabsManager.hpp"});
    std::ofstream source(sourcePath);
    if (!source)
        throw std::runtime_error("Could not create generated source: " + sourcePath.string());

    source << "#include \"Application.hpp\"\n"
           << "#include \"ProjectComponentManager.hpp\"\n"
           << "#include \"ProjectSettings.hpp\"\n"
           << "#include \"nlohmann/json.hpp\"\n"
           << "#include <filesystem>\n"
           << "#include <fstream>\n"
           << "#include <iostream>\n"
           << "#include <memory>\n"
           << "#include <stdexcept>\n"
           << "#include <string>\n";
    if (usesPrefabs)
        source << "#include \"Prefab.hpp\"\n"
               << "#include \"PrefabsManager.hpp\"\n";
    if (!scripts.empty()) {
        source << "#include \"Script.hpp\"\n";
        for (const std::string &script : scripts)
            source << "#include " << '"' << script << '"' << "\n";
    }

    // Nothing plugin-specific here: plugins read their settings (ProjectSettings) and the
    // arguments they understand (CommandLine), e.g. Network's "[host] port"
    source << "\nint main(int argc, char **argv)\n{\n"
           << "    const std::string projectPath = " << cppStringLiteral(projectFile.string())
           << ";\n"
           << "    try {\n"
           << "        std::ifstream file(projectPath);\n"
           << "        if (!file)\n"
           << "            throw std::runtime_error(\"Could not open project file: \" + "
              "projectPath);\n"
           << "        nlohmann::json root;\n"
           << "        file >> root;\n"
           << "        Application application(argc, argv);\n"
           << "        Scene &scene = application._scene;\n"
           << "        scene.addResource<ProjectSettings>(ProjectSettings::fromJson(\n"
           << "            root.value(\"pluginSettings\", nlohmann::json::object())));\n";
    if (usesPrefabs)
        source << "        scene.addResource<PrefabsManager>();\n";
    // The renderer opens the window, before the plugins that draw through it
    source << "        application.setRenderer(" << cppStringLiteral(sceneObjects.renderer)
           << ", " << cppStringLiteral(sceneObjects.projectName) << ");\n";
    for (const std::string &plugin : sceneObjects.activePlugins)
        source << "        application.addPlugin(" << cppStringLiteral(plugin) << ");\n";
    if (!scripts.empty())
        source << "        const auto applyParameters = [](const "
                  "std::shared_ptr<GringoEntityScript> &script,\n"
               << "                                        const nlohmann::json &component) {\n"
               << "            for (const auto &property : component.value(\"properties\", "
                  "nlohmann::json::array())) {\n"
               << "                const std::string name = property.value(\"name\", \"\");\n"
               << "                if (name != \"Path\")\n"
               << "                    script->setParameter(name, property.value(\"value\", "
                  "\"\"));\n"
               << "            }\n"
               << "        };\n";
    source << "        ProjectComponentManager components(scene, "
              "std::filesystem::path(projectPath).parent_path());\n"
           << "        for (const auto &entityData : root.value(\"entities\", "
              "nlohmann::json::array())) {\n"
           << "            Entity entity = scene.createEntity();\n"
           << "            for (const auto &component : entityData.value(\"components\", "
              "nlohmann::json::array())) {\n";
    if (!scripts.empty()) {
        source << "                if (component.value(\"name\", \"\") == \"Script\") {\n"
               << "                    std::string path;\n"
               << "                    for (const auto &property : component.value(\"properties\", "
                  "nlohmann::json::array()))\n"
               << "                        if (property.value(\"name\", \"\") == \"Path\")\n"
               << "                            path = property.value(\"value\", \"\");\n"
               << "                    if (path == \"" << NO_FILE << "\")\n"
               << "                        continue;\n";
        bool firstScript = true;
        for (const std::string &script : scripts) {
            source << "                    " << (firstScript ? "if" : "else if")
                   << " (path == " << '"' << script << '"' << ") {\n"
                   << "                        auto instance = std::make_shared<"
                   << className(script) << ">();\n"
                   << "                        applyParameters(instance, component);\n"
                   << "                        scene.addComponent(entity, Script{instance, "
                      "false});\n"
                   << "                    }\n";
            firstScript = false;
        }
        source << "                    else\n"
               << "                        throw std::runtime_error(\"Unknown script: \" + path);\n"
               << "                } else\n";
    }
    source << "                    components.addToScene(scene, entity, component);\n"
           << "            }\n"
           << "        }\n"
           << "        for (const auto &prefabData : root.value(\"prefabs\", "
              "nlohmann::json::array())) {\n"
           << "            Prefab prefab;\n"
           << "            for (const auto &component : prefabData.value(\"components\", "
              "nlohmann::json::array())) {\n";
    if (!scripts.empty()) {
        source << "                if (component.value(\"name\", \"\") == \"Script\") {\n"
               << "                    std::string path;\n"
               << "                    for (const auto &property : component.value(\"properties\", "
                  "nlohmann::json::array()))\n"
               << "                        if (property.value(\"name\", \"\") == \"Path\")\n"
               << "                            path = property.value(\"value\", \"\");\n";
        bool firstScript = true;
        for (const std::string &script : scripts) {
            source << "                    " << (firstScript ? "if" : "else if")
                   << " (path == " << '"' << script << '"' << ") {\n"
                   << "                        auto instance = std::make_shared<"
                   << className(script) << ">();\n"
                   << "                        applyParameters(instance, component);\n"
                   << "                        prefab.addComponent(Script{instance, false});\n"
                   << "                    }\n";
            firstScript = false;
        }
        source << "                    else\n"
               << "                        throw std::runtime_error(\"Unknown script: \" + path);\n"
               << "                } else\n";
    }
    source << "                    components.addToPrefab(prefab, scene, component);\n"
           << "            }\n"
           << "            application._scene.getResource<PrefabsManager>().createPrefab("
              "prefabData.value(\"name\", \"\"), prefab);\n"
           << "        }\n"
           << "        application.run();\n"
           << "    } catch (const std::exception &error) {\n"
           << "        std::cerr << \"Runtime error: \" << error.what() << '\\n';\n"
           << "        return 84;\n"
           << "    }\n"
           << "    return 0;\n"
           << "}\n";
}

std::string ProjectBuilder::build(const std::string &projectPath, const SceneObjects &sceneObjects,
                                  const std::vector<ComponentDescriptor> &descriptors)
{
    const std::filesystem::path projectFile(projectPath);
    const std::filesystem::path projectFolder = projectFile.parent_path();
    const std::filesystem::path generatedSource = projectFolder / "main.cpp";
    const std::string target = targetName(sceneObjects.projectName);
    const std::set<std::string> scripts = collectScripts(sceneObjects);
    const PluginLoader pluginLoader;
    for (const std::string &plugin : sceneObjects.activePlugins) {
        if (!pluginLoader.isAvailable(plugin))
            throw std::runtime_error("Cannot build project: active plugin \"" + plugin +
                                     "\" is not available");
    }

    // A script including the header of an inactive plugin fails to compile: the game only gets
    // the include directories of its active plugins
    requireComponentPlugins(sceneObjects, descriptors);
    requireRenderer(sceneObjects);

    std::filesystem::create_directories(projectFolder);
    for (const std::string &script : scripts) {
        const std::filesystem::path scriptFile = projectFolder / "scripts" / script;
        std::ifstream input(scriptFile);
        if (!input)
            throw std::runtime_error("Could not open script: " + scriptFile.string());

        const std::string source((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
        if (source.find("onStart") == std::string::npos ||
            source.find("onUpdate") == std::string::npos)
            throw std::runtime_error("Script must define onStart and onUpdate: " +
                                     scriptFile.string());
    }
    writeGeneratedMain(generatedSource, projectFile, projectFolder, sceneObjects);

    const std::filesystem::path workingDirectory = std::filesystem::current_path();
    const std::filesystem::path engineDirectory = workingDirectory / "Gringo Engine";
    const std::filesystem::path resolvedEngineDirectory =
        std::filesystem::is_directory(engineDirectory) ? engineDirectory : workingDirectory;
    const std::filesystem::path buildDirectory = projectFolder / "build";
    // CMake list of the active plugins: "2D;Physics"
    std::string pluginList;
    for (const std::string &plugin : sceneObjects.activePlugins)
        pluginList += (pluginList.empty() ? "" : ";") + plugin;
    std::string command =
        "cmake -S " + shellQuote(resolvedEngineDirectory.string()) + " -B " +
        shellQuote(buildDirectory.string()) +
        " "
        "-DGRINGO_GAME_TARGET=" +
        shellQuote(target) + " -DGRINGO_GAME_OUTPUT_DIR=" + shellQuote(projectFolder.string()) +
        " -DGRINGO_GAME_SOURCE=" + shellQuote(generatedSource.string()) +
        " -DGRINGO_GAME_SCRIPT_DIR=" + shellQuote((projectFolder / "scripts").string()) +
        " -DGRINGO_GAME_PLUGINS=" + shellQuote(pluginList) +
        " -DGRINGO_GAME_RENDERER=" + shellQuote(sceneObjects.renderer) +
        " && cmake --build " + shellQuote(buildDirectory.string()) + " --target " +
        shellQuote(target) + " && cmake -S " + shellQuote(resolvedEngineDirectory.string()) +
        " -B " + shellQuote(buildDirectory.string()) +
        " -U GRINGO_GAME_TARGET"
        " -U GRINGO_GAME_OUTPUT_DIR"
        " -U GRINGO_GAME_SOURCE"
        " -U GRINGO_GAME_SCRIPT_DIR"
        " -U GRINGO_GAME_PLUGINS"
        " -U GRINGO_GAME_RENDERER";
    if (std::system(command.c_str()) != 0)
        throw std::runtime_error("Failed to build project executable: " + target);

    return target;
}
