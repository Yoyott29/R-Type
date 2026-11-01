/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NameMenu
*/

#include "NameMenu.hpp"

NameMenu::NameMenu(IRenderer &renderer, const std::string &templateName)
    : AMenu(renderer), _template(templateName)
{
    _currentMenu = MenuType::NameMenu;

    initTexts();
    initButtons();
    initRectangles();
}

void NameMenu::initTexts()
{
    addText("Title", {960, 200}, 60, "Choose a project name", TEXT_COLOR);
    addText("Input", {960, 400}, 36, "", TEXT_COLOR);
}

void NameMenu::initButtons()
{
    addButton(
        "Back", {1800, 50}, 200, 50, 20, [this]() { _switchMenu = MenuType::TemplateMenu; }, BUTTON_COLOR1,
        BUTTON_COLOR3, TEXT_COLOR);
}

void NameMenu::initRectangles()
{
    addRectangle("InputBox", {960, 400}, 900, 80, BUTTON_COLOR4);
}

void NameMenu::update()
{
    for (int c = _renderer.getNextCharacter(); c > 0; c = _renderer.getNextCharacter()) {
        if (c >= 32 && c <= 126 && _projectName.size() < 24)
            _projectName += static_cast<char>(c);
    }

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_BACKSPACE) && !_projectName.empty())
        _projectName.pop_back();

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_ENTER) && !_projectName.empty())
        _switchMenu = MenuType::EditorMenu;

    _texts.at("Input").text = _projectName;

    AMenu::update();
}
