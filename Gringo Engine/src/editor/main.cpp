/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** main
*/

#include "ComponentCatalog.hpp"
#include "EditorMenu.hpp"
#include "LoadMenu.hpp"
#include "MainMenu.hpp"
#include "NameMenu.hpp"
#include "PluginLoader.hpp"
#include "ProjectBuilder.hpp"
#include "RaylibRenderer.hpp"
#include "TemplateMenu.hpp"
#include <iostream>
#include <memory>

int main(void)
{
    ComponentCatalog componentCatalog;
    RaylibRenderer renderer;
    Parser parser;

    std::string templateName = "";
    std::string projectName = "";
    SceneObjects sceneObjects;

    renderer.initWindow(1920, 1080, "Gringo Engine");

    std::unique_ptr<AMenu> currentMenu = std::make_unique<MainMenu>(renderer);

    try {
        while (!renderer.shouldClose() && currentMenu->isRunning()) {
            renderer.startDrawing();
            renderer.clearScreen(ColorRGBA{0, 0, 0, 255});
            currentMenu->update();
            renderer.endDrawing();

            const MenuType next = currentMenu->getSwitchMenu();
            const MenuType current = currentMenu->getCurrentMenu();

            if (next == MenuType::TemplateMenu)
                currentMenu = std::make_unique<TemplateMenu>(renderer);
            else if (next == MenuType::NameMenu) {
                templateName = static_cast<TemplateMenu &>(*currentMenu).getTemplate();
                currentMenu = std::make_unique<NameMenu>(renderer, templateName,
                                                         PluginLoader("renderers").listNames());
            } else if (next == MenuType::EditorMenu) {
                if (current == MenuType::NameMenu) {
                    sceneObjects = SceneObjects{};
                    const NameMenu &nameMenu = static_cast<NameMenu &>(*currentMenu);
                    projectName = nameMenu.getProjectName();
                    sceneObjects.renderer = nameMenu.getRenderer();
                    // The 2D template draws its Player with a Sprite
                    if (templateName == "2D")
                        sceneObjects.activePlugins = {"2D"};
                } else if (current == MenuType::LoadMenu) {
                    sceneObjects = static_cast<LoadMenu &>(*currentMenu).getSceneObjects();
                    projectName = sceneObjects.projectName;
                }
                currentMenu =
                    std::make_unique<EditorMenu>(renderer, templateName, projectName, parser,
                                                 sceneObjects, componentCatalog.getDescriptors(),
                                                 componentCatalog.getPluginSettings());
            } else if (next == MenuType::MainMenu)
                currentMenu = std::make_unique<MainMenu>(renderer);
            else if (next == MenuType::LoadMenu)
                currentMenu = std::make_unique<LoadMenu>(renderer, parser);
            else if (next == MenuType::Build) {
                const EditorMenu &editorMenu = static_cast<EditorMenu &>(*currentMenu);
                const std::string projectPath = editorMenu.getBuiltProjectPath();
                renderer.close();
                ProjectBuilder::build(projectPath, editorMenu.getSceneObjects(),
                                      componentCatalog.getDescriptors());
                return 0;
            }
        }
    } catch (const std::exception &error) {
        std::cerr << "Editor error: " << error.what() << '\n';
        renderer.close();
        return 84;
    }
    return 0;
}
