#include "ProjectBuilder.hpp"
#include "PluginLoader.hpp"
#include "Structs.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <set>
#include <stdexcept>

// std::system runs the command with cmd.exe on Windows and /bin/sh elsewhere: each has its own
// quoting. cmd.exe does not understand single quotes, so a path holding a space (Gringo Engine)
// would be split in two
std::string ProjectBuilder::shellQuote(const std::string &value)
{
#if defined(_WIN32)
    // Double quotes; a Windows path cannot hold one, but escape it for CMake's own parser anyway
    std::string result = "\"";
    for (char character : value) {
        if (character == '"')
            result += "\\\"";
        else
            result += character;
    }
    return result + "\"";
#else
    std::string result = "'";
    for (char character : value) {
        if (character == '\'')
            result += "'\\''";
        else
            result += character;
    }
    return result + "'";
#endif
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
        if (std::isalnum(static_cast<unsigned char>(character)))
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        else
            result += character == '-' ? '-' : '_';
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

std::set<std::string> ProjectBuilder::collectButtonScripts(const SceneObjects &sceneObjects)
{
    std::set<std::string> scripts;
    const auto collect = [&scripts](const std::vector<Component> &components) {
        for (const Component &component : components) {
            if (component.name != "Button")
                continue;
            for (const Property &property : component.properties)
                if (property.name == "Script" && !property.value.empty() &&
                    property.value != NO_FILE)
                    scripts.insert(property.value);
        }
    };
    for (const Entity &entity : sceneObjects._entities)
        collect(entity.components);
    for (const Prefab &prefab : sceneObjects._prefabs)
        collect(prefab.components);
    return scripts;
}

void ProjectBuilder::writeButtonScripts(std::ostream &source,
                                        const std::set<std::string> &buttonScripts,
                                        const std::string &assignment)
{
    if (buttonScripts.empty())
        return;
    source << "                    if (component.value(\"name\", \"\") == \"Button\") {\n"
           << "                        std::string path;\n"
           << "                        for (const auto &property : component.value(\"properties\", "
              "nlohmann::json::array()))\n"
           << "                            if (property.value(\"name\", \"\") == \"Script\")\n"
           << "                                path = property.value(\"value\", \"\");\n";
    for (const std::string &script : buttonScripts) {
        std::string statement = assignment;
        statement.replace(statement.find("SCRIPT"), 6,
                          "makeButtonScript<" + className(script) + ">(component)");
        source << "                        if (path == " << cppStringLiteral(script) << ")\n"
               << "                            " << statement << "\n";
    }
    source << "                    }\n";
}

std::vector<std::string>
ProjectBuilder::missingFiles(const SceneObjects &sceneObjects,
                             const std::filesystem::path &projectFolder,
                             const std::vector<ComponentDescriptor> &descriptors)
{
    std::vector<std::string> missing;
    const auto check = [&](const std::string &kind, const std::string &object,
                           const std::vector<Component> &components) {
        for (const Component &component : components) {
            const auto descriptor =
                std::find_if(descriptors.begin(), descriptors.end(),
                             [&component](const ComponentDescriptor &candidate) {
                                 return candidate.name == component.name;
                             });
            if (descriptor == descriptors.end())
                continue;
            for (const Property &property : component.properties) {
                const auto setting =
                    std::find_if(descriptor->properties.begin(), descriptor->properties.end(),
                                 [&property](const PropertyDescriptor &candidate) {
                                     return candidate.name == property.name;
                                 });
                if (setting == descriptor->properties.end() || setting->type != valueType::File ||
                    setting->options.empty())
                    continue;
                // No file chosen ("default.png" is how older projects said it)
                if (property.value.empty() || property.value == NO_FILE ||
                    property.value == "default.png")
                    continue;
                // "assets/Player.png" and "Player.png" both mean <project>/assets/Player.png
                const std::string &folder = setting->options.front();
                std::filesystem::path relative = property.value;
                if (relative.begin() != relative.end() && *relative.begin() == folder)
                    relative = relative.lexically_relative(*relative.begin());
                const std::filesystem::path file = std::filesystem::path(folder) / relative;
                if (!std::filesystem::is_regular_file(projectFolder / file))
                    missing.push_back("Impossible to build " + sceneObjects.projectName +
                                      ": missing asset: " + file.generic_string() + " (" + kind +
                                      " " + object + ", " + component.name + "." + property.name +
                                      ")");
            }
        }
    };
    for (const Entity &entity : sceneObjects._entities)
        check("entity", entity.name, entity.components);
    for (const Prefab &prefab : sceneObjects._prefabs)
        check("prefab", prefab.name, prefab.components);
    return missing;
}

int ProjectBuilder::parentIdOf(const std::vector<Component> &components)
{
    for (const Component &component : components) {
        if (component.name != "ChildOf")
            continue;
        for (const Property &property : component.properties)
            if (property.name == "Parent") {
                try {
                    return std::stoi(property.value);
                } catch (const std::exception &) {
                    return -1;
                }
            }
    }
    return -1;
}

std::vector<std::string> ProjectBuilder::hierarchyErrors(const SceneObjects &sceneObjects)
{
    std::vector<std::string> errors;
    const std::string prefix = "Impossible to build " + sceneObjects.projectName + ": ";
    const auto hasChildOf = [](const std::vector<Component> &components) {
        return std::any_of(components.begin(), components.end(),
                           [](const Component &component) { return component.name == "ChildOf"; });
    };
    const auto entityWithId = [&sceneObjects](int id) {
        return std::find_if(sceneObjects._entities.begin(), sceneObjects._entities.end(),
                            [id](const Entity &entity) { return entity.id == id; });
    };

    for (const Entity &entity : sceneObjects._entities) {
        if (!hasChildOf(entity.components))
            continue;
        const std::string name = "entity " + entity.name + " [" + std::to_string(entity.id) + "]";
        const int parentId = parentIdOf(entity.components);
        if (parentId == entity.id) {
            errors.push_back(prefix + name + " is its own parent");
            continue;
        }
        if (entityWithId(parentId) == sceneObjects._entities.end()) {
            errors.push_back(prefix + name + " is ChildOf unknown entity " +
                             std::to_string(parentId));
            continue;
        }
        // Goes up the parents: coming back to this entity means a loop
        int current = parentId;
        for (std::size_t steps = 0; steps < sceneObjects._entities.size(); ++steps) {
            auto parent = entityWithId(current);
            if (parent == sceneObjects._entities.end() || !hasChildOf(parent->components))
                break;
            current = parentIdOf(parent->components);
            if (current == entity.id) {
                errors.push_back(prefix + name + " is in a ChildOf loop");
                break;
            }
        }
    }
    // A prefab can be the child of a scene entity
    for (const Prefab &prefab : sceneObjects._prefabs) {
        if (!hasChildOf(prefab.components))
            continue;
        const int parentId = parentIdOf(prefab.components);
        if (entityWithId(parentId) == sceneObjects._entities.end())
            errors.push_back(prefix + "prefab " + prefab.name + " is ChildOf unknown entity " +
                             std::to_string(parentId));
    }
    return errors;
}

std::vector<std::string> ProjectBuilder::prefabErrors(const SceneObjects &sceneObjects)
{
    std::vector<std::string> errors;
    const auto exists = [&sceneObjects](const std::string &name) {
        return std::any_of(sceneObjects._prefabs.begin(), sceneObjects._prefabs.end(),
                           [&name](const Prefab &prefab) { return prefab.name == name; });
    };

    for (const Entity &entity : sceneObjects._entities)
        for (const Component &component : entity.components)
            for (const Property &property : component.properties)
                if (property.type == valueType::Prefab && property.value != NO_FILE &&
                    !exists(property.value))
                    errors.push_back("Impossible to build " + sceneObjects.projectName +
                                     ": entity " + entity.name + " uses unknown prefab " +
                                     property.value);
    return errors;
}

std::vector<std::string>
ProjectBuilder::buildErrors(const SceneObjects &sceneObjects,
                            const std::filesystem::path &projectFolder,
                            const std::vector<ComponentDescriptor> &descriptors)
{
    std::vector<std::string> errors = missingFiles(sceneObjects, projectFolder, descriptors);
    for (std::string &error : hierarchyErrors(sceneObjects))
        errors.push_back(std::move(error));
    for (std::string &error : prefabErrors(sceneObjects))
        errors.push_back(std::move(error));
    return errors;
}

void ProjectBuilder::requireRenderer(const SceneObjects &sceneObjects)
{
    if (sceneObjects.renderer.empty())
        throw std::runtime_error(
            "Cannot build project: it has no renderer, choose one in Settings");
    if (!PluginLoader("renderers").isAvailable(sceneObjects.renderer))
        throw std::runtime_error("Cannot build project: renderer \"" + sceneObjects.renderer +
                                 "\" is not available");
}

void ProjectBuilder::requireComponentPlugins(const SceneObjects &sceneObjects,
                                             const std::vector<ComponentDescriptor> &descriptors)
{
    const auto check = [&](const std::vector<Component> &components) {
        for (const Component &component : components) {
            const auto descriptor =
                std::find_if(descriptors.begin(), descriptors.end(),
                             [&component](const ComponentDescriptor &candidate) {
                                 return candidate.name == component.name;
                             });
            if (descriptor == descriptors.end())
                throw std::runtime_error("Cannot build project: no available plugin provides the "
                                         "component \"" +
                                         component.name + "\"");
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
                                    std::initializer_list<std::string> headers)
{
    for (const std::string &script : scripts) {
        std::ifstream input(projectFolder / "scripts" / script);
        const std::string source((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
        for (const std::string &header : headers)
            if (source.find(std::string{"#include \""} + header + '"') != std::string::npos)
                return true;
    }
    return false;
}

void ProjectBuilder::writeGeneratedMain(const std::filesystem::path &sourcePath,
                                        const std::filesystem::path &projectFile,
                                        const std::filesystem::path &projectFolder,
                                        const SceneObjects &sceneObjects,
                                        const BuildVariant &variant,
                                        const std::set<std::string> &skippedPlugins,
                                        const std::set<std::string> &ignoredComponents)
{
    const std::set<std::string> scripts = collectScripts(sceneObjects);
    std::set<std::string> buttonScripts;
    if (!ignoredComponents.count("Button"))
        buttonScripts = collectButtonScripts(sceneObjects);
    std::set<std::string> allScripts = scripts;
    allScripts.insert(buttonScripts.begin(), buttonScripts.end());
    const bool usesPrefabs = !sceneObjects._prefabs.empty() ||
                             scriptsInclude(projectFolder, allScripts, {"PrefabsManager.hpp"});
    std::ofstream source(sourcePath);
    if (!source)
        throw std::runtime_error("Could not create generated source: " + sourcePath.string());

    source << "#include \"Application.hpp\"\n"
           << "#include \"BuildVariantName.hpp\"\n"
           << "#include \"ProjectComponentManager.hpp\"\n"
           << "#include \"ProjectSettings.hpp\"\n"
           << "#include \"nlohmann/json.hpp\"\n"
           << "#include <cstdlib>\n"
           << "#include <ctime>\n"
           << "#include <filesystem>\n"
           << "#include <fstream>\n"
           << "#include <functional>\n"
           << "#include <iostream>\n"
           << "#include <memory>\n"
           << "#include <stdexcept>\n"
           << "#include <string>\n"
           << "#include <vector>\n";
    if (usesPrefabs)
        source << "#include \"Prefab.hpp\"\n"
               << "#include \"PrefabsManager.hpp\"\n";
    if (!scripts.empty())
        source << "#include \"Script.hpp\"\n";
    if (!buttonScripts.empty())
        source << "#include \"UIButton.hpp\"\n";
    for (const std::string &script : allScripts)
        source << "#include " << '"' << script << '"' << "\n";

    // A button script gets the values of its component's properties as parameters, like the
    // entity scripts (its own properties too: it ignores the names it does not know)
    if (!buttonScripts.empty())
        source << "\ntemplate <typename ButtonScript>\n"
               << "static std::shared_ptr<IGringoButtonScript> makeButtonScript(const "
                  "nlohmann::json &component)\n"
               << "{\n"
               << "    auto script = std::make_shared<ButtonScript>();\n"
               << "    for (const auto &property : component.value(\"properties\", "
                  "nlohmann::json::array())) {\n"
               << "        const std::string name = property.value(\"name\", \"\");\n"
               << "        if (name != \"Script\")\n"
               << "            script->setParameter(name, property.value(\"value\", \"\"));\n"
               << "    }\n"
               << "    return script;\n"
               << "}\n";

    // Nothing plugin-specific here: plugins read their settings (ProjectSettings) and the
    // arguments they understand (CommandLine), e.g. Network's "[host] port"
    // The .gringo is found next to the executable, so the project folder can be moved
    // Seeded once with the time: std::rand gives other numbers every game
    source << "\nint main(int argc, char **argv)\n{\n"
           << "    std::srand(static_cast<unsigned int>(std::time(nullptr)));\n"
           << "    try {\n"
           << "        const std::filesystem::path projectPath =\n"
           << "            ProjectComponentManager::findProjectFile("
           << cppStringLiteral(projectFile.filename().string()) << ");\n"
           << "        std::ifstream file(projectPath);\n"
           << "        if (!file)\n"
           << "            throw std::runtime_error(\"Could not open project file: \" + "
              "projectPath.string());\n"
           << "        nlohmann::json root;\n"
           << "        file >> root;\n"
           << "        Application application(std::vector<std::string>(argv + 1, argv + argc));\n"
           << "        Scene &scene = application._scene;\n"
           << "        scene.addResource<ProjectSettings>(ProjectSettings::fromJson(\n"
           << "            root.value(\"pluginSettings\", nlohmann::json::object())));\n"
           << "        scene.addResource<BuildVariantName>(BuildVariantName{"
           << cppStringLiteral(variant.name) << "});\n";
    if (usesPrefabs)
        source << "        scene.addResource<PrefabsManager>();\n";
    // The renderer opens the window, before the plugins that draw through it. A headless
    // executable opens none
    if (!variant.headless)
        source << "        application.setRenderer(" << cppStringLiteral(sceneObjects.renderer)
               << ", " << cppStringLiteral(sceneObjects.projectName) << ");\n";
    for (const std::string &plugin : sceneObjects.activePlugins)
        if (!skippedPlugins.count(plugin))
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
    source << "        ProjectComponentManager components(scene, projectPath.parent_path());\n";
    // The components of the skipped plugins are not registered: they are left out of the scene
    if (!ignoredComponents.empty()) {
        source << "        components.ignore({";
        bool firstComponent = true;
        for (const std::string &component : ignoredComponents) {
            source << (firstComponent ? "" : ", ") << cppStringLiteral(component);
            firstComponent = false;
        }
        source << "});\n";
    }
    source << "        for (const auto &entityData : root.value(\"entities\", "
              "nlohmann::json::array())) {\n"
           << "            Entity entity = scene.createEntity();\n"
           << "            components.addEntityId(scene, entityData.value(\"id\", -1), entity);\n"
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
               << "                } else {\n";
    } else
        source << "                {\n";
    source << "                    components.addToScene(scene, entity, component);\n";
    writeButtonScripts(source, buttonScripts,
                       "scene.getComponent<UIButton>(entity).script = SCRIPT;");
    source << "                }\n"
           << "            }\n"
           << "        }\n"
           << "        components.linkChildren(scene);\n"
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
                   << "                        // Each copy of the prefab gets its own script\n"
                   << "                        prefab.addComponent(std::function<void(Scene &, "
                      "Entity)>([component, applyParameters](Scene &target, Entity copy) {\n"
                   << "                            auto instance = std::make_shared<"
                   << className(script) << ">();\n"
                   << "                            applyParameters(instance, component);\n"
                   << "                            target.addComponent(copy, Script{instance, "
                      "false});\n"
                   << "                        }));\n"
                   << "                    }\n";
            firstScript = false;
        }
        source << "                    else\n"
               << "                        throw std::runtime_error(\"Unknown script: \" + path);\n"
               << "                } else {\n";
    } else
        source << "                {\n";
    source << "                    components.addToPrefab(prefab, scene, component);\n";
    // Each instance of the prefab gets its own script
    writeButtonScripts(source, buttonScripts,
                       "prefab.addComponent(std::function<void(Scene &, Entity)>("
                       "[component](Scene &target, Entity instance) { "
                       "target.getComponent<UIButton>(instance).script = SCRIPT; }));");
    source << "                }\n"
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
    // Checked before writing or compiling anything
    const std::vector<std::string> missing = buildErrors(sceneObjects, projectFolder, descriptors);
    if (!missing.empty()) {
        std::string message;
        for (const std::string &line : missing)
            message += (message.empty() ? "" : "\n") + line;
        throw std::runtime_error(message);
    }
    const PluginTraits traits = inspectPlugins(sceneObjects.activePlugins);

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
    for (const std::string &script : collectButtonScripts(sceneObjects)) {
        const std::filesystem::path scriptFile = projectFolder / "scripts" / script;
        std::ifstream input(scriptFile);
        if (!input)
            throw std::runtime_error("Could not open button script: " + scriptFile.string());

        const std::string source((std::istreambuf_iterator<char>(input)),
                                 std::istreambuf_iterator<char>());
        if (source.find("onClick") == std::string::npos)
            throw std::runtime_error("Button script must define onClick: " + scriptFile.string());
    }

    const std::filesystem::path workingDirectory = std::filesystem::current_path();
    const std::filesystem::path engineDirectory = workingDirectory / "Gringo Engine";
    const std::filesystem::path resolvedEngineDirectory =
        std::filesystem::is_directory(engineDirectory) ? engineDirectory : workingDirectory;
    const std::filesystem::path buildDirectory = projectFolder / "build";
    // CMake list of the active plugins: "2D;Physics2D"
    std::string pluginList;
    for (const std::string &plugin : sceneObjects.activePlugins)
        pluginList += (pluginList.empty() ? "" : ";") + plugin;

    // No variant: one plain executable <target>. Otherwise one <target>_<variant> each
    std::vector<BuildVariant> variants = traits.variants;
    if (variants.empty())
        variants.push_back({"", false});
    for (const BuildVariant &variant : variants) {
        const std::string variantTarget =
            variant.name.empty() ? target : target + "_" + variant.name;
        const std::filesystem::path generatedSource =
            projectFolder / (variant.name.empty() ? "main.cpp" : variant.name + "_main.cpp");
        std::set<std::string> skippedPlugins;
        if (variant.headless)
            skippedPlugins = traits.needRenderer;
        std::set<std::string> ignoredComponents;
        for (const ComponentDescriptor &descriptor : descriptors)
            if (skippedPlugins.count(descriptor.plugin))
                ignoredComponents.insert(descriptor.name);
        writeGeneratedMain(generatedSource, projectFile, projectFolder, sceneObjects, variant,
                           skippedPlugins, ignoredComponents);

        const std::string renderer = variant.headless ? "" : sceneObjects.renderer;
        std::string command =
            "cmake -S " + shellQuote(resolvedEngineDirectory.generic_string()) + " -B " +
            shellQuote(buildDirectory.generic_string()) +
            " "
            "-DGRINGO_GAME_TARGET=" +
            shellQuote(variantTarget) +
            " -DGRINGO_GAME_OUTPUT_DIR=" + shellQuote(projectFolder.generic_string()) +
            " -DGRINGO_GAME_SOURCE=" + shellQuote(generatedSource.generic_string()) +
            " -DGRINGO_GAME_SCRIPT_DIR=" + shellQuote((projectFolder / "scripts").generic_string()) +
            " -DGRINGO_GAME_PLUGINS=" + shellQuote(pluginList) +
            " -DGRINGO_GAME_RENDERER=" + shellQuote(renderer) + " && cmake --build " +
            shellQuote(buildDirectory.generic_string()) + " --target " + shellQuote(variantTarget) +
            // Visual Studio builds Debug unless told otherwise; single-config generators ignore it
            " --config Release -j";
        if (std::system(command.c_str()) != 0)
            throw std::runtime_error("Failed to build project executable: " + variantTarget);
    }

    return target;
}
