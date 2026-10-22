/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Menu
*/

#ifndef MENU_HPP_
#define MENU_HPP_

#include <raylib.h>
#include <map>
#include <iostream>
#include <IRenderer.hpp>

class Menu {
    public:
        Menu(IRenderer& renderer);
        ~Menu();
        void update();
        bool isRunning() const { return _isRunning; }
        
    private:
        void initButtons();
        void initTexts();

    std::map<std::string, Button> _buttons;
    std::map<std::string, Text> _texts;
    bool _isRunning;
    IRenderer& _renderer;
};

#endif /* !MENU_HPP_ */
