/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TemplateMenu
*/

#include "TemplateMenu.hpp"

TemplateMenu::TemplateMenu(IEditorRenderer &renderer) : AMenu(renderer), _template("default")
{
    _currentMenu = MenuType::TemplateMenu;
    TemplateMenu::initTexts();
    TemplateMenu::initButtons();
}

void TemplateMenu::initTexts()
{
    addText("Template", {960, 200}, 60, "Choose your Template", TEXT_COLOR);
}

void TemplateMenu::initButtons()
{
    addButton(
        "2D", {450, 400}, 300, 100, 30,
        [this]() {
            _switchMenu = MenuType::NameMenu;
            setTemplate("2D");
        },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Back", {1800, 50}, 200, 50, 20, [this]() { _switchMenu = MenuType::MainMenu; },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
}

void TemplateMenu::initRectangles() {}
