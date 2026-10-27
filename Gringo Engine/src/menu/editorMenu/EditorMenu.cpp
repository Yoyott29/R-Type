/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

namespace {
    // window size (same as the one given in main.cpp)
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;

    // top bar and sidebars
    constexpr int topBarHeight = 64;
    constexpr int sidebarWidth = 300;
    constexpr int borderThickness = 2;
    constexpr int sidebarTop = topBarHeight + borderThickness;
    constexpr int panelHeight = screenHeight - sidebarTop;

    // list of items shown under the sidebar buttons
    constexpr int sidebarListTop = sidebarTop + 114;
    constexpr int sidebarRowHeight = 44;
    constexpr int sidebarRowGap = 8;

    // center panel
    constexpr int centerWidth = screenWidth - 2 * sidebarWidth;
    constexpr int centerPadding = 45;
    constexpr int contentLeft = sidebarWidth + centerPadding;
    constexpr int contentWidth = centerWidth - 2 * centerPadding;
    constexpr int contentTop = sidebarTop + 170;

    // settings rows
    constexpr int rowHeight = 60;
    constexpr int rowGap = 6;

    // entity and prefab cards
    constexpr int cardsPerRow = 3;
    constexpr int cardWidth = 390;
    constexpr int cardGap = 30;
    constexpr int cardPadding = 20;
    constexpr int cardHeaderHeight = 64;
    constexpr int componentTitleHeight = 36;
    constexpr int propertyHeight = 28;

    // colors
    const ColorRGBA topBarColor{18, 18, 24, 255};
    const ColorRGBA sidebarColor{26, 26, 34, 255};
    const ColorRGBA centerColor{20, 20, 26, 255};
    const ColorRGBA cardColor{34, 34, 46, 255};
    const ColorRGBA rowColor{30, 30, 40, 255};
    const ColorRGBA rowAlternateColor{36, 36, 48, 255};
    const ColorRGBA borderColor{48, 48, 64, 255};
    const ColorRGBA accentColor{92, 124, 255, 255};
    const ColorRGBA buttonColor{44, 44, 58, 255};
    const ColorRGBA hoverColor{62, 62, 86, 255};
    const ColorRGBA mainTextColor{235, 235, 245, 255};
    const ColorRGBA dimTextColor{140, 140, 160, 255};
    const ColorRGBA componentTitleColor{150, 175, 255, 255};

    // true if the mouse is above a rectangle (position is the center, like everywhere else)
    bool isMouseOver(const UiRectangle& rectangle)
    {
        // left and top corner of the rectangle
        float left = rectangle.position.x - rectangle.width / 2.0f;
        float top = rectangle.position.y - rectangle.height / 2.0f;

        // current mouse position
        float mouseX = static_cast<float>(GetMouseX());
        float mouseY = static_cast<float>(GetMouseY());

        // check the four sides
        return mouseX >= left && mouseX <= left + rectangle.width
            && mouseY >= top && mouseY <= top + rectangle.height;
    }

    // height of a card: header + every component title + every property line
    int getCardHeight(const Entity& entity)
    {
        // the header is always there
        int height = cardHeaderHeight;

        for (const Component& component : entity.components) {
            // one line for the component name
            height += componentTitleHeight;
            // one line for each property of the component
            height += static_cast<int>(component.properties.size()) * propertyHeight;
        }
        // space at the bottom of the card
        return height + cardPadding;
    }
}

EditorMenu::EditorMenu(IRenderer& renderer, std::string templateName)
    : AMenu(renderer), _template(templateName), _activePanel(EditorPanel::Entities)
{
    initTexts();
    initButtons();
    initPrefabs();
    initEntities();
}

void EditorMenu::initTexts()
{
    // engine name, centered above the left sidebar
    addText("Title", {sidebarWidth / 2.0f, topBarHeight / 2.0f}, 28, "GRINGO ENGINE");
    // current template, centered in the top bar
    addText("Template", {screenWidth / 2.0f, topBarHeight / 2.0f}, 22, "Template: " + _template);
}

void EditorMenu::initButtons()
{
    // vertical center of everything in the top bar
    const float topBarCenterY = topBarHeight / 2.0f;
    // vertical center of the buttons at the top of each sidebar
    const float sidebarButtonY = sidebarTop + 16 + 25;

    // top bar buttons
    addButton("Settings", {395, topBarCenterY}, 130, 40, 22,
        [this]() { _activePanel = EditorPanel::Settings; });
    addButton("Plugins", {541, topBarCenterY}, 130, 40, 22,
        [this]() { _activePanel = EditorPanel::Plugins; });
    addButton("Back", {screenWidth - 90, topBarCenterY}, 140, 40, 22,
        [this]() { _switchMenu = "TemplateMenu"; });

    // left sidebar button
    addButton("Prefabs", {sidebarWidth / 2.0f, sidebarButtonY}, 260, 50, 26,
        [this]() { _activePanel = EditorPanel::Prefabs; });
    // right sidebar button
    addButton("Entities", {screenWidth - sidebarWidth / 2.0f, sidebarButtonY}, 260, 50, 26,
        [this]() { _activePanel = EditorPanel::Entities; });

    // remember which button opens which panel (used to highlight the active one)
    _panelByButtonName["Settings"] = EditorPanel::Settings;
    _panelByButtonName["Plugins"] = EditorPanel::Plugins;
    _panelByButtonName["Prefabs"] = EditorPanel::Prefabs;
    _panelByButtonName["Entities"] = EditorPanel::Entities;
}

void EditorMenu::initPrefabs()
{
    // prefabs that every template has
    _prefabs.push_back(Entity{0, "Player", {
        Component{"Transform", {{"x", "0"}, {"y", "0"}}},
        Component{"Sprite", {{"texture", "player.png"}}},
        Component{"Health", {{"max", "100"}}}
    }});
    _prefabs.push_back(Entity{1, "Enemy", {
        Component{"Transform", {{"x", "0"}, {"y", "0"}}},
        Component{"Sprite", {{"texture", "enemy.png"}}},
        Component{"Health", {{"max", "50"}}}
    }});
    _prefabs.push_back(Entity{2, "Bullet", {
        Component{"Transform", {{"x", "0"}, {"y", "0"}}},
        Component{"Velocity", {{"speed", "600"}}}
    }});

    // one more prefab depending on the template
    if (_template == "Platformer 2D") {
        _prefabs.push_back(Entity{3, "Platform", {
            Component{"Transform", {{"x", "0"}, {"y", "0"}}},
            Component{"Collider", {{"width", "128"}, {"height", "32"}}}
        }});
    } else if (_template == "Endless Runner 2D") {
        _prefabs.push_back(Entity{3, "Obstacle", {
            Component{"Transform", {{"x", "0"}, {"y", "0"}}},
            Component{"Scroll", {{"speed", "300"}}}
        }});
    } else if (_template == "Multiplayer 2D") {
        _prefabs.push_back(Entity{3, "Remote Player", {
            Component{"Transform", {{"x", "0"}, {"y", "0"}}},
            Component{"Network", {{"owner", "remote"}}}
        }});
    }
}

void EditorMenu::initEntities()
{
    // every scene starts with a camera and one player
    _entities.push_back(Entity{0, "Main Camera", {
        Component{"Transform", {{"x", "0"}, {"y", "0"}}},
        Component{"Camera", {{"zoom", "1.0"}}}
    }});
    _entities.push_back(Entity{1, "Player", {
        Component{"Transform", {{"x", "320"}, {"y", "180"}}},
        Component{"Sprite", {{"texture", "player.png"}}},
        Component{"Health", {{"max", "100"}}}
    }});
}

void EditorMenu::updateButtonColors()
{
    for (auto& [name, button] : _buttons) {
        // is this button the one of the panel currently shown ?
        auto panelEntry = _panelByButtonName.find(name);
        bool isPanelButton = panelEntry != _panelByButtonName.end();
        bool isActive = isPanelButton && panelEntry->second == _activePanel;

        // is the mouse above this button ?
        bool isHovered = isMouseOver(button.rectangle);

        // active button wins over hover, hover wins over the default color
        if (isActive)
            button.rectangle.color = accentColor;
        else if (isHovered)
            button.rectangle.color = hoverColor;
        else
            button.rectangle.color = buttonColor;
    }
}

void EditorMenu::drawBox(float left, float top, int width, int height, ColorRGBA color) const
{
    // the renderer wants the center of the rectangle, not the top left corner
    Position center = {left + width / 2.0f, top + height / 2.0f};

    _renderer.drawRectangle(center, width, height, color);
}

void EditorMenu::drawTextLeft(const std::string& text, float left, float centerY,
                              int fontSize, ColorRGBA color) const
{
    // the renderer centers the text, so shift it by half of its width
    Position textSize = _renderer.getTextSize(text, fontSize);
    Position center = {left + textSize.x / 2.0f, centerY};

    _renderer.drawText(text, center, fontSize, color);
}

void EditorMenu::drawTextRight(const std::string& text, float right, float centerY,
                               int fontSize, ColorRGBA color) const
{
    // same as drawTextLeft but the text ends at "right"
    Position textSize = _renderer.getTextSize(text, fontSize);
    Position center = {right - textSize.x / 2.0f, centerY};

    _renderer.drawText(text, center, fontSize, color);
}

void EditorMenu::drawLayout() const
{
    // center background, drawn first so everything else is on top of it
    drawBox(sidebarWidth, sidebarTop, centerWidth, panelHeight, centerColor);

    // top bar with an accent line under it
    drawBox(0, 0, screenWidth, topBarHeight, topBarColor);
    drawBox(0, topBarHeight, screenWidth, borderThickness, accentColor);

    // left sidebar with a border on its right side
    drawBox(0, sidebarTop, sidebarWidth, panelHeight, sidebarColor);
    drawBox(sidebarWidth - borderThickness, sidebarTop, borderThickness, panelHeight, borderColor);

    // right sidebar with a border on its left side
    drawBox(screenWidth - sidebarWidth, sidebarTop, sidebarWidth, panelHeight, sidebarColor);
    drawBox(screenWidth - sidebarWidth, sidebarTop, borderThickness, panelHeight, borderColor);
}

void EditorMenu::drawSidebarList(const std::vector<Entity>& items, float sidebarLeft,
                                 const std::string& itemName) const
{
    // small counter under the sidebar button
    std::string counter = std::to_string(items.size()) + " " + itemName;
    drawTextLeft(counter, sidebarLeft + 20, sidebarTop + 92, 18, dimTextColor);

    // top of the first row
    float rowTop = sidebarListTop;

    for (const Entity& item : items) {
        // row background with a small accent stripe on its left
        drawBox(sidebarLeft + 20, rowTop, 260, sidebarRowHeight, cardColor);
        drawBox(sidebarLeft + 20, rowTop, 4, sidebarRowHeight, accentColor);

        // name of the item, vertically centered in the row
        drawTextLeft(item.name, sidebarLeft + 40, rowTop + sidebarRowHeight / 2.0f, 22, mainTextColor);

        // go to the next row
        rowTop += sidebarRowHeight + sidebarRowGap;
    }
}

void EditorMenu::drawCenterPanel() const
{
    // show the content of the panel selected by the user
    switch (_activePanel) {
        case EditorPanel::Settings:
            drawPanelTitle("Settings", "Information about the editor and the project");
            drawSettingsPanel();
            break;
        case EditorPanel::Plugins:
            drawPanelTitle("Plugins", "Add more features to the editor");
            drawPluginsPanel();
            break;
        case EditorPanel::Prefabs:
            drawPanelTitle("Prefabs", "Models you can use to create new entities");
            drawCardsPanel(_prefabs, false);
            break;
        case EditorPanel::Entities:
            drawPanelTitle("Entities", "Everything that is currently in your scene");
            drawCardsPanel(_entities, true);
            break;
    }
}

void EditorMenu::drawPanelTitle(const std::string& title, const std::string& subtitle) const
{
    // big title, then a smaller description under it
    drawTextLeft(title, contentLeft, sidebarTop + 60, 44, mainTextColor);
    drawTextLeft(subtitle, contentLeft, sidebarTop + 100, 22, dimTextColor);

    // short accent bar under the texts
    drawBox(contentLeft, sidebarTop + 126, 60, 4, accentColor);
}

void EditorMenu::drawRow(const std::string& label, const std::string& value, size_t index) const
{
    // rows are stacked, one color out of two to make them easier to read
    float rowTop = contentTop + index * (rowHeight + rowGap);
    ColorRGBA backgroundColor = (index % 2 == 0) ? rowColor : rowAlternateColor;
    float textCenterY = rowTop + rowHeight / 2.0f;

    // background, then the label on the left and the value on the right
    drawBox(contentLeft, rowTop, contentWidth, rowHeight, backgroundColor);
    drawTextLeft(label, contentLeft + 24, textCenterY, 24, mainTextColor);
    drawTextRight(value, contentLeft + contentWidth - 24, textCenterY, 24, dimTextColor);
}

void EditorMenu::drawSettingsPanel() const
{
    // values are read live from raylib so they are always correct
    const std::string resolution = std::to_string(GetScreenWidth()) + " x " + std::to_string(GetScreenHeight());
    const std::vector<std::pair<std::string, std::string>> settings = {
        {"Template", _template},
        {"Resolution", resolution},
        {"Current FPS", std::to_string(GetFPS())}
    };

    for (size_t index = 0; index < settings.size(); index++)
        drawRow(settings[index].first, settings[index].second, index);
}

void EditorMenu::drawPluginsPanel() const
{
    // nothing to list yet
    if (_plugins.empty()) {
        drawEmptyState("No plugins installed", "Installed plugins will show up here.");
        return;
    }

    for (size_t index = 0; index < _plugins.size(); index++)
        drawRow(_plugins[index], "", index);
}

void EditorMenu::drawEmptyState(const std::string& title, const std::string& hint) const
{
    // size of the box and its position (centered horizontally on the screen)
    const int boxWidth = 640;
    const int boxHeight = 200;
    const float boxLeft = (screenWidth - boxWidth) / 2.0f;
    const float boxTop = contentTop + 60;
    const float centerX = screenWidth / 2.0f;

    // box with an accent line on top
    drawBox(boxLeft, boxTop, boxWidth, boxHeight, cardColor);
    drawBox(boxLeft, boxTop, boxWidth, 4, accentColor);

    // title and hint, centered in the box
    _renderer.drawText(title, {centerX, boxTop + 80}, 30, mainTextColor);
    _renderer.drawText(hint, {centerX, boxTop + 125}, 20, dimTextColor);
}

void EditorMenu::drawCardsPanel(const std::vector<Entity>& items, bool showId) const
{
    // nothing to show
    if (items.empty()) {
        drawEmptyState("Nothing here yet", "Items will appear here once they are created.");
        return;
    }

    // top of the current row of cards
    float rowTop = contentTop;

    for (size_t rowStart = 0; rowStart < items.size(); rowStart += cardsPerRow) {
        // index after the last card of this row
        size_t rowEnd = std::min(rowStart + cardsPerRow, items.size());

        // every card of a row gets the height of the tallest one
        int tallestCard = 0;
        for (size_t index = rowStart; index < rowEnd; index++)
            tallestCard = std::max(tallestCard, getCardHeight(items[index]));

        // draw the cards of the row side by side
        for (size_t index = rowStart; index < rowEnd; index++) {
            float cardLeft = contentLeft + (index - rowStart) * (cardWidth + cardGap);
            drawCard(items[index], cardLeft, rowTop, tallestCard, showId);
        }

        // next row goes under this one
        rowTop += tallestCard + cardGap;
    }
}

void EditorMenu::drawCard(const Entity& entity, float left, float top, int height, bool showId) const
{
    // card background with an accent line on top
    drawBox(left, top, cardWidth, height, cardColor);
    drawBox(left, top, cardWidth, 4, accentColor);

    // name on the left, id on the right (only for entities)
    drawTextLeft(entity.name, left + cardPadding, top + 38, 26, mainTextColor);
    if (showId)
        drawTextRight("#" + std::to_string(entity.id), left + cardWidth - cardPadding, top + 38, 20, dimTextColor);

    // y position of the next line to draw
    float lineTop = top + cardHeaderHeight;

    for (const Component& component : entity.components) {
        // component name
        drawTextLeft(component.name, left + cardPadding, lineTop + componentTitleHeight / 2.0f, 22, componentTitleColor);
        lineTop += componentTitleHeight;

        for (const auto& [key, value] : component.properties) {
            // property name on the left, its value on the right
            float lineCenterY = lineTop + propertyHeight / 2.0f;
            drawTextLeft(key, left + cardPadding + 10, lineCenterY, 18, dimTextColor);
            drawTextRight(value, left + cardWidth - cardPadding, lineCenterY, 18, mainTextColor);
            lineTop += propertyHeight;
        }
    }
}

void EditorMenu::update()
{
    // backgrounds first, then everything that goes on top of them
    drawLayout();
    drawSidebarList(_prefabs, 0, "prefabs");
    drawSidebarList(_entities, screenWidth - sidebarWidth, "entities");
    drawCenterPanel();

    // set the button colors, then let AMenu draw the texts and handle the clicks
    updateButtonColors();
    AMenu::update();
}