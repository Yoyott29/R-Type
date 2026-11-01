#include "ProjectBuilder.hpp"
#include "Parser.hpp"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>

namespace {

std::string shellQuote(const std::string &value)
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

std::string targetName(const std::string &projectName)
{
    std::string result = "game_";
    for (char character : projectName)
        result += std::isalnum(static_cast<unsigned char>(character)) ? character : '_';
    return result;
}

std::string scriptPath(const Component &component)
{
    for (const Property &property : component.properties)
        if (property.name == "Path")
            return property.value;
    return {};
}

std::string className(const std::string &path)
{
    return std::filesystem::path(path).stem().string();
}

std::set<std::string> collectScripts(const SceneObjects &sceneObjects)
{
    std::set<std::string> scripts;
    for (const Entity &entity : sceneObjects._entities) {
        for (const Component &component : entity.components) {
            if (component.name == "Script" && !scriptPath(component).empty())
                scripts.insert(scriptPath(component));
        }
    }
    return scripts;
}

void writeGeneratedMain(const std::filesystem::path &sourcePath,
                        const SceneObjects &sceneObjects)
{
    const std::set<std::string> scripts = collectScripts(sceneObjects);
    std::ofstream source(sourcePath);
    if (!source)
        throw std::runtime_error("Could not create generated source: " + sourcePath.string());

    source << "#include \"Application.hpp\"\n"
           << "#include \"ProjectComponentManager.hpp\"\n"
           << "#include \"ScriptSystem.hpp\"\n"
           << "#include \"nlohmann/json.hpp\"\n"
           << "#include <filesystem>\n"
           << "#include <fstream>\n"
           << "#include <iostream>\n"
           << "#include <memory>\n"
           << "#include <stdexcept>\n"
           << "#include <string>\n";
    for (const std::string &script : scripts)
        source << "#include " << '"' << script << '"' << "\n";

    source << "\nnamespace {\n"
           << "std::shared_ptr<GringoEntityScript> createScript(const std::string &path)\n{\n";
    for (const std::string &script : scripts)
        source << "    if (path == " << '"' << script << '"' << ")\n"
               << "        return std::make_shared<" << className(script) << ">();\n";
    source << "    throw std::runtime_error(\"Unknown script: \" + path);\n"
           << "}\n\n"
           << "std::string scriptPath(const nlohmann::json &component)\n{\n"
           << "    for (const auto &property : component.value(\"properties\", nlohmann::json::array()))\n"
           << "        if (property.value(\"name\", \"\") == \"Path\")\n"
           << "            return property.value(\"value\", \"\");\n"
           << "    return {};\n"
           << "}\n\n"
           << "void loadProject(Application &application, const std::string &path)\n{\n"
           << "    std::ifstream file(path);\n"
           << "    if (!file)\n"
           << "        throw std::runtime_error(\"Could not open project file: \" + path);\n"
           << "    nlohmann::json root;\n"
           << "    file >> root;\n"
           << "    Scene &scene = application._scene;\n"
           << "    ProjectComponentManager components(std::filesystem::path(path).parent_path());\n"
           << "    for (const auto &entityData : root.value(\"entities\", nlohmann::json::array())) {\n"
           << "        Entity entity = scene.createEntity();\n"
           << "        for (const auto &component : entityData.value(\"components\", nlohmann::json::array())) {\n"
           << "            if (component.value(\"name\", \"\") == \"Script\")\n"
           << "                scene.addComponent(entity, Script{createScript(scriptPath(component)), false});\n"
           << "            else\n"
           << "                components.addToScene(scene, entity, component);\n"
           << "        }\n"
           << "    }\n"
           << "}\n"
           << "}\n\n"
           << "int main(int argc, char **argv)\n{\n"
           << "    if (argc != 2)\n"
           << "        return 84;\n"
           << "    try {\n"
           << "        Application application;\n"
           << "        application.addPlugin(\"RayLibRenderer\");\n"
           << "        application.addPlugin(\"Physics\");\n"
           << "        application._scene.registerSystem<ScriptSystem>(FixedUpdate);\n"
           << "        ComponentSet requirements;\n"
           << "        requirements.set(application._scene.getComponentType<Script>());\n"
           << "        application._scene.setSystemComponentSet<ScriptSystem>(requirements);\n"
           << "        loadProject(application, argv[1]);\n"
           << "        application.run();\n"
           << "    } catch (const std::exception &error) {\n"
           << "        std::cerr << \"Runtime error: \" << error.what() << '\\\\n';\n"
           << "        return 84;\n"
           << "    }\n"
           << "    return 0;\n"
           << "}\n";
}

} // namespace

std::string ProjectBuilder::build(const std::string &projectPath,
                                  const SceneObjects &sceneObjects)
{
    const std::filesystem::path projectFile(projectPath);
    const std::filesystem::path projectFolder = projectFile.parent_path();
    const std::filesystem::path generatedFolder = projectFolder / "generated";
    const std::filesystem::path generatedSource = generatedFolder / "main.cpp";
    const std::string target = targetName(sceneObjects.projectName);

    std::filesystem::create_directories(generatedFolder);
    for (const std::string &script : collectScripts(sceneObjects)) {
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
    writeGeneratedMain(generatedSource, sceneObjects);

    const std::string command =
        "cmake -S . -B build -DBUILD_GRINGO=ON -DBUILD_RTYPE=ON "
        "-DGRINGO_GAME_TARGET=" + shellQuote(target) +
        " -DGRINGO_GAME_SOURCE=" + shellQuote(generatedSource.string()) +
        " -DGRINGO_GAME_SCRIPT_DIR=" + shellQuote((projectFolder / "scripts").string()) +
        " && cmake --build build --target " + shellQuote(target);
    if (std::system(command.c_str()) != 0)
        throw std::runtime_error("Failed to build project executable: " + target);

    return target;
}
