/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AMenu
*/

#include "AMenu.hpp"

AMenu::AMenu(IRenderer& renderer) : _isRunning(true), _renderer(renderer)
{
}

void AMenu::addText(const std::string& name, Position pos, int fontSize,
                    const std::string& content, ColorRGBA color)
{
    _texts.emplace(name, Text{pos, fontSize, content, color});
}

void AMenu::addButton(const std::string& name, Position center, int width,
                      int height, int fontSize, std::function<void()> onClick)
{
    _buttons.emplace(name, Button{UiRectangle{center, width, height, ColorRGBA{100, 100, 100, 255}},
        Text{center, fontSize, name, ColorRGBA{255, 255, 255, 255}}, onClick});
}

void AMenu::drawTexts() const
{
    for (const auto& [name, text] : _texts)
        _renderer.drawText(text.text, text.position, text.fontSize,
                           ColorRGBA{255, 255, 255, 255});
}

void AMenu::handleButtons()
{
    for (const auto& [name, button] : _buttons) {
        float left = button.rectangle.position.x - button.rectangle.width / 2.0f;
        float top = button.rectangle.position.y - button.rectangle.height / 2.0f;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            GetMouseX() >= left && GetMouseX() <= left + button.rectangle.width &&
            GetMouseY() >= top && GetMouseY() <= top + button.rectangle.height &&
            button.onClick)
            button.onClick();

        _renderer.drawRectangle(button.rectangle.position, button.rectangle.width, button.rectangle.height,
                                button.rectangle.color);
        _renderer.drawText(button.text.text, button.text.position,
                           button.text.fontSize, ColorRGBA{255, 255, 255, 255});
    }
}

void AMenu::drawRectangles() const
{
    for (const auto& [name, rectangle] : _rectangles) {
        _renderer.drawRectangle(rectangle.position, rectangle.width, rectangle.height,
                                rectangle.color);
    }
}


void AMenu::update()
{
    drawTexts();
    handleButtons();
}