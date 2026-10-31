/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AMenu
*/

#ifndef AMENU_HPP_
#define AMENU_HPP_

#include <IRenderer.hpp>
#include <functional>
#include <map>
#include <string>

#define TEXT_COLOR ColorRGBA{170, 173, 182, 255}
#define BUTTON_COLOR1 ColorRGBA{52, 55, 66, 255}
#define BUTTON_COLOR2 ColorRGBA{42, 45, 54, 255}
#define BUTTON_COLOR3 ColorRGBA{32, 34, 42, 255}
#define BUTTON_COLOR4 ColorRGBA{22, 24, 29, 255}
#define BUTTON_COLOR5 ColorRGBA{12, 13, 17, 255}

class AMenu
{
  public:
    AMenu(IRenderer &renderer);
    virtual ~AMenu() = default;

    virtual void update();
    bool isRunning() const { return _isRunning; }
    const std::string &getSwitchMenu() const { return _switchMenu; }

  protected:
    virtual void initTexts() = 0;
    virtual void initButtons() = 0;
    virtual void initRectangles() = 0;

    void addText(const std::string &name, Position pos, int fontSize, const std::string &text,
                 ColorRGBA color = ColorRGBA{255, 255, 255, 255});
    void addButton(const std::string &name, Position center, int width, int height, int fontSize,
                   std::function<void()> onClick, ColorRGBA buttonColor, ColorRGBA hoveredColor, ColorRGBA textColor, std::string text = "");
    void addRectangle(const std::string &name, Position center, int width, int height,
                      ColorRGBA color, int drawOrder = 0);

    void removeText(const std::string &name) { _texts.erase(name); }
    void removeButton(const std::string &name) { _buttons.erase(name); }
    void removeRectangle(const std::string &name) { _rectangles.erase(name); }

    void setButtonText(const std::string &Name, const std::string &newText);
    void unSelectButton(Button &button);

    bool isMouseOver(const UiRectangle &rectangle);
    ColorRGBA checkHoveredButton(Button &button);

    virtual void drawTexts() const;
    virtual void handleButtons();
    virtual void drawRectangles() const;
    
    std::map<std::string, Button> _buttons;
    std::map<std::string, Text> _texts;
    std::map<std::string, UiRectangle> _rectangles;
    bool _isRunning;
    IRenderer &_renderer;
    std::string _switchMenu;

    std::string _pressedButtonName;
};

#endif /* !AMENU_HPP_ */
