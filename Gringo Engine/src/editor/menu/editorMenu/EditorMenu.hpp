/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "AMenu.hpp"
#include "ComponentCatalog.hpp"
#include "EditorAnimations.hpp"
#include "EditorComponents.hpp"
#include "EditorObjects.hpp"
#include "EditorProperties.hpp"
#include "Parser.hpp"
#include "PluginLoader.hpp"
#include "iostream"
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

#ifndef EDITORMENU_HPP_
#define EDITORMENU_HPP_

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define TOP_BAR_HEIGHT 64
#define BOTTOM_BAR_HEIGHT 64

#define SIDEBAR_WIDTH 400

#define CURSOR_BLINK_FRAMES 30
#define CURSOR_WIDTH 2

#define MAX_NAME_LENGTH 20

#define ITEMS_PER_PAGE 15

#define SAVED_MESSAGE_FRAMES 180

enum class ObjectType { Entity, Prefab };

struct EditingText {
    std::string name;

    std::string objectProperty;
    bool editingName = false;
    // Set: the typed text is given to it instead of a component property (Animation panel)
    std::function<void(const std::string &)> commit;

    int cursorIndex = 0;
    int blinkFrames = 0;

    int objectComponentIndex = 0;

    valueType type;
    Vec2 position;
    int fontSize;
    std::string text;
    ColorRGBA color;

    bool active = false;
};

class EditorMenu : public AMenu
{
  public:
    EditorMenu(
        IEditorRenderer &renderer, std::string templateName, std::string projectName,
        Parser &parser, SceneObjects sceneObjects,
        const std::vector<ComponentDescriptor> &componentDescriptors,
        const std::map<std::string, std::vector<PropertyDescriptor>> &pluginSettingDescriptors);
    ~EditorMenu() override;

    void update() override;
    void saveProject();
    void buildProject();
    std::string getBuiltProjectPath() const;
    SceneObjects getSceneObjects() const
    {
        return {_projectName,     _entities,        _prefabs,       _activePlugins,
                _projectFilePath, _projectRenderer, _pluginSettings};
    }
    void showPanel(const std::string &text);
    void closePluginPanel();
    void togglePlugin(const std::string &plugin);
    void updateBuildButton();
    bool hasActivePlugin(const std::string &plugin) const;
    // Settings panel: switches the project to another renderer (a project always has one)
    void selectRenderer(const std::string &renderer);
    // Removes everything the Settings and Plugins panels added
    void clearPanel();
    // Settings panel: a section per active plugin with settings, a row per setting
    void addPluginSettingRows(float &y);
    // Click on a setting: cycles Choice/Bool/Color/File values, starts typing the others
    void editPluginSetting(const std::string &plugin, const PropertyDescriptor &setting);
    // Keyboard while a setting is typed: Enter keeps it
    void updateSettingEdit();
    void commitSettingEdit();
    // Every active plugin gets its default settings when the project does not set them
    void fillDefaultPluginSettings();

    // INITIALIZATION

    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;
    Component createComponent(std::string componentName, std::vector<std::string> values = {});
    void initPrefabs(SceneObjects sceneObjects);
    void initEntities(SceneObjects sceneObjects);
    void syncComponentWithTemplate(Component &component);
    // Every entity and prefab has a Transform, first in its list: added if missing, never removed
    static inline const std::string REQUIRED_COMPONENT = "Transform";
    void ensureRequiredComponent(std::vector<Component> &components);
    void initComponentTemplates();

  private:
    friend class EditorObjects;
    friend class EditorComponents;
    friend class EditorProperties;
    friend class EditorAnimations;

    std::string _template;
    std::string _projectName;
    Parser &_parser;
    std::vector<Component> _componentTemplates;
    const std::vector<ComponentDescriptor> &_componentDescriptors;

    std::vector<Entity> _entities;
    std::vector<Prefab> _prefabs;
    std::filesystem::path _projectFilePath;
    std::vector<std::string> _activePlugins;
    std::vector<std::string> _availablePlugins;
    // Not _renderer: AMenu::_renderer is the editor's own window
    std::string _projectRenderer;
    std::vector<std::string> _availableRenderers;
    std::map<std::string, std::map<std::string, std::string>> _pluginSettings;
    const std::map<std::string, std::vector<PropertyDescriptor>> &_pluginSettingDescriptors;
    struct SettingEdit {
        bool active = false;
        std::string plugin;
        std::string key;
        std::string text;
        valueType type = valueType::String;
    } _settingEdit;
    std::vector<std::string> _settingButtonNames;
    std::vector<std::string> _settingTextNames;

    int _currentEntityPage = 0;
    int _currentPrefabPage = 0;
    int _currentComponentPage = 0;

    std::vector<std::string> _componentsTextNames;
    std::vector<std::string> _componentsRectangleNames;
    std::vector<std::string> _componentsButtonNames;

    std::vector<std::string> _entityListNames;
    std::vector<std::string> _prefabListNames;

    EditingText _currentlyUpdatedProperty;
    ObjectType _selectedObjectType;
    int _selectedObjectId;
    int _savedMessageFrames = 0;

    EditorObjects _objects;
    EditorComponents _components;
    EditorProperties _properties;
    EditorAnimations _animations;
};

#endif /* !EDITORMENU_HPP_ */
