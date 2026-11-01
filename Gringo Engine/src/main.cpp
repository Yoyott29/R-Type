/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** main
*/

#include <memory>
#include "MainMenu.hpp"
#include "TemplateMenu.hpp"
#include "LoadMenu.hpp"
#include "EditorMenu.hpp"
#include "NameMenu.hpp"
#include "RaylibRenderer.hpp"

int main(void)
{
    RaylibRenderer renderer;
    Parser parser;

    std::string templateName = "";
    std::string projectName = "";
    SceneObjects sceneObjects;

    renderer.initWindow(1920, 1080, "Gringo Engine");

    std::unique_ptr<AMenu> currentMenu = std::make_unique<MainMenu>(renderer);

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
            templateName = static_cast<TemplateMenu*>(currentMenu.get())->getTemplate();
            currentMenu = std::make_unique<NameMenu>(renderer, templateName);
        }
        else if (next == MenuType::EditorMenu) {
            if (current == MenuType::NameMenu) {
                sceneObjects = SceneObjects{};
                projectName = static_cast<NameMenu*>(currentMenu.get())->getProjectName();
            }
            else if (current == MenuType::LoadMenu) {
                sceneObjects = static_cast<LoadMenu*>(currentMenu.get())->getSceneObjects();
                projectName = sceneObjects.projectName;
            }
            currentMenu = std::make_unique<EditorMenu>(renderer, templateName, projectName, parser, sceneObjects);
        }
        else if (next == MenuType::MainMenu)
            currentMenu = std::make_unique<MainMenu>(renderer);
        else if (next == MenuType::LoadMenu)
            currentMenu = std::make_unique<LoadMenu>(renderer, parser);
    }
    return 0;
}