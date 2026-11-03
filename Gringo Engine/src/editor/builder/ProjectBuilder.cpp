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
                scriptPath(component) != NO_SCRIPT)
                scripts.insert(scriptPath(component));
        }
    };
    for (const Entity &entity : sceneObjects._entities)
        collect(entity.components);
    for (const Prefab &prefab : sceneObjects._prefabs)
        collect(prefab.components);
    return scripts;
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
    const auto hasPlugin = [&sceneObjects](const std::string &name) {
        return std::find(sceneObjects.activePlugins.begin(), sceneObjects.activePlugins.end(),
                         name) != sceneObjects.activePlugins.end();
    };
    const std::set<std::string> scripts = collectScripts(sceneObjects);
    const bool usesNetwork = hasPlugin("Network");
    const bool usesPrefabs = !sceneObjects._prefabs.empty() ||
                             scriptsInclude(projectFolder, scripts, {"PrefabsManager.hpp"});
    std::ofstream source(sourcePath);
    if (!source)
        throw std::runtime_error("Could not create generated source: " + sourcePath.string());

    source << "#include \"Application.hpp\"\n"
           << "#include \"ProjectComponentManager.hpp\"\n"
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
    if (usesNetwork)
        source << "#include \"NetworkConfig.hpp\"\n"
               << "#include <asio.hpp>\n"
               << "#include <charconv>\n"
               << "#include <cstring>\n"
               << "#include <optional>\n";
    if (!scripts.empty()) {
        source << "#include \"ScriptSystem.hpp\"\n";
        for (const std::string &script : scripts)
            source << "#include " << '"' << script << '"' << "\n";
    }

    source << "\nint main(int argc, char **argv)\n{\n";
    if (usesNetwork) {
        source << "    std::string ip = \"127.0.0.1\";\n"
               << "    uint16_t port = 4242;\n"
               << "    if (argc > 3)\n"
               << "        return 84;\n"
               << "    if (argc >= 2) {\n"
               << "        const char *argument = argc == 3 ? argv[2] : argv[1];\n"
               << "        unsigned int parsedPort = 0;\n"
               << "        const char *end = argument + std::strlen(argument);\n"
               << "        auto [pointer, error] = std::from_chars(argument, end, parsedPort);\n"
               << "        if (error != std::errc() || pointer != end || parsedPort == 0 || "
                  "parsedPort > 65535)\n"
               << "            return 84;\n"
               << "        port = static_cast<uint16_t>(parsedPort);\n"
               << "    }\n"
               << "    if (argc == 3) {\n"
               << "        asio::error_code error;\n"
               << "        asio::ip::make_address_v4(argv[1], error);\n"
               << "        if (error)\n"
               << "            return 84;\n"
               << "        ip = argv[1];\n"
               << "    }\n"
               << "    const std::string projectPath = " << cppStringLiteral(projectFile.string())
               << ";\n";
    } else {
        source << "    const std::string projectPath = argc == 1 ? "
               << cppStringLiteral(projectFile.string()) << " : argv[1];\n"
               << "    if (argc > 2)\n"
               << "        return 84;\n";
    }
    source << "    try {\n"
           << "        Application application;\n";
    if (usesPrefabs)
        source << "        application._scene.addResource<PrefabsManager>();\n";
    if (usesNetwork)
        source << "        application._scene.addResource<NetworkConfig>(NetworkConfig::client(ip, "
                  "port));\n";
    for (const std::string &plugin : sceneObjects.activePlugins)
        source << "        application.addPlugin(" << cppStringLiteral(plugin) << ");\n";
    if (!scripts.empty())
        source << "        application._scene.registerSystem<ScriptSystem>(FixedUpdate);\n"
               << "        ComponentSet requirements;\n"
               << "        requirements.set(application._scene.getComponentType<Script>());\n"
               << "        application._scene.setSystemComponentSet<ScriptSystem>(requirements);\n";
    source << "        std::ifstream file(projectPath);\n"
           << "        if (!file)\n"
           << "            throw std::runtime_error(\"Could not open project file: \" + "
              "projectPath);\n"
           << "        nlohmann::json root;\n"
           << "        file >> root;\n"
           << "        Scene &scene = application._scene;\n";
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
               << "                    if (path == \"" << NO_SCRIPT << "\")\n"
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
    const auto hasPlugin = [&sceneObjects](const std::string &name) {
        return std::find(sceneObjects.activePlugins.begin(), sceneObjects.activePlugins.end(),
                         name) != sceneObjects.activePlugins.end();
    };
    const bool usesNetwork = hasPlugin("Network");
    const PluginLoader pluginLoader;
    for (const std::string &plugin : sceneObjects.activePlugins) {
        if (!pluginLoader.isAvailable(plugin))
            throw std::runtime_error("Cannot build project: active plugin \"" + plugin +
                                     "\" is not available");
    }

    // A script including the header of an inactive plugin fails to compile: the game only gets
    // the include directories of its active plugins
    requireComponentPlugins(sceneObjects, descriptors);
    if (sceneObjects.network.enabled && !usesNetwork)
        throw std::runtime_error(
            "Cannot build project: Network must be active when networking is enabled");
    if (!sceneObjects.network.plugin.empty() &&
        std::find(sceneObjects.activePlugins.begin(), sceneObjects.activePlugins.end(),
                  sceneObjects.network.plugin) == sceneObjects.activePlugins.end())
        throw std::runtime_error("Cannot build project: network plugin \"" +
                                 sceneObjects.network.plugin + "\" is not active");
    if (usesNetwork && !sceneObjects.network.enabled)
        throw std::runtime_error(
            "Cannot build project: Network is active but the network configuration is disabled");

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
    // CMake list of the active plugins: "RayLibRenderer;Physics"
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
        " && cmake --build " + shellQuote(buildDirectory.string()) + " --target " +
        shellQuote(target) + " && cmake -S " + shellQuote(resolvedEngineDirectory.string()) +
        " -B " + shellQuote(buildDirectory.string()) +
        " -U GRINGO_GAME_TARGET"
        " -U GRINGO_GAME_OUTPUT_DIR"
        " -U GRINGO_GAME_SOURCE"
        " -U GRINGO_GAME_SCRIPT_DIR"
        " -U GRINGO_GAME_PLUGINS";
    if (std::system(command.c_str()) != 0)
        throw std::runtime_error("Failed to build project executable: " + target);

    return target;
}
