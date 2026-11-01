/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

EditorMenu::EditorMenu(IRenderer &renderer, std::string templateName, std::string projectName,
                       Parser &parser, SceneObjects sceneObjects,
                       const std::vector<ComponentDescriptor> &componentDescriptors)
    : AMenu(renderer), _template(templateName), _projectName(projectName), _parser(parser),
      _componentDescriptors(componentDescriptors), _objects(*this), _components(*this),
      _properties(*this)
{
    _currentMenu = MenuType::EditorMenu;

    if (_projectName.empty())
        _projectName = sceneObjects.projectName;

    initComponentTemplates();
    initPrefabs(sceneObjects);
    initEntities(sceneObjects);

    initTexts();
    initButtons();
    initRectangles();
}

EditorMenu::~EditorMenu()
{
    try {
        _properties.applyCurrentEdit();
        _parser.saveFile(_projectName + ".gringo", SceneObjects{_projectName, _entities, _prefabs});
    } catch (...) {
    }
}

void EditorMenu::update()
{
    drawRectangles();
    drawTexts();
    handleButtons();
    _properties.manageInputs();
    _properties.drawTextCursor();

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_ENTER) && _currentlyUpdatedProperty.active) {
        std::string editedButton = _currentlyUpdatedProperty.name;
        std::string editButton = _currentlyUpdatedProperty.editingName ? "ObjectHeader_Rename" : "";

        _properties.applyCurrentEdit();

        for (std::string name : {editedButton, editButton}) {
            auto it = _buttons.find(name);
            if (it != _buttons.end())
                unSelectButton(it->second);
        }
    }

    if (_savedMessageFrames > 0) {
        _savedMessageFrames -= 1;
        if (_savedMessageFrames == 0)
            removeText("Saved");
    }
    _objects.updateMoveHighlight();
}

void EditorMenu::saveProject()
{
    _properties.applyCurrentEdit();
    _parser.saveFile(_projectName + ".gringo", SceneObjects{_projectName, _entities, _prefabs});

    const std::string message = "File saved...";
    const Button &saveButton = _buttons.at("Save Project");
    float textWidth = _renderer.getTextSize(message, 20).x;
    float x = saveButton.rectangle.position.x - saveButton.rectangle.width / 2.0f - 20.0f -
              textWidth / 2.0f;

    removeText("Saved");
    unSelectButton(_buttons.at("Save Project"));
    addText("Saved", {x, saveButton.rectangle.position.y}, 20, message, TEXT_COLOR);
    _savedMessageFrames = SAVED_MESSAGE_FRAMES;
}

void EditorMenu::showPanel(const std::string &text)
{
    _properties.applyCurrentEdit();
    _components.clearDisplayedComponents();
    removeText("PanelText");

    if (text == "Settings") {
        addText("PanelText", {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f}, 40,
                "Settings panel is not implemented yet.", TEXT_COLOR);
    } else if (text == "Plugins") {
        addText("PanelText", {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f}, 40,
                "Plugins panel is not implemented yet.", TEXT_COLOR);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------
// INITIALIZATION
//--------------------------------------------------------------------------------------------------------------------------------

void EditorMenu::initTexts()
{
    addText("Title", {WINDOW_WIDTH / 2.0f, TOP_BAR_HEIGHT / 2.0f}, 30, "GRINGO ENGINE", TEXT_COLOR);
    addText("Entities", {200, TOP_BAR_HEIGHT + 32}, 40, "Entities", TEXT_COLOR);
    addText("Prefabs", {WINDOW_WIDTH - 200, TOP_BAR_HEIGHT + 32}, 40, "Prefabs", TEXT_COLOR);
    addText("Entity Page", {200, WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20}, 20, "1", TEXT_COLOR);
    addText("Prefab Page", {WINDOW_WIDTH - 200, WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20}, 20, "1",
            TEXT_COLOR);
    addText("Project Name", {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - TOP_BAR_HEIGHT / 2.0f}, 20,
            _projectName, TEXT_COLOR);
}

void EditorMenu::initButtons()
{
    float topBarCenterY = TOP_BAR_HEIGHT / 2.0f;
    float sideMargin = 20.0f;
    float buttonSpacing = 16.0f;
    float buttonWidth = 130.0f;
    float buttonHeight = 40.0f;
    float backButtonWidth = 140.0f;
    float settingsCenterX = sideMargin + buttonWidth / 2.0f;
    float pluginsCenterX = settingsCenterX + buttonWidth + buttonSpacing;
    float backCenterX = WINDOW_WIDTH - sideMargin - backButtonWidth / 2.0f;

    // top bar buttons
    addButton(
        "Settings", {settingsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22,
        [this]() { showPanel("Settings"); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Plugins", {pluginsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22,
        [this]() { showPanel("Plugins"); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    addButton(
        "Back", {backCenterX, topBarCenterY}, backButtonWidth, buttonHeight, 22,
        [this]() {
            _template.empty() ? _switchMenu = MenuType::LoadMenu
                              : _switchMenu = MenuType::TemplateMenu;
            saveProject();
        },
        BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    addButton("Build", {backCenterX - backButtonWidth - buttonSpacing, topBarCenterY},
              backButtonWidth, buttonHeight, 22, nullptr, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    float navY = WINDOW_HEIGHT - TOP_BAR_HEIGHT - buttonHeight / 2.0f;

    addButton(
        "Previous Entity Page", {130, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
        [this]() { _objects.changePage(ObjectType::Entity, -1); }, BUTTON_COLOR4, BUTTON_COLOR5,
        TEXT_COLOR, "<");
    addButton(
        "Next Entity Page", {270, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
        [this]() { _objects.changePage(ObjectType::Entity, 1); }, BUTTON_COLOR4, BUTTON_COLOR5,
        TEXT_COLOR, ">");

    addButton(
        "Previous Prefab Page", {WINDOW_WIDTH - 270.0f, navY}, SIDEBAR_WIDTH / 6,
        buttonHeight / 2.0f, 18, [this]() { _objects.changePage(ObjectType::Prefab, -1); },
        BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "<");
    addButton(
        "Next Prefab Page", {WINDOW_WIDTH - 130.0f, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f,
        18, [this]() { _objects.changePage(ObjectType::Prefab, 1); }, BUTTON_COLOR4, BUTTON_COLOR5,
        TEXT_COLOR, ">");
    addButton(
        "Save Project", {backCenterX, WINDOW_HEIGHT - TOP_BAR_HEIGHT / 2.0f}, 150, 40, 20,
        [this]() { saveProject(); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    _objects.refreshObjectList(ObjectType::Entity);
    _objects.refreshObjectList(ObjectType::Prefab);
}

void EditorMenu::initRectangles()
{
    float bottomBarHeight = BOTTOM_BAR_HEIGHT;
    float sidebarWidth = SIDEBAR_WIDTH;

    float sidebarHeight = WINDOW_HEIGHT - TOP_BAR_HEIGHT - bottomBarHeight;
    float sidebarCenterY = TOP_BAR_HEIGHT + sidebarHeight / 2.0f;

    addRectangle("TopBar", {WINDOW_WIDTH / 2.0f, TOP_BAR_HEIGHT / 2.0f}, WINDOW_WIDTH,
                 TOP_BAR_HEIGHT, BUTTON_COLOR4);
    addRectangle("BottomBar", {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - bottomBarHeight / 2.0f},
                 WINDOW_WIDTH, bottomBarHeight, BUTTON_COLOR4);

    addRectangle("Entities Sidebar", {sidebarWidth / 2.0f, sidebarCenterY}, sidebarWidth,
                 sidebarHeight, BUTTON_COLOR3);
    addRectangle("Prefabs Sidebar", {WINDOW_WIDTH - sidebarWidth / 2.0f, sidebarCenterY},
                 sidebarWidth, sidebarHeight, BUTTON_COLOR3);
    addRectangle("Entities Text Underline", {200, TOP_BAR_HEIGHT + 64}, sidebarWidth, 4,
                 ColorRGBA{12, 13, 17, 255});
    addRectangle("Prefabs Text Underline", {WINDOW_WIDTH - 200, TOP_BAR_HEIGHT + 64}, sidebarWidth,
                 4, ColorRGBA{12, 13, 17, 255});
}

Component EditorMenu::createComponent(std::string componentName, std::vector<std::string> values)
{
    for (const Component &componentTemplate : _componentTemplates) {
        if (componentTemplate.name != componentName)
            continue;

        Component component = componentTemplate;

        for (int index = 0; index < static_cast<int>(values.size()) &&
                            index < static_cast<int>(component.properties.size());
             ++index)
            component.properties[index].value = values[index];

        return component;
    }

    return Component{componentName, {}, _components.getComponentColor(componentName)};
}

void EditorMenu::initPrefabs(SceneObjects sceneObjects)
{
    if (!sceneObjects._prefabs.empty()) {
        _prefabs = sceneObjects._prefabs;

        for (Prefab &prefab : _prefabs) {
            for (Component &component : prefab.components) {
                component.color = _components.getComponentColor(component.name);
                syncComponentWithTemplate(component);
            }
        }
        return;
    }
}

void EditorMenu::initEntities(SceneObjects sceneObjects)
{
    if (!sceneObjects._entities.empty()) {
        _entities = sceneObjects._entities;

        for (Entity &entity : _entities) {
            for (Component &component : entity.components) {
                component.color = _components.getComponentColor(component.name);
                syncComponentWithTemplate(component);
            }
        }
        return;
    }

    if (_template != "2D")
        return;

    _entities = {
        {0, "Player", {createComponent("Position", {"960", "450"}), createComponent("Sprite")}}};
}

void EditorMenu::initComponentTemplates()
{
    _componentTemplates.clear();

    for (const ComponentDescriptor &componentDescriptor : _componentDescriptors) {
        Component componentTemplate;
        componentTemplate.name = componentDescriptor.name;
        componentTemplate.color = _components.getComponentColor(componentDescriptor.name);

        int propertyId = 0;

        for (const PropertyDescriptor &propertyDescriptor : componentDescriptor.properties) {
            componentTemplate.properties.push_back({propertyId, propertyDescriptor.name,
                                                    propertyDescriptor.defaultValue,
                                                    propertyDescriptor.type});
            propertyId++;
        }
        _componentTemplates.push_back(componentTemplate);
    }
}

void EditorMenu::syncComponentWithTemplate(Component &component)
{
    for (const Component &componentTemplate : _componentTemplates) {
        if (componentTemplate.name != component.name)
            continue;

        std::vector<Property> syncedProperties = componentTemplate.properties;

        for (Property &syncedProperty : syncedProperties) {
            for (const Property &savedProperty : component.properties) {
                if (savedProperty.name == syncedProperty.name &&
                    savedProperty.type == syncedProperty.type)
                    syncedProperty.value = savedProperty.value;
            }
        }
        component.properties = syncedProperties;
        return;
    }
}
