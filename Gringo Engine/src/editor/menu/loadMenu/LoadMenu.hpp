/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** LoadMenu
*/

#ifndef LOADMENU_HPP_
#define LOADMENU_HPP_

#include "AMenu.hpp"
#include "Parser.hpp"

class LoadMenu : public AMenu
{
  public:
    LoadMenu(IEditorRenderer &renderer, Parser &parser);
    ~LoadMenu();
    SceneObjects getSceneObjects() const { return _sceneObjects; }

  protected:
    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;

  private:
    std::vector<std::string> _savedFiles;
    SceneObjects _sceneObjects;
    Parser &_parser;
};

#endif /* !LOADMENU_HPP_ */
