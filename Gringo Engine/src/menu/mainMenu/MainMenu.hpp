/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MainMenu
*/

#ifndef MAINMENU_HPP_
#define MAINMENU_HPP_

#include "AMenu.hpp"

class MainMenu : public AMenu {
    public:
        MainMenu(IRenderer& renderer);
        ~MainMenu() override = default;

    protected:
        void initTexts() override;
        void initButtons() override;
};

#endif /* !MAINMENU_HPP_ */
