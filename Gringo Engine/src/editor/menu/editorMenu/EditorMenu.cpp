/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "EditorMenu.hpp"
#include "ProjectBuilder.hpp"
#include <algorithm>
#include <iostream>
#include <utility>

EditorMenu::EditorMenu(
    IEditorRenderer &renderer, std::string templateName, std::string projectName, Parser &parser,
    SceneObjects sceneObjects, const std::vector<ComponentDescriptor> &componentDescriptors,
    const std::map<std::string, std::vector<PropertyDescriptor>> &pluginSettingDescriptors)
    : AMenu(renderer), _template(templateName), _projectName(projectName), _parser(parser),
      _componentDescriptors(componentDescriptors), _projectFilePath(sceneObjects.projectFilePath),
      _activePlugins(sceneObjects.activePlugins), _availablePlugins(PluginLoader{}.listNames()),
      _projectRenderer(sceneObjects.renderer),
      _availableRenderers(PluginLoader{"renderers"}.listNames()),
      _pluginSettings(sceneObjects.pluginSettings),
      _pluginSettingDescriptors(pluginSettingDescriptors), _objects(*this), _components(*this),
      _properties(*this), _animations(*this)
{
    _currentMenu = MenuType::EditorMenu;

    if (_projectName.empty())
        _projectName = sceneObjects.projectName;
    // A project always has a renderer: one saved without gets the default (RaylibRenderer if
    // there, otherwise the first one)
    if (_projectRenderer.empty() && !_availableRenderers.empty()) {
        const bool hasRaylib = std::find(_availableRenderers.begin(), _availableRenderers.end(),
                                         "RaylibRenderer") != _availableRenderers.end();
        _projectRenderer = hasRaylib ? "RaylibRenderer" : _availableRenderers.front();
    }

    fillDefaultPluginSettings();
    initComponentTemplates();
    initPrefabs(sceneObjects);
    initEntities(sceneObjects);

    EditorMenu::initTexts();
    EditorMenu::initButtons();
    EditorMenu::initRectangles();
}

EditorMenu::~EditorMenu()
{
    try {
        _properties.applyCurrentEdit();
        _parser.saveFile(_projectName + ".gringo", getSceneObjects());
    } catch (...) {
    }
}

void EditorMenu::update()
{
    updateBuildButton();
    drawRectangles();
    drawTexts();
    handleButtons();
    _properties.manageInputs();
    _properties.drawTextCursor();
    updateSettingEdit();

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

bool EditorMenu::hasActivePlugin(const std::string &plugin) const
{
    return std::find(_activePlugins.begin(), _activePlugins.end(), plugin) != _activePlugins.end();
}

void EditorMenu::updateBuildButton()
{
    // Every component needs the plugin that registered it (descriptor.plugin, empty for the engine)
    const auto pluginsActiveFor = [this](const std::vector<Component> &components) {
        return std::all_of(
            components.begin(), components.end(), [this](const Component &component) {
                const auto descriptor =
                    std::find_if(_componentDescriptors.begin(), _componentDescriptors.end(),
                                 [&component](const ComponentDescriptor &candidate) {
                                     return candidate.name == component.name;
                                 });
                if (descriptor == _componentDescriptors.end())
                    return false;
                return descriptor->plugin.empty() || hasActivePlugin(descriptor->plugin);
            });
    };
    const bool componentsValid =
        std::all_of(_entities.begin(), _entities.end(),
                    [&](const Entity &entity) { return pluginsActiveFor(entity.components); }) &&
        std::all_of(_prefabs.begin(), _prefabs.end(),
                    [&](const Prefab &prefab) { return pluginsActiveFor(prefab.components); });
    const bool pluginsAvailable = std::all_of(
        _activePlugins.begin(), _activePlugins.end(), [this](const std::string &plugin) {
            return std::find(_availablePlugins.begin(), _availablePlugins.end(), plugin) !=
                   _availablePlugins.end();
        });
    // The project's renderer must be one of Gringo Engine/renderers
    const bool rendererValid = std::find(_availableRenderers.begin(), _availableRenderers.end(),
                                         _projectRenderer) != _availableRenderers.end();
    const bool valid = pluginsAvailable && componentsValid && rendererValid;
    setButtonEnabled("Build", valid);
}

void EditorMenu::saveProject()
{
    _properties.applyCurrentEdit();
    _parser.saveFile(_projectName + ".gringo", getSceneObjects());
    _projectFilePath = _parser.getLastSavedPath();

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

std::string EditorMenu::getBuiltProjectPath() const { return _projectFilePath.string(); }

void EditorMenu::buildProject()
{
    _properties.applyCurrentEdit();
    const SceneObjects sceneObjects = getSceneObjects();
    _parser.saveFile(_projectName + ".gringo", sceneObjects);
    _projectFilePath = _parser.getLastSavedPath();

    // Missing scripts or assets, wrong ChildOf: nothing is compiled, the editor stays open
    const std::vector<std::string> missing = ProjectBuilder::buildErrors(
        sceneObjects, Parser::getProjectFolder(_projectFilePath, _projectName),
        _componentDescriptors);
    if (!missing.empty()) {
        for (const std::string &line : missing)
            std::cerr << line << std::endl;
        return;
    }
    _switchMenu = MenuType::Build;
    _isRunning = false;
}

void EditorMenu::clearPanel()
{
    commitSettingEdit();
    _animations.clear();
    removeText("PanelText");
    for (const std::string &plugin : _availablePlugins) {
        removeButton("Plugin_" + plugin);
        removeButton("PluginToggle_" + plugin);
        removeText("PluginLabel_" + plugin);
    }
    removeButton("RendererSelect_None");
    removeText("RendererLabel_None");
    for (const std::string &renderer : _availableRenderers) {
        removeButton("RendererSelect_" + renderer);
        removeText("RendererLabel_" + renderer);
    }
    for (const std::string &name : _settingButtonNames)
        removeButton(name);
    for (const std::string &name : _settingTextNames)
        removeText(name);
    _settingButtonNames.clear();
    _settingTextNames.clear();
}

void EditorMenu::showPanel(const std::string &text)
{
    _properties.applyCurrentEdit();
    _components.clearDisplayedComponents();
    clearPanel();

    if (text == "Settings") {
        // Renderer of the project: one of Gringo Engine/renderers, never none
        addText("PanelText", {WINDOW_WIDTH / 2.0f, 130}, 40, "Renderer", TEXT_COLOR);
        float y = 220;
        for (const std::string &choice : _availableRenderers) {
            const std::string &renderer = choice;
            const bool selected = renderer == _projectRenderer;
            addText("RendererLabel_" + choice, {WINDOW_WIDTH / 2.0f - 180.0f, y}, 22, choice,
                    TEXT_COLOR);
            addButton(
                "RendererSelect_" + choice, {WINDOW_WIDTH / 2.0f + 150.0f, y}, 140, 42, 18,
                [this, renderer]() { selectRenderer(renderer); },
                selected ? BUTTON_COLOR2 : BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR,
                selected ? "SELECTED" : "USE");
            y += 65;
        }
        addPluginSettingRows(y);
    } else if (text == "Plugins") {
        addText("PanelText", {WINDOW_WIDTH / 2.0f, 130}, 40, "Active plugins", TEXT_COLOR);
        float y = 220;
        for (const std::string &plugin : _availablePlugins) {
            const bool active = std::find(_activePlugins.begin(), _activePlugins.end(), plugin) !=
                                _activePlugins.end();
            addText("PluginLabel_" + plugin, {WINDOW_WIDTH / 2.0f - 180.0f, y}, 22, plugin,
                    TEXT_COLOR);
            addButton(
                "PluginToggle_" + plugin, {WINDOW_WIDTH / 2.0f + 150.0f, y}, 100, 42, 18,
                [this, plugin]() { togglePlugin(plugin); }, active ? BUTTON_COLOR2 : BUTTON_COLOR1,
                BUTTON_COLOR3, TEXT_COLOR, active ? "ON" : "OFF");
            y += 65;
        }
    } else if (text == "Animation") {
        _animations.show();
    }
}

void EditorMenu::closePluginPanel() { clearPanel(); }

void EditorMenu::addPluginSettingRows(float &y)
{
    // One section per active plugin declaring settings (IPlugin::settings)
    for (const std::string &plugin : _activePlugins) {
        auto descriptors = _pluginSettingDescriptors.find(plugin);
        if (descriptors == _pluginSettingDescriptors.end() || descriptors->second.empty())
            continue;
        y += 25;
        const std::string header = "SettingHeader_" + plugin;
        addText(header, {WINDOW_WIDTH / 2.0f, y}, 30, plugin + " settings", TEXT_COLOR);
        _settingTextNames.push_back(header);
        y += 60;
        for (const PropertyDescriptor &setting : descriptors->second) {
            const std::string id = plugin + "_" + setting.name;
            const bool editing = _settingEdit.active && _settingEdit.plugin == plugin &&
                                 _settingEdit.key == setting.name;
            const std::string value =
                editing ? _settingEdit.text + "_" : _pluginSettings[plugin][setting.name];
            addText("SettingLabel_" + id, {WINDOW_WIDTH / 2.0f - 180.0f, y}, 22, setting.name,
                    TEXT_COLOR);
            addButton(
                "SettingValue_" + id, {WINDOW_WIDTH / 2.0f + 150.0f, y}, 260, 42, 18,
                [this, plugin, setting]() { editPluginSetting(plugin, setting); },
                editing ? BUTTON_COLOR2 : BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR, value);
            _settingTextNames.push_back("SettingLabel_" + id);
            _settingButtonNames.push_back("SettingValue_" + id);
            y += 55;
        }
    }
}

void EditorMenu::editPluginSetting(const std::string &plugin, const PropertyDescriptor &setting)
{
    commitSettingEdit();
    std::string &value = _pluginSettings[plugin][setting.name];

    // Values with a fixed set cycle on click, the others are typed
    if (setting.type == valueType::Choice)
        value = EditorProperties::nextOption(setting.options, value);
    else if (setting.type == valueType::Bool)
        value = value == "True" ? "False" : "True";
    else if (setting.type == valueType::Color)
        value = _properties.nextColorName(value);
    else if (setting.type == valueType::File)
        value = EditorProperties::nextOption(_properties.getProjectFiles(setting.options), value);
    else
        _settingEdit = {true, plugin, setting.name, value, setting.type};
    showPanel("Settings");
}

void EditorMenu::updateSettingEdit()
{
    if (!_settingEdit.active)
        return;
    std::string &text = _settingEdit.text;
    for (int character = _renderer.getNextCharacter(); character > 0;
         character = _renderer.getNextCharacter()) {
        const bool digit = character >= '0' && character <= '9';
        const bool allowed =
            _settingEdit.type == valueType::String  ? character >= 32 && character <= 126
            : _settingEdit.type == valueType::Float ? digit || character == '-' || character == '.'
                                                    : digit || character == '-';
        if (allowed && text.size() < 64)
            text += static_cast<char>(character);
    }
    if (_renderer.isKeyPressed(KeyboardKeys::KEY_BACKSPACE) && !text.empty())
        text.pop_back();
    if (_renderer.isKeyPressed(KeyboardKeys::KEY_ENTER)) {
        commitSettingEdit();
        showPanel("Settings");
        return;
    }
    setButtonText("SettingValue_" + _settingEdit.plugin + "_" + _settingEdit.key, text + "_");
}

void EditorMenu::commitSettingEdit()
{
    if (!_settingEdit.active)
        return;
    _settingEdit.active = false;
    _pluginSettings[_settingEdit.plugin][_settingEdit.key] = _settingEdit.text;
}

void EditorMenu::fillDefaultPluginSettings()
{
    for (const std::string &plugin : _activePlugins) {
        auto descriptors = _pluginSettingDescriptors.find(plugin);
        if (descriptors == _pluginSettingDescriptors.end())
            continue;
        for (const PropertyDescriptor &setting : descriptors->second)
            _pluginSettings[plugin].try_emplace(setting.name, setting.defaultValue);
    }
}

void EditorMenu::selectRenderer(const std::string &renderer)
{
    _projectRenderer = renderer;
    showPanel("Settings");
}

void EditorMenu::togglePlugin(const std::string &plugin)
{
    auto iterator = std::find(_activePlugins.begin(), _activePlugins.end(), plugin);
    if (iterator == _activePlugins.end())
        _activePlugins.push_back(plugin);
    else
        _activePlugins.erase(iterator);
    fillDefaultPluginSettings();
    initComponentTemplates();
    showPanel("Plugins");
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
    float animationCenterX = pluginsCenterX + buttonWidth + buttonSpacing;
    float backCenterX = WINDOW_WIDTH - sideMargin - backButtonWidth / 2.0f;

    // top bar buttons
    addButton(
        "Settings", {settingsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22,
        [this]() { showPanel("Settings"); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Plugins", {pluginsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22,
        [this]() { showPanel("Plugins"); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Animation", {animationCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22,
        [this]() { showPanel("Animation"); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    addButton(
        "Back", {backCenterX, topBarCenterY}, backButtonWidth, buttonHeight, 22,
        [this]() {
            _template.empty() ? _switchMenu = MenuType::LoadMenu
                              : _switchMenu = MenuType::TemplateMenu;
            saveProject();
        },
        BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    addButton(
        "Build", {backCenterX - backButtonWidth - buttonSpacing, topBarCenterY}, backButtonWidth,
        buttonHeight, 22, [this]() { buildProject(); }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

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
    // Draw order 1: always on top of the sidebars they are drawn on
    addRectangle("Entities Text Underline", {200, TOP_BAR_HEIGHT + 64}, sidebarWidth, 4,
                 ColorRGBA{12, 13, 17, 255}, 1);
    addRectangle("Prefabs Text Underline", {WINDOW_WIDTH - 200, TOP_BAR_HEIGHT + 64}, sidebarWidth,
                 4, ColorRGBA{12, 13, 17, 255}, 1);
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
            ensureRequiredComponent(prefab.components);
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
            ensureRequiredComponent(entity.components);
        }
        return;
    }

    if (_template == "3D") {
        _entities = {{0,
                      "Camera",
                      {createComponent("Transform", {"0", "3", "8"}), createComponent("Camera"),
                       createComponent("CameraFollow3D"), createComponent("ChildOf", {"1"})}},
                     {1, "Cube", {createComponent("Transform"), createComponent("3DModel")}}};
        return;
    }
    if (_template != "2D")
        return;

    _entities = {
        {0, "Player", {createComponent("Transform", {"960", "450"}), createComponent("Sprite")}}};
}

void EditorMenu::ensureRequiredComponent(std::vector<Component> &components)
{
    const bool present =
        std::any_of(components.begin(), components.end(), [](const Component &component) {
            return component.name == REQUIRED_COMPONENT;
        });
    if (!present)
        components.insert(components.begin(), createComponent(REQUIRED_COMPONENT));
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
            componentTemplate.properties.push_back(
                {propertyId, propertyDescriptor.name, propertyDescriptor.defaultValue,
                 propertyDescriptor.type, propertyDescriptor.options,
                 propertyDescriptor.visibleWhen});
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
                // By name only: a property whose type changed (e.g. TexturePath, now a File)
                // keeps its value
                if (savedProperty.name == syncedProperty.name)
                    syncedProperty.value = savedProperty.value;
            }
        }
        std::vector<Property> savedProperties = component.properties;
        component.properties = syncedProperties;
        const auto script = std::find_if(component.properties.begin(), component.properties.end(),
                                         &EditorProperties::isScriptFile);
        if (script != component.properties.end()) {
            _properties.applyScriptParameters(component, std::string(script->value));
            for (Property &parameter : component.properties)
                for (const Property &savedProperty : savedProperties)
                    if (parameter.name == savedProperty.name)
                        parameter.value = savedProperty.value;
        }

        return;
    }
}
