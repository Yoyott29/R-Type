/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "AMenu.hpp"
#include <map>
#include <string>
#include <vector>
#include "iostream"
#include "Parser.hpp"
#include "EditorObjects.hpp"
#include "EditorComponents.hpp"
#include "EditorProperties.hpp"

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
   

enum class ObjectType {
    Entity,
    Prefab
};

struct EditingText {
    std::string name;

    std::string objectProperty;
    bool editingName = false;

    int cursorIndex = 0;
    int blinkFrames = 0;

    int objectComponentIndex = 0;
    
    valueType type;
    Position position;
    int fontSize;
    std::string text;
    ColorRGBA color;

    bool active = false;
};


class EditorMenu : public AMenu
{
  public:
    EditorMenu(IRenderer &renderer, std::string templateName, std::string projectName, Parser &parser, SceneObjects sceneObjects);
    ~EditorMenu() override;

    void update() override;
    void saveProject();
    void showPanel(const std::string &text);

    //INITIALIZATION

    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;
    Component createComponent(std::string componentName, std::vector<std::string> values = {});
    void initPrefabs(SceneObjects sceneObjects);
    void initEntities(SceneObjects sceneObjects);
    void initComponentTemplates();

  private:
    friend class EditorObjects;
    friend class EditorComponents;
    friend class EditorProperties;

    std::string _template;
    std::string _projectName;
    Parser &_parser;
    std::vector<Component> _componentTemplates;

    std::vector<Entity> _entities;
    std::vector<Prefab> _prefabs;

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

    
};

#endif /* !EDITORMENU_HPP_ */
