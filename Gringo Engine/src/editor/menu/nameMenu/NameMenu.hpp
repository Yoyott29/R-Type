/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NameMenu
*/

#ifndef NAMEMENU_HPP_
#define NAMEMENU_HPP_

#include "AMenu.hpp"
#include <cstddef>
#include <string>
#include <vector>

class NameMenu : public AMenu
{
  public:
    // renderers: the names in Gringo Engine/renderers, the user picks one. With none, the
    // project cannot be created
    NameMenu(IEditorRenderer &renderer, const std::string &templateName,
             const std::vector<std::string> &renderers);
    ~NameMenu() override = default;

    void update() override;

    const std::string &getTemplate() const { return _template; }
    const std::string &getProjectName() const { return _projectName; }
    // The chosen renderer
    std::string getRenderer() const;

  protected:
    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;

  private:
    void nextRenderer();

    std::string _template;
    std::string _projectName;
    // Available renderers
    std::vector<std::string> _rendererChoices;
    std::size_t _rendererIndex = 0;
};

#endif /* !NAMEMENU_HPP_ */
