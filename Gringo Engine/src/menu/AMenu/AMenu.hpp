/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** AMenu
*/

#ifndef AMENU_HPP_
#define AMENU_HPP_

#include <raylib.h>
#include <map>
#include <string>
#include <functional>
#include <IRenderer.hpp>

class AMenu {
    public:
        AMenu(IRenderer& renderer);
        virtual ~AMenu() = default;

        virtual void update();
        bool isRunning() const { return _isRunning; }
        const std::string& getSwitchMenu() const { return _switchMenu; }

    protected:
        virtual void initTexts() = 0;
        virtual void initButtons() = 0;

        void addText(const std::string& name, Position pos, int fontSize,
                     const std::string& content,
                     ColorRGBA color = ColorRGBA{255, 255, 255, 255});
        void addButton(const std::string& name, Position center, int width,
                       int height, int fontSize, std::function<void()> onClick);

        virtual void drawTexts() const;
        virtual void handleButtons();
        virtual void drawRectangles() const;

        std::map<std::string, Button> _buttons;
        std::map<std::string, Text> _texts;
        std::map<std::string, UiRectangle> _rectangles;
        bool _isRunning;
        IRenderer& _renderer;
        std::string _switchMenu;
};

#endif /* !AMENU_HPP_ */