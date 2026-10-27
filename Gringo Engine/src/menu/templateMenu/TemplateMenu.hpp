/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TemplateMenu
*/

#ifndef TemplateMENU_HPP_
#define TemplateMENU_HPP_

#include "AMenu.hpp"

class TemplateMenu : public AMenu {
    public:
        TemplateMenu(IRenderer& renderer);
        ~TemplateMenu() override = default;

        std::string getTemplate() const { return _template; }
        void setTemplate(const std::string& templateName) { _template = templateName; }

    protected:
        void initTexts() override;
        void initButtons() override;

        std::string _template;
};

#endif /* !TemplateMENU_HPP_ */
