/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Menu
*/

#include "Menu.hpp"

Menu::Menu(IRenderer& renderer) : _isRunning(true), _renderer(renderer)
{
    initTexts();
    initButtons();
}

Menu::~Menu()
{
}

void Menu::initTexts()
{
    _texts.emplace("title", Text{Position{960, 300}, 60, "GRINGO ENGINE"});
    _texts.emplace("subtitle", Text{Position{960, 350}, 30, "Welcome to the Gringo Engine!"});
}

void Menu::initButtons()
{
    auto createButton = [&](const std::string& name, Position center, int width, int height,
                            int fontSize, std::function<void()> function) {
        _buttons.emplace(name, Button{center, width, height,
            Text{center, fontSize, name}, function});
    };

    createButton("New Game", {660, 540},  300, 100, 30, nullptr);
    createButton("Load Game", {1260, 540}, 300, 100, 30, nullptr);
    createButton("Exit", {1800, 50},  200, 50,  20, [&]() { _isRunning = false; });
}



void Menu::update()
{
    for (const auto& text : _texts)
        _renderer.drawText(text.second.text, text.second.position, text.second.fontSize, ColorRGBA{255, 255, 255, 255});

    for (const auto& [name, button] : _buttons) {
        float left = button.position.x - button.width  / 2.0f;
        float top = button.position.y - button.height / 2.0f;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            GetMouseX() >= left && GetMouseX() <= left + button.width &&
            GetMouseY() >= top  && GetMouseY() <= top  + button.height &&
            button.onClick)
            button.onClick();

        _renderer.drawRectangle(button.position, button.width, button.height, ColorRGBA{100, 100, 100, 255});
        _renderer.drawText(button.text.text, button.text.position, button.text.fontSize, ColorRGBA{255, 255, 255, 255});
    }
}
