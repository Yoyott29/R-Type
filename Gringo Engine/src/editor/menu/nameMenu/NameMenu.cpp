/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NameMenu
*/

#include "NameMenu.hpp"

NameMenu::NameMenu(IEditorRenderer &renderer, const std::string &templateName,
                   const std::vector<std::string> &renderers)
    : AMenu(renderer), _template(templateName), _rendererChoices(renderers)
{
    // RaylibRenderer by default when it is there, otherwise the first one
    for (std::size_t index = 0; index < _rendererChoices.size(); ++index)
        if (_rendererChoices[index] == "RaylibRenderer")
            _rendererIndex = index;

    _currentMenu = MenuType::NameMenu;

    NameMenu::initTexts();
    NameMenu::initButtons();
    NameMenu::initRectangles();
}

void NameMenu::initTexts()
{
    addText("Title", {960, 200}, 60, "Choose a project name", TEXT_COLOR);
    addText("Input", {960, 400}, 36, "", TEXT_COLOR);
    // A game always draws through a renderer: without one, no project can be created
    if (_rendererChoices.empty())
        addText("RendererTitle", {960, 540}, 30,
                "No renderer in Gringo Engine/renderers: build the engine first", TEXT_COLOR);
    else
        addText("RendererTitle", {960, 540}, 30, "Renderer (click to change)", TEXT_COLOR);
}

void NameMenu::initButtons()
{
    addButton(
        "Back", {1800, 50}, 200, 50, 20, [this]() { _switchMenu = MenuType::TemplateMenu; }, BUTTON_COLOR1,
        BUTTON_COLOR3, TEXT_COLOR);
    if (!_rendererChoices.empty())
        addButton(
            "Renderer", {960, 610}, 500, 60, 28, [this]() { nextRenderer(); }, BUTTON_COLOR2,
            BUTTON_COLOR3, TEXT_COLOR, _rendererChoices[_rendererIndex]);
}

void NameMenu::nextRenderer()
{
    _rendererIndex = (_rendererIndex + 1) % _rendererChoices.size();
    setButtonText("Renderer", _rendererChoices[_rendererIndex]);
}

std::string NameMenu::getRenderer() const
{
    return _rendererChoices.empty() ? "" : _rendererChoices[_rendererIndex];
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

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_ENTER) && !_projectName.empty() &&
        !_rendererChoices.empty())
        _switchMenu = MenuType::EditorMenu;

    _texts.at("Input").text = _projectName;

    AMenu::update();
}
