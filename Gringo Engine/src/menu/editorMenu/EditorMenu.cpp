/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorMenu
*/

#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

EditorMenu::EditorMenu(IRenderer &renderer, std::string templateName)
    : AMenu(renderer), _template(templateName)
{
    initPrefabs();
    initEntities();

    initTexts();
    initButtons();
    initRectangles();
    initComponentTemplates();

    _entityNumber = static_cast<int>(_entities.size());
    _prefabNumber = static_cast<int>(_prefabs.size());
}

void EditorMenu::update()
{
    drawRectangles();
    drawTexts();
    handleButtons();
    manageInputs();
    drawTextCursor();

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_ENTER) && _currentlyUpdatedProperty.active) {
         std::string editedButton = _currentlyUpdatedProperty.name;
         std::string editButton = _currentlyUpdatedProperty.editingName ? "ObjectHeader_Rename" : "";

        applyCurrentEdit();

        for (std::string name : {editedButton, editButton}) {
            auto it = _buttons.find(name);
            if (it != _buttons.end())
                unSelectButton(it->second);
        }
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
    addText("Prefab Page", {WINDOW_WIDTH - 200, WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20}, 20, "1", TEXT_COLOR);
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
    addButton("Settings", {settingsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22, nullptr,
              BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);
    addButton("Plugins", {pluginsCenterX, topBarCenterY}, buttonWidth, buttonHeight, 22, nullptr,
              BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);
              
    addButton(
        "Back", {backCenterX, topBarCenterY}, backButtonWidth, buttonHeight, 22,
        [this]() { _switchMenu = "TemplateMenu"; }, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    addButton("Build", {backCenterX - backButtonWidth - buttonSpacing, topBarCenterY},
              backButtonWidth, buttonHeight, 22, nullptr, BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    float navY = WINDOW_HEIGHT - TOP_BAR_HEIGHT - buttonHeight / 2.0f;

    addButton("Previous Entity Page", {130, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
              [this]() { changePage(ObjectType::Entity, -1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "<");
    addButton("Next Entity Page", {270, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
              [this]() { changePage(ObjectType::Entity, 1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, ">");

    addButton("Previous Prefab Page", {WINDOW_WIDTH - 270.0f, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
              [this]() { changePage(ObjectType::Prefab, -1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "<");
    addButton("Next Prefab Page", {WINDOW_WIDTH - 130.0f, navY}, SIDEBAR_WIDTH / 6, buttonHeight / 2.0f, 18,
              [this]() { changePage(ObjectType::Prefab, 1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, ">");
    addButton("Save Project", {backCenterX, WINDOW_HEIGHT - TOP_BAR_HEIGHT/2.0f}, 150, 40, 20, nullptr,
              BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR);

    refreshObjectList(ObjectType::Entity);
    refreshObjectList(ObjectType::Prefab);
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

void EditorMenu::initPrefabs()
{
    Component position = {"Position", {{0, "PositionX", "0", valueType::Float}, {1, "PositionY", "0", valueType::Float}}, ColorRGBA{58, 53, 63, 255}};
    Component sprite = {"Sprite", {{0, "Width", "32", valueType::Float}, {1, "Height", "32", valueType::Float}, {2, "TexturePath", "default.png", valueType::String}, {3, "Layer", "1", valueType::Float}, {4, "Color", "Transparent", valueType::Color}}, ColorRGBA{51, 56, 70, 255}};
    Component collider = {"Collider", {{0, "Shape", "Box", valueType::Shape}, {1, "Width", "32", valueType::Float}, {2, "Height", "32", valueType::Float}, {3, "IsTrigger", "False", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};

    Component enemyVelocity = {"Velocity", {{0, "Dx", "-5", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component enemyScript = {"Script", {{0, "Path", "script.hpp", valueType::String}}, ColorRGBA{56, 53, 69, 255}};

    Component asteroidVelocity = {"Velocity", {{0, "Dx", "-2", valueType::Float}, {1, "Dy", "1", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component asteroidCollider = {"Collider", {{0, "Shape", "Circle", valueType::Shape}, {1, "Width", "32", valueType::Float}, {2, "Height", "32", valueType::Float}, {3, "IsTrigger", "False", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};
    Component asteroidScript = {"Script", {{0, "Path", "script.hpp", valueType::String}}, ColorRGBA{56, 53, 69, 255}};

    Component bulletVelocity = {"Velocity", {{0, "Dx", "10", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component bulletCollider = {"Collider", {{0, "Shape", "Box", valueType::Shape}, {1, "Width", "8", valueType::Float}, {2, "Height", "8", valueType::Float}, {3, "IsTrigger", "True", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};

    _prefabs = {
        {0, "EnemyShip", {position, enemyVelocity, sprite, collider, enemyScript}},
        {1, "Asteroid", {position, asteroidVelocity, sprite, asteroidCollider, asteroidScript}},
        {2, "Bullet", {position, bulletVelocity, sprite, bulletCollider}}
    };
}

void EditorMenu::initEntities()
{
    Component sprite = {"Sprite", {{0, "Width", "32", valueType::Float}, {1, "Height", "32", valueType::Float}, {2, "TexturePath", "default.png", valueType::String}, {3, "Layer", "1", valueType::Float}, {4, "Color", "Transparent", valueType::Color}}, ColorRGBA{51, 56, 70, 255}};
    Component collider = {"Collider", {{0, "Shape", "Box", valueType::Shape}, {1, "Width", "32", valueType::Float}, {2, "Height", "32", valueType::Float}, {3, "IsTrigger", "False", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};

    Component playerPosition = {"Position", {{0, "PositionX", "100", valueType::Float}, {1, "PositionY", "300", valueType::Float}}, ColorRGBA{58, 53, 63, 255}};
    Component playerVelocity = {"Velocity", {{0, "Dx", "0", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component playerScript = {"Script", {{0, "Path", "script.hpp", valueType::String}}, ColorRGBA{56, 53, 69, 255}};

    Component enemyPosition = {"Position", {{0, "PositionX", "900", valueType::Float}, {1, "PositionY", "200", valueType::Float}}, ColorRGBA{58, 53, 63, 255}};
    Component enemyVelocity = {"Velocity", {{0, "Dx", "-3", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component enemyScript = {"Script", {{0, "Path", "script.hpp", valueType::String}}, ColorRGBA{56, 53, 69, 255}};

    Component bulletPosition = {"Position", {{0, "PositionX", "132", valueType::Float}, {1, "PositionY", "300", valueType::Float}}, ColorRGBA{58, 53, 63, 255}};
    Component bulletVelocity = {"Velocity", {{0, "Dx", "10", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component bulletCollider = {"Collider", {{0, "Shape", "Box", valueType::Shape}, {1, "Width", "8", valueType::Float}, {2, "Height", "8", valueType::Float}, {3, "IsTrigger", "True", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};

    Component asteroidPosition = {"Position", {{0, "PositionX", "700", valueType::Float}, {1, "PositionY", "450", valueType::Float}}, ColorRGBA{58, 53, 63, 255}};
    Component asteroidVelocity = {"Velocity", {{0, "Dx", "-2", valueType::Float}, {1, "Dy", "0", valueType::Float}}, ColorRGBA{51, 59, 63, 255}};
    Component asteroidSprite = {"Sprite", {{0, "Width", "64", valueType::Float}, {1, "Height", "64", valueType::Float}, {2, "TexturePath", "default.png", valueType::String}, {3, "Layer", "1", valueType::Float}, {4, "Color", "Transparent", valueType::Color}}, ColorRGBA{51, 56, 70, 255}};
    Component asteroidCollider = {"Collider", {{0, "Shape", "Circle", valueType::Shape}, {1, "Width", "64", valueType::Float}, {2, "Height", "64", valueType::Float}, {3, "IsTrigger", "False", valueType::Bool}}, ColorRGBA{58, 57, 61, 255}};
    Component asteroidScript = {"Script", {{0, "Path", "script.hpp", valueType::String}}, ColorRGBA{56, 53, 69, 255}};

    _entities = {
        {0, "Player1", {playerPosition, playerVelocity, sprite, collider, playerScript}},
        {1, "Enemy1", {enemyPosition, enemyVelocity, sprite, collider, enemyScript}},
        {2, "Bullet1", {bulletPosition, bulletVelocity, sprite, bulletCollider}},
        {3, "Asteroid1", {asteroidPosition, asteroidVelocity, asteroidSprite, asteroidCollider, asteroidScript}}
    };
}

void EditorMenu::initComponentTemplates()
{
    std::vector<Property> positionProperties = {
        {0, "PositionX", "0", valueType::Float},
        {1, "PositionY", "0", valueType::Float}
    };

    std::vector<Property> velocityProperties = {
        {0, "Dx", "0", valueType::Float},
        {1, "Dy", "0", valueType::Float}
    };

    std::vector<Property> spriteProperties = {
        {0, "Width", "32", valueType::Float},
        {1, "Height", "32", valueType::Float},
        {2, "TexturePath", "default.png", valueType::String},
        {3, "Layer", "1", valueType::Float},
        {4, "Color", "Transparent", valueType::Color}
    };

    std::vector<Property> colliderProperties = {
        {0, "Shape", "Box", valueType::Shape},
        {1, "Width", "32", valueType::Float},
        {2, "Height", "32", valueType::Float},
        {3, "IsTrigger", "False", valueType::Bool}
    };

    std::vector<Property> scriptProperties = {
        {0, "Path", "script.hpp", valueType::String}
    };



    Component position = {"Position", positionProperties, ColorRGBA{58, 53, 63, 255}};
    Component velocity = {"Velocity", velocityProperties, ColorRGBA{51, 59, 63, 255}};
    Component sprite = {"Sprite", spriteProperties, ColorRGBA{51, 56, 70, 255}};
    Component collider = {"Collider", colliderProperties, ColorRGBA{58, 57, 61, 255}};
    Component script = {"Script", scriptProperties, ColorRGBA{56, 53, 69, 255}};

    _componentTemplates = {position, velocity, sprite, collider, script};
}

//--------------------------------------------------------------------------------------------------------------------------------
// ENTITIES AND PREFABS MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------

void EditorMenu::addNewEntity()
{
    applyCurrentEdit();

    int newId = _entities.empty() ? 1 : _entities.back().id + 1;
    _entities.push_back({newId, "Entity" + std::to_string(newId), {}});
    _entityNumber++;

    _currentEntityPage = static_cast<int>((_entities.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Entity);
}

void EditorMenu::addNewPrefab()
{
    applyCurrentEdit();

    int newId = _prefabs.empty() ? 1 : _prefabs.back().id + 1;
    _prefabs.push_back({newId, "Prefab" + std::to_string(newId), {}});
    _prefabNumber++;

    _currentPrefabPage = static_cast<int>((_prefabs.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Prefab);
}

void EditorMenu::editEntityName(int entityId,  std::string &newName)
{
    auto it = std::find_if(_entities.begin(), _entities.end(),
                           [entityId]( Entity &entity) { return entity.id == entityId; });

    if (it != _entities.end()) {
        it->name = newName;
        setButtonText(getObjectButtonName(ObjectType::Entity, entityId), newName);

        if (_selectedObjectType == ObjectType::Entity && _selectedObjectId == entityId)
            updateObjectHeader();
    }
}

void EditorMenu::editPrefabName(int prefabId,  std::string &newName)
{
    auto it = std::find_if(_prefabs.begin(), _prefabs.end(),
                           [prefabId]( Prefab &prefab) { return prefab.id == prefabId; });

    if (it != _prefabs.end()) {
        it->name = newName;
        setButtonText(getObjectButtonName(ObjectType::Prefab, prefabId), newName);

        if (_selectedObjectType == ObjectType::Prefab && _selectedObjectId == prefabId)
            updateObjectHeader();
    }
}

void EditorMenu::editObject(ObjectType objectType, int objectId)
{
    applyCurrentEdit();

    _currentlyUpdatedProperty.name = "ObjectHeader_Name";
    _currentlyUpdatedProperty.text = getObjectName(objectType, objectId);
    _currentlyUpdatedProperty.type = valueType::String;
    _currentlyUpdatedProperty.editingName = true;
    _currentlyUpdatedProperty.cursorIndex = static_cast<int>(_currentlyUpdatedProperty.text.size());
    _currentlyUpdatedProperty.blinkFrames = 0;
    _currentlyUpdatedProperty.active = true;
}

std::string EditorMenu::getObjectName(ObjectType type, int id)
{
    if (type == ObjectType::Entity) {
        auto it = std::find_if(_entities.begin(), _entities.end(),
                               [id]( Entity &e) { return e.id == id; });
        if (it != _entities.end())
            return it->name;
    } else {
        auto it = std::find_if(_prefabs.begin(), _prefabs.end(),
                               [id]( Prefab &p) { return p.id == id; });
        if (it != _prefabs.end())
            return it->name;
    }
    return "";
}

std::string EditorMenu::getObjectButtonName(ObjectType type, int id)
{
    return (type == ObjectType::Entity ? "Entity_" : "Prefab_") + std::to_string(id);
}

void EditorMenu::applyCurrentEdit()
{
    if (!_currentlyUpdatedProperty.active)
        return;

    if (_currentlyUpdatedProperty.text.empty()) {
        if (_currentlyUpdatedProperty.editingName)
            _currentlyUpdatedProperty.text =
                (_selectedObjectType == ObjectType::Entity ? "Entity" : "Prefab") + std::to_string(_selectedObjectId);
        else
            _currentlyUpdatedProperty.text = "0";

        setButtonText(_currentlyUpdatedProperty.name, _currentlyUpdatedProperty.text);
        updateObjectProperty();
    }
    _currentlyUpdatedProperty.active = false;
    _currentlyUpdatedProperty.editingName = false;
}

int EditorMenu::getPageCount(ObjectType type)
{
     int total = (type == ObjectType::Entity) ? _entities.size() : _prefabs.size();
    return static_cast<int>(total / ITEMS_PER_PAGE) + 1;
}

void EditorMenu::changePage(ObjectType type, int delta)
{
     bool isEntity = (type == ObjectType::Entity);
     int page = isEntity ? _currentEntityPage : _currentPrefabPage;
     int target = std::clamp(page + delta, 0, getPageCount(type) - 1);

    if (target == page)
        return;

    applyCurrentEdit();

    if (isEntity)
        _currentEntityPage = target;
    else
        _currentPrefabPage = target;

    refreshObjectList(type);
}

void EditorMenu::refreshObjectList(ObjectType type)
{
    bool isEntity = (type == ObjectType::Entity);
    int page = isEntity ? _currentEntityPage : _currentPrefabPage;
    std::vector<std::string> listNames = isEntity ? _entityListNames : _prefabListNames;

    float buttonHeight = 40.0f;
    float buttonSpacing = 16.0f;
    float firstRowY = static_cast<float>(TOP_BAR_HEIGHT + 100);
    float mainX = isEntity ? SIDEBAR_WIDTH / 2.0f : static_cast<float>(WINDOW_WIDTH) - SIDEBAR_WIDTH / 2.0f;
    float objectButtonWidth = SIDEBAR_WIDTH - 60.0f;

    for (int i = 0; i < listNames.size(); ++i)
        removeButton(listNames[i]);
    listNames.clear();

    std::vector<std::pair<int, std::string>> items;
    if (isEntity) {
        for (int i = 0; i < _entities.size(); ++i)
            items.emplace_back(_entities[i].id, _entities[i].name);
    } else {
        for (int i = 0; i < _prefabs.size(); ++i)
            items.emplace_back(_prefabs[i].id, _prefabs[i].name);
    }
    
    page = std::clamp(page, 0, getPageCount(type) - 1);
    if (isEntity)
        _currentEntityPage = page;
    else
        _currentPrefabPage = page;

     int start = static_cast<int>(page) * ITEMS_PER_PAGE;
     int end = std::min(start + ITEMS_PER_PAGE, static_cast<int>(items.size()));

     for (int i = start; i < end; ++i) {
         int id = items[i].first;
         float y = firstRowY + static_cast<float>(i - start) * (buttonHeight + buttonSpacing);
         std::string objectButton = getObjectButtonName(type, id);

        addButton(objectButton, {mainX, y}, objectButtonWidth, buttonHeight, 20,
                  [this, type, id]() { createComponents(getObjectComponents(type, id), type, id); },
                  BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, items[i].second);

        listNames.push_back(objectButton);
    }

    if (page == getPageCount(type) - 1) {
         std::string addName = isEntity ? "Add Entity" : "Add Prefab";
         float y = firstRowY + static_cast<float>(end - start) * (buttonHeight + buttonSpacing);

        addButton(addName, {mainX, y}, SIDEBAR_WIDTH / 4, buttonHeight / 2.0f, 18,
                  [this, isEntity]() { isEntity ? addNewEntity() : addNewPrefab(); },
                  BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
        listNames.push_back(addName);
    }

    if (isEntity)
        _entityListNames = listNames;
    else
        _prefabListNames = listNames;

     std::string pageTextName = isEntity ? "Entity Page" : "Prefab Page";
    removeText(pageTextName);
    addText(pageTextName,
            {mainX, static_cast<float>(WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20)}, 20,
            std::to_string(page + 1) + " / " + std::to_string(getPageCount(type)), TEXT_COLOR);

     std::string previousName = isEntity ? "Previous Entity Page" : "Previous Prefab Page";
     std::string nextName = isEntity ? "Next Entity Page" : "Next Prefab Page";

    if (_buttons.count(previousName) > 0)
        unSelectButton(_buttons[previousName]);
    if (_buttons.count(nextName) > 0)
        unSelectButton(_buttons[nextName]);
}

void EditorMenu::deleteSelectedObject()
{
    applyCurrentEdit();

     ObjectType type = _selectedObjectType;
     int id = _selectedObjectId;

    clearDisplayedComponents();

    if (type == ObjectType::Entity) {
        _entities.erase(std::remove_if(_entities.begin(), _entities.end(),
                        [id]( Entity &e) { return e.id == id; }), _entities.end());
        _entityNumber = static_cast<int>(_entities.size());
    } else {
        _prefabs.erase(std::remove_if(_prefabs.begin(), _prefabs.end(),
                       [id]( Prefab &p) { return p.id == id; }), _prefabs.end());
        _prefabNumber = static_cast<int>(_prefabs.size());
    }

    _selectedObjectId = -1;
    _currentComponentPage = 0;

    refreshObjectList(type);
}

//--------------------------------------------------------------------------------------------------------------------------------
// COMPONENTS MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------

void EditorMenu::updateObjectHeader()
{
    if (_buttons.count("ObjectHeader_Name") == 0)
        return;

    setButtonText("ObjectHeader_Name", getObjectName(_selectedObjectType, _selectedObjectId));
}

void EditorMenu::clearDisplayedComponents()
{
    _currentlyUpdatedProperty.active = false;
    _deleteObject = false;
    for ( std::string &textName : _componentsTextNames)
        removeText(textName);
    for ( std::string &rectangleName : _componentsRectangleNames)
        removeRectangle(rectangleName);
    for ( std::string &buttonName : _componentsButtonNames)
        removeButton(buttonName);

    _componentsTextNames.clear();
    _componentsRectangleNames.clear();
    _componentsButtonNames.clear();
    _pickerButtonNames.clear();
}

void EditorMenu::createComponents(std::vector<Component> components, ObjectType objectType, int objectId)
{
    applyCurrentEdit();

    if (objectType != _selectedObjectType || objectId != _selectedObjectId)
        _currentComponentPage = 0;

    _selectedObjectType = objectType;
    _selectedObjectId = objectId;

    clearDisplayedComponents();

    float componentWidth = 500.0f;
    float headerHeight = 40.0f;
    float propertyRowHeight = 30.0f;

    float xSpacing = 40.0f;
    float ySpacing = 32.0f;

    float topOffset = TOP_BAR_HEIGHT + 30.0f;
    float sidebarOffset = SIDEBAR_WIDTH + 40.0f;

    int columnCount = 2;

    float firstPositionX = sidebarOffset + componentWidth / 2.0f;

    float objectHeaderHeight = 50.0f;
    float objectHeaderWidth = columnCount * componentWidth + (columnCount - 1) * xSpacing;
    float objectHeaderX = firstPositionX + (componentWidth + xSpacing) / 2.0f;
    float objectHeaderY = topOffset + objectHeaderHeight / 2.0f;

     float navY = WINDOW_HEIGHT - BOTTOM_BAR_HEIGHT - 20.0f;
     float bottomLimit = navY - 30.0f;
     float addButtonHeight = 40.0f;

    addRectangle("ObjectHeader_Box", {objectHeaderX, objectHeaderY}, objectHeaderWidth, objectHeaderHeight, BUTTON_COLOR4, 1);
    _componentsRectangleNames.push_back("ObjectHeader_Box");

    addButton("ObjectHeader_Name", {objectHeaderX, objectHeaderY}, objectHeaderWidth - 300.0f, objectHeaderHeight - 10.0f, 30,
              nullptr, BUTTON_COLOR4, BUTTON_COLOR4, TEXT_COLOR, getObjectName(objectType, objectId));
    _componentsButtonNames.push_back("ObjectHeader_Name");

    addButton("ObjectHeader_Rename", {objectHeaderX - objectHeaderWidth / 2.0f + 80.0f, objectHeaderY}, 120.0f, 36.0f, 20,
              [this]() { editObject(_selectedObjectType, _selectedObjectId); },
              BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, "Rename");
    _componentsButtonNames.push_back("ObjectHeader_Rename");

    topOffset += objectHeaderHeight + ySpacing;

    std::vector<int> pageOf(components.size());
    std::vector<int> columnOf(components.size());
    std::vector<float> topOf(components.size());

    std::vector<float> columnTops(columnCount, topOffset);
    int pageCount = 1;
    int countOnPage = 0;

    for (int index = 0; index < components.size(); ++index) {
        float componentHeight = headerHeight + components[index].properties.size() * propertyRowHeight;
        int column = countOnPage % columnCount;

        if (columnTops[column] + componentHeight > bottomLimit && countOnPage > 0) {
            pageCount++;
            countOnPage = 0;
            column = 0;
            std::fill(columnTops.begin(), columnTops.end(), topOffset);
        }

        pageOf[index] = pageCount - 1;
        columnOf[index] = column;
        topOf[index] = columnTops[column];

        columnTops[column] += componentHeight + ySpacing;
        countOnPage++;
    }

    int shortestColumn = 0;
    for (int column = 1; column < columnCount; ++column) {
        if (columnTops[column] < columnTops[shortestColumn])
            shortestColumn = column;
    }

    if (columnTops[shortestColumn] + addButtonHeight > bottomLimit) {
        pageCount++;
        shortestColumn = 0;
        std::fill(columnTops.begin(), columnTops.end(), topOffset);
    }

    _currentComponentPage = std::clamp(_currentComponentPage, 0, static_cast<int>(pageCount) - 1);
     int currentPage = static_cast<int>(_currentComponentPage);

    for (int index = 0; index < components.size(); ++index) {
        if (pageOf[index] != currentPage)
            continue;

         Component &component = components[index];

        float positionX = firstPositionX + columnOf[index] * (componentWidth + xSpacing);
        float headerPositionY = topOf[index] + headerHeight / 2.0f;
        float firstPropertyPositionY = headerPositionY + headerHeight / 2.0f + propertyRowHeight / 2.0f;

        std::string prefix = component.name + "_" + std::to_string(index);
        std::string headerName = prefix + "_Header";
        std::string componentTitle = prefix + "_Title";

        addRectangle(headerName, {positionX, headerPositionY}, componentWidth, headerHeight, component.color, 1);
        _componentsRectangleNames.push_back(headerName);

        addText(componentTitle, {positionX, headerPositionY}, 24, component.name, TEXT_COLOR);
        _componentsTextNames.push_back(componentTitle);

         std::string deleteName = prefix + "_Delete";
        addButton(deleteName, {positionX + componentWidth / 2.0f - 25.0f, headerPositionY}, 30.0f, 30.0f, 18,
                  [this, index]() { deleteComponent(index); },
                  BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR, "X");
        _componentsButtonNames.push_back(deleteName);

        createProperties(component, index, positionX, firstPropertyPositionY, componentWidth, objectType, objectId);
    }

    if (currentPage == pageCount - 1) {
        _addButtonPositionX = firstPositionX + shortestColumn * (componentWidth + xSpacing);
        _addButtonPositionY = columnTops[shortestColumn] + addButtonHeight / 2.0f;

        addButton("AddComponent", {_addButtonPositionX, _addButtonPositionY}, 60.0f, addButtonHeight, 28,
                  [this]() { openComponentPicker(); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
        _componentsButtonNames.push_back("AddComponent");
    }

    addButton("DeleteObject", {objectHeaderX + objectHeaderWidth / 2.0f - 70.0f, navY}, 140.0f, 30.0f, 18,
              [this]() {
                  if (!_deleteObject) {
                      _deleteObject = true;
                      setButtonText("DeleteObject", "Confirm?");
                  } else {
                      deleteSelectedObject();
                  }
              },
              ColorRGBA{150, 40, 40, 255}, ColorRGBA{190, 60, 60, 255}, TEXT_COLOR, "Delete");
    _componentsButtonNames.push_back("DeleteObject");


    if (pageCount > 1) {
        addButton("ComponentPage_Previous", {objectHeaderX - 80.0f, navY}, 50.0f, 30.0f, 18,
                  [this]() { changeComponentPage(-1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "<");
        addButton("ComponentPage_Next", {objectHeaderX + 80.0f, navY}, 50.0f, 30.0f, 18,
                  [this]() { changeComponentPage(1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, ">");
        addText("ComponentPage_Label", {objectHeaderX, navY}, 20,
                std::to_string(currentPage + 1) + " / " + std::to_string(pageCount), TEXT_COLOR);

        _componentsButtonNames.push_back("ComponentPage_Previous");
        _componentsButtonNames.push_back("ComponentPage_Next");
        _componentsTextNames.push_back("ComponentPage_Label");
    }
}

void EditorMenu::changeComponentPage(int delta)
{
    applyCurrentEdit();
    _currentComponentPage += delta;
    createComponents(getObjectComponents(_selectedObjectType, _selectedObjectId),
                     _selectedObjectType, _selectedObjectId);
}

void EditorMenu::openComponentPicker()
{
    removeButton("AddComponent");

    std::vector<Component> &currentComponents = getObjectComponents(_selectedObjectType, _selectedObjectId);

    std::vector<Component> available;
    for ( Component &componentTemplate : _componentTemplates) {
        bool alreadyAdded = std::any_of(currentComponents.begin(), currentComponents.end(),
            [&componentTemplate]( Component &existing) { return existing.name == componentTemplate.name; });

        if (alreadyAdded && componentTemplate.name != "Script")
            continue;
        available.push_back(componentTemplate);
    }

    int columns = 3;
    int maxRows = 5;
     float buttonWidth = 150.0f;
     float buttonHeight = 30.0f;
     float gap = 8.0f;
     float padding = 12.0f;
     float cancelHeight = 30.0f;

    int count = std::min(static_cast<int>(available.size()), columns * maxRows);
    int rows = (count + columns - 1) / columns;

    float gridWidth = columns * buttonWidth + (columns - 1) * gap;
    float gridHeight = rows * buttonHeight + (rows > 0 ? (rows - 1) * gap : 0.0f);

    float panelWidth = gridWidth + 2 * padding;
    float panelHeight = padding + gridHeight + (rows > 0 ? gap : 0.0f) + cancelHeight + padding;

    float panelTop = _addButtonPositionY - 20.0f;
    float panelCenterY = panelTop + panelHeight / 2.0f;
    float gridLeft = _addButtonPositionX - gridWidth / 2.0f;

    addRectangle("ComponentPicker_Background", {_addButtonPositionX, panelCenterY},
                 panelWidth, panelHeight, BUTTON_COLOR3, 2);
    _componentsRectangleNames.push_back("ComponentPicker_Background");

    for (int i = 0; i < count; ++i) {
        int column = i % columns;
        int row = i / columns;

        float x = gridLeft + buttonWidth / 2.0f + column * (buttonWidth + gap);
        float y = panelTop + padding + buttonHeight / 2.0f + row * (buttonHeight + gap);

        std::string buttonName = "ComponentPicker_" + available[i].name;
        Component componentToAdd = available[i];

        addButton(buttonName, {x, y}, buttonWidth, buttonHeight, 18,
                  [this, componentToAdd]() { addComponentToSelectedObject(componentToAdd); },
                  BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, componentToAdd.name);

        _componentsButtonNames.push_back(buttonName);
        _pickerButtonNames.push_back(buttonName);
    }

    float cancelY = panelTop + padding + gridHeight + (rows > 0 ? gap : 0.0f) + cancelHeight / 2.0f;

    addButton("ComponentPicker_Cancel", {_addButtonPositionX, cancelY}, gridWidth, cancelHeight, 18,
              [this]() { closeComponentPicker(); },
              BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "Cancel");

    _componentsButtonNames.push_back("ComponentPicker_Cancel");
    _pickerButtonNames.push_back("ComponentPicker_Cancel");
}

void EditorMenu::closeComponentPicker()
{
    for ( std::string &buttonName : _pickerButtonNames) {
        removeButton(buttonName);
        _componentsButtonNames.erase(
            std::remove(_componentsButtonNames.begin(), _componentsButtonNames.end(), buttonName),
            _componentsButtonNames.end());
    }
    _pickerButtonNames.clear();

    removeRectangle("ComponentPicker_Background");
    _componentsRectangleNames.erase(
        std::remove(_componentsRectangleNames.begin(), _componentsRectangleNames.end(), "ComponentPicker_Background"),
        _componentsRectangleNames.end());

    addButton("AddComponent", {_addButtonPositionX, _addButtonPositionY}, 60.0f, 40.0f, 28,
              [this]() { openComponentPicker(); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
    _componentsButtonNames.push_back("AddComponent");
}

void EditorMenu::addComponentToSelectedObject(const Component &componentToAdd)
{
    std::vector<Component> &currentComponents = getObjectComponents(_selectedObjectType, _selectedObjectId);
    currentComponents.push_back(componentToAdd);
    _currentComponentPage = 9999;
    createComponents(currentComponents, _selectedObjectType, _selectedObjectId);
}

std::vector<Component> &EditorMenu::getObjectComponents(ObjectType objectType, int objectId)
{
    if (objectType == ObjectType::Entity) {
        auto it = std::find_if(_entities.begin(), _entities.end(),
                               [objectId]( Entity &e) { return e.id == objectId; });
        if (it != _entities.end())
            return it->components;
    } else {
        auto it = std::find_if(_prefabs.begin(), _prefabs.end(),
                               [objectId]( Prefab &p) { return p.id == objectId; });
        if (it != _prefabs.end())
            return it->components;
    }

    return _componentTemplates;
}

void EditorMenu::deleteComponent(int index)
{
    applyCurrentEdit();

    std::vector<Component> &components = getObjectComponents(_selectedObjectType, _selectedObjectId);

    if (index >= components.size())
        return;

    components.erase(components.begin() + index);

    createComponents(components, _selectedObjectType, _selectedObjectId);
}


//--------------------------------------------------------------------------------------------------------------------------------
// PROPERTIES MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------

void EditorMenu::createProperties( Component &component, int componentIndex, float positionX, float positionY, float componentWidth, ObjectType objectType, int objectId)
{
    float propertySpacing = 30.0f;
    float propertyFontSize = 24.0f;

    float buttonWidth = 200.0f;
    float buttonHeight = 30.0f;

    int propertyIndex = 0;
    std::string prefix = component.name + "_" + std::to_string(componentIndex) + "_";

    for ( auto &[propertyId, propertyName, propertyValue, propertyType] : component.properties) {

        std::string propertyTextName = prefix + propertyName + "_Text";
        std::string propertyValueName = prefix + propertyName + "_Value";
        std::string propertyRowName = prefix + propertyName + "_Row";

        Position buttonPosition = {positionX + 80.0f, positionY};

        valueType currentType = propertyType;
        ObjectType currentObjectType = objectType;
        int currentObjectId = objectId;
        std::string currentPropertyName = propertyName;
        std::string value = propertyValue;
        int id = propertyId;

        auto selectProperty = [this, propertyValueName, currentType, buttonPosition, propertyFontSize, currentObjectType, currentObjectId, componentIndex, currentPropertyName, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = getObjectComponents(currentObjectType, currentObjectId);

            _currentlyUpdatedProperty.name = propertyValueName;

            _currentlyUpdatedProperty.objectComponentIndex = componentIndex;
            _currentlyUpdatedProperty.objectProperty = currentPropertyName;

            _currentlyUpdatedProperty.type = currentType;
            _currentlyUpdatedProperty.text = components[componentIndex].properties[id].value;
            _currentlyUpdatedProperty.position = buttonPosition;
            _currentlyUpdatedProperty.fontSize = propertyFontSize;
            _currentlyUpdatedProperty.active = true;

            _currentlyUpdatedProperty.cursorIndex = _currentlyUpdatedProperty.text.size();
            _currentlyUpdatedProperty.blinkFrames = 0;
        };

        auto cycleShape = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex >= components.size() || id >= static_cast<int>(components[componentIndex].properties.size()))
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = nextShapeName(stored);
            setButtonText(propertyValueName, stored);

            auto it = _buttons.find(propertyValueName);
            if (it != _buttons.end())
                unSelectButton(it->second);
        };

        auto toggleBool = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex >= components.size() || id >= static_cast<int>(components[componentIndex].properties.size()))
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = (stored == "True") ? "False" : "True";
            setButtonText(propertyValueName, stored);

            auto it = _buttons.find(propertyValueName);
            if (it != _buttons.end())
                unSelectButton(it->second);
        };

        auto cycleColor = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex >= components.size() || id >= static_cast<int>(components[componentIndex].properties.size()))
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = nextColorName(stored);
            setButtonText(propertyValueName, stored);

            auto it = _buttons.find(propertyValueName);
            if (it != _buttons.end())
                unSelectButton(it->second);
        };

        

        std::function<void()> onClick = selectProperty;

         if (currentType == valueType::Shape)
            onClick = cycleShape;
        else if (currentType == valueType::Bool)
            onClick = toggleBool;
        else if (currentType == valueType::Color)
            onClick = cycleColor;


        ColorRGBA rowColor = (propertyIndex % 2 == 0) ? BUTTON_COLOR2 : BUTTON_COLOR1;

        addRectangle(propertyRowName, {positionX, positionY}, componentWidth, propertySpacing, rowColor, 1);
        addText(propertyTextName, {positionX - 120.0f, positionY}, propertyFontSize, propertyName, TEXT_COLOR);
        addButton(propertyValueName, {positionX + 80.0f, positionY}, buttonWidth, buttonHeight, 20,
                  onClick, rowColor, BUTTON_COLOR3, TEXT_COLOR, propertyValue);

        _componentsRectangleNames.push_back(propertyRowName);
        _componentsTextNames.push_back(propertyTextName);
        _componentsButtonNames.push_back(propertyValueName);

        positionY += propertySpacing;
        propertyIndex++;
    }
}

void EditorMenu::updateObjectProperty()
{
    if (_currentlyUpdatedProperty.editingName) {
        if (_selectedObjectType == ObjectType::Entity)
            editEntityName(_selectedObjectId, _currentlyUpdatedProperty.text);
        else
            editPrefabName(_selectedObjectId, _currentlyUpdatedProperty.text);
        return;
    }

    std::vector<Component> &components = getObjectComponents(_selectedObjectType, _selectedObjectId);
    int index = _currentlyUpdatedProperty.objectComponentIndex;

    if (index >= components.size())
        return;
    for (auto &[id, name, value, type] : components[index].properties) {
        if (name == _currentlyUpdatedProperty.objectProperty) {
            value = _currentlyUpdatedProperty.text;
            setButtonText(_currentlyUpdatedProperty.name, value);
            return;
        }
    }
}

void EditorMenu::manageInputs()
{
    if (_currentlyUpdatedProperty.active == false)
        return;

    std::string &currentContent = _currentlyUpdatedProperty.text;
    int &cursorIndex = _currentlyUpdatedProperty.cursorIndex;

    bool contentChanged = false;
    bool cursorMoved = false;

    int pressedCharacter = _renderer.getNextCharacter();

    while (pressedCharacter > 0) {
        bool isPrintable = pressedCharacter >= 32 && pressedCharacter <= 126;
        bool nameTooLong = _currentlyUpdatedProperty.editingName && currentContent.size() >= MAX_NAME_LENGTH;
        char character = static_cast<char>(pressedCharacter);

        if (isPrintable && !nameTooLong && isCharacterAllowed(character, _currentlyUpdatedProperty.type, currentContent, cursorIndex)) {
            currentContent.insert(cursorIndex, 1, character);
            cursorIndex++;
            contentChanged = true;
        }
        pressedCharacter = _renderer.getNextCharacter();
    }

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_BACKSPACE) && cursorIndex > 0) {
        currentContent.erase(cursorIndex - 1, 1);
        cursorIndex--;
        contentChanged = true;
    }

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_LEFT) && cursorIndex > 0) {
        cursorIndex--;
        cursorMoved = true;
    }

    if (_renderer.isKeyPressed(KeyboardKeys::KEY_RIGHT) && cursorIndex < currentContent.size()) {
        cursorIndex++;
        cursorMoved = true;
    }

    if (contentChanged) {
        setButtonText(_currentlyUpdatedProperty.name, currentContent);
        updateObjectProperty();
    }

    if (contentChanged || cursorMoved)
        _currentlyUpdatedProperty.blinkFrames = 0;
}

bool EditorMenu::isCharacterAllowed(char character, valueType type,  std::string &currentContent, int cursorIndex)
{
    if (type == valueType::String)
        return true;

    bool hasMinusSign = !currentContent.empty() && currentContent.front() == '-';

    if (hasMinusSign && cursorIndex == 0)
        return false;

    bool isDigit = character >= '0' && character <= '9';

    if (isDigit)
        return true;

    if (character == '-')
        return cursorIndex == 0;

    if (type == valueType::Float && character == '.')
        return currentContent.find('.') == std::string::npos;

    return false;
}

void EditorMenu::drawTextCursor()
{
    if (_currentlyUpdatedProperty.active == false)
        return;

    _currentlyUpdatedProperty.blinkFrames++;

    bool cursorVisible = (_currentlyUpdatedProperty.blinkFrames / CURSOR_BLINK_FRAMES) % 2 == 0;

    if (cursorVisible == false)
        return;

    auto buttonIterator = _buttons.find(_currentlyUpdatedProperty.name);

    if (buttonIterator == _buttons.end())
        return;

     Text &label = buttonIterator->second.text;

    Position fullTextSize = _renderer.getTextSize(label.text, label.fontSize);
    float textLeft = label.position.x - fullTextSize.x / 2.0f;

    std::string textBeforeCursor = label.text.substr(0, _currentlyUpdatedProperty.cursorIndex);
    Position textBeforeCursorSize = _renderer.getTextSize(textBeforeCursor, label.fontSize);
    float cursorX = textLeft + textBeforeCursorSize.x;

    _renderer.drawRectangle({cursorX, label.position.y}, CURSOR_WIDTH, label.fontSize, label.color);
}

std::string EditorMenu::nextShapeName(std::string current)
{
    for (int i = 0; i < _shapeOptions.size(); ++i) {
        if (_shapeOptions[i].second == current)
            return _shapeOptions[(i + 1) % _shapeOptions.size()].second;
    }
    return _shapeOptions.front().second;
}

std::string EditorMenu::nextColorName(std::string current)
{
    for (int i = 0; i < _colorOptions.size(); ++i) {
        if (_colorOptions[i].first == current)
            return _colorOptions[(i + 1) % _colorOptions.size()].first;
    }
    return _colorOptions.front().first;
}