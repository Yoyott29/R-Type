/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#ifndef EDITORMENU_HPP_
#define EDITORMENU_HPP_

#include "AMenu.hpp"
#include <map>
#include <string>
#include <vector>

struct Component {
    std::string name;
    std::map<std::string, std::string> properties;
};

struct Entity {
    int id;
    std::string name;
    std::vector<Component> components;
};

// the four panels that can be shown in the center of the editor
enum class EditorPanel {
    Settings,
    Plugins,
    Prefabs,
    Entities
};

class EditorMenu : public AMenu {
    public:
        EditorMenu(IRenderer& renderer, std::string templateName);
        ~EditorMenu() override = default;

        void update() override;

    protected:
        void initTexts() override;
        void initButtons() override;
        void initPrefabs();
        void initEntities();

        void updateButtonColors();

        // small drawing helpers (positions are top left, not centered)
        void drawBox(float left, float top, int width, int height, ColorRGBA color) const;
        void drawTextLeft(const std::string& text, float left, float centerY,
                          int fontSize, ColorRGBA color) const;
        void drawTextRight(const std::string& text, float right, float centerY,
                           int fontSize, ColorRGBA color) const;

        // layout
        void drawLayout() const;
        void drawSidebarList(const std::vector<Entity>& items, float sidebarLeft,
                             const std::string& itemName) const;

        // center panels
        void drawCenterPanel() const;
        void drawPanelTitle(const std::string& title, const std::string& subtitle) const;
        void drawSettingsPanel() const;
        void drawPluginsPanel() const;
        void drawCardsPanel(const std::vector<Entity>& items, bool showId) const;
        void drawCard(const Entity& entity, float left, float top, int height, bool showId) const;
        void drawRow(const std::string& label, const std::string& value, size_t index) const;
        void drawEmptyState(const std::string& title, const std::string& hint) const;

        std::string _template;
        std::vector<Entity> _entities;
        std::vector<Entity> _prefabs;
        std::vector<std::string> _plugins;

        EditorPanel _activePanel;
        std::map<std::string, EditorPanel> _panelByButtonName;
};

#endif /* !EDITORMENU_HPP_ */