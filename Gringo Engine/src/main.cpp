/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** main
*/

#include <memory>
#include "MainMenu.hpp"
#include "TemplateMenu.hpp"
#include "EditorMenu.hpp"
#include "RaylibRenderer.hpp"

int main(void)
{
    RaylibRenderer renderer;
    renderer.initWindow(1920, 1080, "Gringo Engine");

    std::unique_ptr<AMenu> currentMenu = std::make_unique<MainMenu>(renderer);

    while (!renderer.shouldClose() && currentMenu->isRunning()) {
        renderer.startDrawing();
        renderer.clearScreen(ColorRGBA{0, 0, 0, 255});
        currentMenu->update();
        renderer.endDrawing();

        const std::string next = currentMenu->getSwitchMenu();
        
        if (next == "TemplateMenu")
            currentMenu = std::make_unique<TemplateMenu>(renderer);
        else if (next == "MainMenu")
            currentMenu = std::make_unique<MainMenu>(renderer);
        else if (next == "EditorMenu") {
            std::string templateName = static_cast<TemplateMenu*>(currentMenu.get())->getTemplate();
            currentMenu = std::make_unique<EditorMenu>(renderer, templateName);
        }
    }
    return 0;
}