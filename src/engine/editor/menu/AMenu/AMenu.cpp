/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AMenu
*/

#include "AMenu.hpp"
#include <algorithm>
#include <vector>

AMenu::AMenu(IRenderer &renderer) : _isRunning(true), _renderer(renderer) {}

void AMenu::addText(const std::string &name, Position pos, int fontSize, const std::string &text,
                    ColorRGBA color)
{
    _texts.emplace(name, Text{pos, fontSize, text, color, true});
}

void AMenu::addButton(const std::string &name, Position center, int width, int height, int fontSize,
                      std::function<void()> onClick, ColorRGBA buttonColor, ColorRGBA hoveredColor,
                      ColorRGBA textColor, std::string text)
{
    if (text.empty())
        text = name;
    _buttons.emplace(name, Button{UiRectangle{center, width, height, buttonColor}, hoveredColor,
                                  false, Text{center, fontSize, text, textColor, true}, onClick});
}

void AMenu::addRectangle(const std::string &name, Position center, int width, int height,
                         ColorRGBA color, int drawOrder)
{
    _rectangles.emplace(name, UiRectangle{center, width, height, color, drawOrder});
}

void AMenu::drawTexts() const
{
    for (const auto &[name, text] : _texts)
        _renderer.drawText(text.text, text.position, text.fontSize, text.color);
}

void AMenu::handleButtons()
{
    std::vector<std::string> clickedButtons;
    Position mousePosition = _renderer.getMousePosition();
    bool mousePressed = _renderer.isMouseButtonPressed(MouseButtons::LEFT);

    for (auto &[name, button] : _buttons) {
        float left = button.rectangle.position.x - button.rectangle.width / 2.0f;
        float top = button.rectangle.position.y - button.rectangle.height / 2.0f;

        bool mouseOver = mousePosition.x >= left &&
                         mousePosition.x <= left + button.rectangle.width &&
                         mousePosition.y >= top &&
                         mousePosition.y <= top + button.rectangle.height;

        bool clicked = mouseOver && mousePressed;

        if (clicked)
            _pressedButtonName = name;

        button.ispressed = (name == _pressedButtonName);

        if (clicked && button.onClick)
            clickedButtons.push_back(name);
        
        ColorRGBA buttonColor = checkHoveredButton(button);
        _renderer.drawRectangle(button.rectangle.position, button.rectangle.width,
                                button.rectangle.height, buttonColor);
        _renderer.drawText(button.text.text, button.text.position, button.text.fontSize,
                           button.text.color);
    }

    for (const std::string &buttonName : clickedButtons) {
        auto buttonIterator = _buttons.find(buttonName);
        if (buttonIterator != _buttons.end() && buttonIterator->second.onClick)
            buttonIterator->second.onClick();
    }
}

void AMenu::unSelectButton(Button &button)
{
    button.ispressed = false;
    if (_pressedButtonName.empty())
        return;

    for (const auto &[name, currentButton] : _buttons) {
        if (&currentButton == &button) {
            _pressedButtonName.clear();
            return;
        }
    }
}

void AMenu::drawRectangles() const
{
    // References to the rectangles, so nothing is copied
    std::vector<std::reference_wrapper<const UiRectangle>> sortedRectangles;
    sortedRectangles.reserve(_rectangles.size());

    for (const auto &[name, rectangle] : _rectangles)
        sortedRectangles.push_back(std::cref(rectangle));

    // Lowest draw order first
    std::sort(sortedRectangles.begin(), sortedRectangles.end(),
              [](const UiRectangle &left, const UiRectangle &right) {
                  return left.drawOrder < right.drawOrder;
              });

    for (const UiRectangle &rectangle : sortedRectangles)
        _renderer.drawRectangle(rectangle.position, rectangle.width, rectangle.height,
                                rectangle.color);
}

void AMenu::setButtonText(const std::string &buttonName, const std::string &newText)
{
    auto buttonIterator = _buttons.find(buttonName);

    if (buttonIterator == _buttons.end())
        return;

    buttonIterator->second.text.text = newText;
}


bool AMenu::isMouseOver(const UiRectangle &rectangle)
{
    float left = rectangle.position.x - rectangle.width / 2.0f;
    float top = rectangle.position.y - rectangle.height / 2.0f;

    Position mousePosition = _renderer.getMousePosition();
    float mouseX = mousePosition.x;
    float mouseY = mousePosition.y;

    return mouseX >= left && mouseX <= left + rectangle.width && mouseY >= top &&
           mouseY <= top + rectangle.height;
}

ColorRGBA AMenu::checkHoveredButton(Button &button)
{
    if (isMouseOver(button.rectangle) || button.ispressed)
        return button.hoveredColor;
    return button.rectangle.color;
}


void AMenu::update()
{
    drawRectangles();
    drawTexts();
    handleButtons();
}
