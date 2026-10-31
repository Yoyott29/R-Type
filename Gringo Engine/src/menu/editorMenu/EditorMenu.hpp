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

#define COMPONENT_LIST = {"Sprite", "Collider", "Script"}
#define COMPONENT_COLS = {COLORGBA{255, 0, 0, 255}, COLORRGBA{0, 255, 0, 255}, COLORRGBA{0, 0, 255, 255}}

enum class valueType {
    Int,
    Float,
    String,
    Bool,
    Color,
    Shape,
};    
struct Property {
    int id;
    std::string name;
    std::string value;
    valueType type;
};

struct Component {
    std::string name;
    std::vector<Property> properties;
    ColorRGBA color;
};    

enum class ObjectType {
    Entity,
    Prefab
};

struct EditingText {
    std::string name;

    std::string objectComponent;
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

struct Entity {
    int id;
    std::string name;
    std::vector<Component> components;
};

struct Prefab {
    int id;
    std::string name;
    std::vector<Component> components;
};


class EditorMenu : public AMenu
{
  public:
    EditorMenu(IRenderer &renderer, std::string templateName);
    ~EditorMenu() override = default;

    void update() override;

    //INITIALIZATION

    void initTexts() override;
    void initButtons() override;
    void initRectangles() override;
    void initPrefabs();
    void initEntities();
    void initComponentTemplates();

    //ADDING AND REMOVING ENTITIES AND PREFABS

    void addNewEntity();
    void addNewPrefab();
    void editEntityName(int entityId,  std::string &newName);
    void editPrefabName(int prefabId,  std::string &newName);
    void editObject(ObjectType objectType, int objectId);

    void applyCurrentEdit();
    std::string getObjectName(ObjectType type, int id);
    std::string getObjectButtonName(ObjectType type, int id);

    void refreshObjectList(ObjectType type);
    void changePage(ObjectType type, int delta);
    int getPageCount(ObjectType type);

    void deleteSelectedObject();

    //COMPONENTS MANAGEMENT

    void updateObjectHeader();
    void changeComponentPage(int delta);
    
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
    std::vector<Component> _componentTemplates;

    std::vector<Entity> _entities;
    std::vector<Prefab> _prefabs;

    int _currentEntityPage = 0;
    int _currentPrefabPage = 0;
    int _currentComponentPage = 0;

    std::vector<std::string> _componentsTextNames;
    std::vector<std::string> _componentsRectangleNames;
    std::vector<std::string> _componentsButtonNames;

    int _entityNumber = 0;
    int _prefabNumber = 0;
    std::vector<std::string> _entityListNames;
    std::vector<std::string> _prefabListNames;

    EditingText _currentlyUpdatedProperty;
    ObjectType _selectedObjectType;
    int _selectedObjectId;
    float _addButtonPositionX = 0.0f;
    float _addButtonPositionY = 0.0f;

    std::vector<std::string> _pickerButtonNames;

    bool _newComponentSelection = false;

    bool editingName = false;

    bool _deleteObject = false;

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
    
};

#endif /* !EDITORMENU_HPP_ */
