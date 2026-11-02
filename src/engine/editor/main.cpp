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
                templateName = static_cast<TemplateMenu *>(currentMenu.get())->getTemplate();
                currentMenu = std::make_unique<NameMenu>(renderer, templateName);
            } else if (next == MenuType::EditorMenu) {
                if (current == MenuType::NameMenu) {
                    sceneObjects = SceneObjects{};
                    projectName = static_cast<NameMenu *>(currentMenu.get())->getProjectName();
                } else if (current == MenuType::LoadMenu) {
                    sceneObjects = static_cast<LoadMenu *>(currentMenu.get())->getSceneObjects();
                    projectName = sceneObjects.projectName;
                }
                currentMenu =
                    std::make_unique<EditorMenu>(renderer, templateName, projectName, parser,
                                                 sceneObjects, componentCatalog.getDescriptors());
            } else if (next == MenuType::MainMenu)
                currentMenu = std::make_unique<MainMenu>(renderer);
            else if (next == MenuType::LoadMenu)
                currentMenu = std::make_unique<LoadMenu>(renderer, parser);
            else if (next == MenuType::Build) {
                const auto *editorMenu = static_cast<EditorMenu *>(currentMenu.get());
                const std::string projectPath = editorMenu->getBuiltProjectPath();
                ProjectBuilder::build(projectPath, editorMenu->getSceneObjects());
                renderer.close();
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
