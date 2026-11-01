/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NameMenu
*/

#ifndef NAMEMENU_HPP_
#define NAMEMENU_HPP_

#include "AMenu.hpp"

class NameMenu : public AMenu
{
  public:
    NameMenu(IRenderer &renderer, const std::string &templateName);
    ~NameMenu() override = default;

    void update() override;

    const std::string &getTemplate() const { return _template; }
    const std::string &getProjectName() const { return _projectName; }

  protected:
    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;

  private:
    std::string _template;
    std::string _projectName;
};

#endif /* !NAMEMENU_HPP_ */
