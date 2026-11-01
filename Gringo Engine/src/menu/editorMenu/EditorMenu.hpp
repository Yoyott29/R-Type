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


    //ADDING AND REMOVING ENTITIES AND PREFABS

    void addNewEntity();
    void addNewPrefab();
    void editEntityName(int entityId,  std::string &newName);
    void editPrefabName(int prefabId,  std::string &newName);
    void editObject(ObjectType objectType, int objectId);

    std::string getObjectName(ObjectType type, int id);
    std::string getObjectButtonName(ObjectType type, int id);
    std::string cleanNumber(std::string text);
    void applyCurrentEdit();
    

    void refreshObjectList(ObjectType type);
    void changePage(ObjectType type, int delta);
    int getPageCount(ObjectType type);

    void deleteSelectedObject();

    //COMPONENTS MANAGEMENT

    void updateObjectHeader();
    void changeComponentPage(int delta);
    ColorRGBA getComponentColor(const std::string &name);
    
    void clearDisplayedComponents();
    void createComponents(std::vector<Component> components, ObjectType objectType, int objectId);
    std::vector<Component> &getObjectComponents(ObjectType objectType, int objectId);
    void openComponentPicker();
    void closeComponentPicker();
    void addComponentToSelectedObject( const Component &componentToAdd);
    void deleteComponent(int index);

    //PROPERTIES MANAGEMENT

    void createProperties( Component &component, int componentIndex, float positionX, float positionY, float componentWidth, ObjectType objectType, int objectId);    void updateObjectProperty();
    void manageInputs();
    void drawTextCursor();
    bool isCharacterAllowed(char character, valueType type,  std::string &currentContent, int cursorIndex);

    std::string nextShapeName(std::string current);
    std::string nextColorName(std::string current);

  private:
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
    float _addButtonPositionX = 0.0f;
    float _addButtonPositionY = 0.0f;

    std::vector<std::string> _pickerButtonNames;

    bool _deleteObject = false;

    int _savedMessageFrames = 0;

    std::vector<std::pair<Shape, std::string>> _shapeOptions = {
        {Shape::BoxShape, "Box"},
        {Shape::CircleShape, "Circle"},
    };

    std::vector<std::pair<std::string, ColorRGBA>> _colorOptions = {
        {"Red", {255, 0, 0, 255}},
        {"Orange", {255, 165, 0, 255}},
        {"Yellow", {255, 255, 0, 255}},
        {"Lime", {0, 255, 0, 255}},
        {"Green", {0, 128, 0, 255}},
        {"Cyan", {0, 255, 255, 255}},
        {"Blue", {0, 0, 255, 255}},
        {"Purple", {128, 0, 128, 255}},
        {"Magenta", {255, 0, 255, 255}},
        {"Pink", {255, 192, 203, 255}},
        {"Brown", {139, 69, 19, 255}},
        {"White", {255, 255, 255, 255}},
        {"Gray", {128, 128, 128, 255}},
        {"Black", {0, 0, 0, 255}},
        {"Transparent", {0, 0, 0, 0}},
    };

    std::map<std::string, ColorRGBA> _componentColors = {
        {"Position", {58, 53, 63, 255}},
        {"Velocity", {51, 59, 63, 255}},
        {"Sprite",   {51, 56, 70, 255}},
        {"Collider", {58, 57, 61, 255}},
        {"Script",   {56, 53, 69, 255}},
    };
    
};

#endif /* !EDITORMENU_HPP_ */
