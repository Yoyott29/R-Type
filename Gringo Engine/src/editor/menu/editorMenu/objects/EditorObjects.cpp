/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorObjects
*/

#include "EditorObjects.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

void EditorObjects::addNewEntity()
{
    _editor._properties.applyCurrentEdit();

    int newId = _editor._entities.empty() ? 1 : _editor._entities.back().id + 1;
    _editor._entities.push_back({newId, "Entity" + std::to_string(newId), {}});

    _editor._currentEntityPage = static_cast<int>((_editor._entities.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Entity);
}

void EditorObjects::addNewPrefab()
{
    _editor._properties.applyCurrentEdit();

    int newId = _editor._prefabs.empty() ? 1 : _editor._prefabs.back().id + 1;
    _editor._prefabs.push_back({newId, "Prefab" + std::to_string(newId), {}});

    _editor._currentPrefabPage = static_cast<int>((_editor._prefabs.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Prefab);
}

void EditorObjects::editEntityName(int entityId,  std::string &newName)
{
    auto it = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                           [entityId]( Entity &entity) { return entity.id == entityId; });

    if (it != _editor._entities.end()) {
        it->name = newName;
        _editor.setButtonText(getObjectButtonName(ObjectType::Entity, entityId), newName);

        if (_editor._selectedObjectType == ObjectType::Entity && _editor._selectedObjectId == entityId)
            _editor._components.updateObjectHeader();
    }
}

void EditorObjects::editPrefabName(int prefabId,  std::string &newName)
{
    auto it = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                           [prefabId]( Prefab &prefab) { return prefab.id == prefabId; });

    if (it != _editor._prefabs.end()) {
        it->name = newName;
        _editor.setButtonText(getObjectButtonName(ObjectType::Prefab, prefabId), newName);

        if (_editor._selectedObjectType == ObjectType::Prefab && _editor._selectedObjectId == prefabId)
            _editor._components.updateObjectHeader();
    }
}

void EditorObjects::editObject(ObjectType objectType, int objectId)
{
    _editor._properties.applyCurrentEdit();

    _editor._currentlyUpdatedProperty.name = "ObjectHeader_Name";
    _editor._currentlyUpdatedProperty.text = getObjectName(objectType, objectId);
    _editor._currentlyUpdatedProperty.type = valueType::String;
    _editor._currentlyUpdatedProperty.editingName = true;
    _editor._currentlyUpdatedProperty.cursorIndex = static_cast<int>(_editor._currentlyUpdatedProperty.text.size());
    _editor._currentlyUpdatedProperty.blinkFrames = 0;
    _editor._currentlyUpdatedProperty.active = true;
}

std::string EditorObjects::getObjectName(ObjectType type, int id)
{
    if (type == ObjectType::Entity) {
        auto it = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                               [id]( Entity &e) { return e.id == id; });
        if (it != _editor._entities.end())
            return it->name;
    } else {
        auto it = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                               [id]( Prefab &p) { return p.id == id; });
        if (it != _editor._prefabs.end())
            return it->name;
    }
    return "";
}

std::string EditorObjects::getObjectButtonName(ObjectType type, int id)
{
    return (type == ObjectType::Entity ? "Entity_" : "Prefab_") + std::to_string(id);
}

int EditorObjects::getPageCount(ObjectType type)
{
     int total = (type == ObjectType::Entity) ? _editor._entities.size() : _editor._prefabs.size();
    return static_cast<int>(total / ITEMS_PER_PAGE) + 1;
}

void EditorObjects::changePage(ObjectType type, int delta)
{
     bool isEntity = (type == ObjectType::Entity);
     int page = isEntity ? _editor._currentEntityPage : _editor._currentPrefabPage;
     int target = std::clamp(page + delta, 0, getPageCount(type) - 1);

    if (target == page)
        return;

    _editor._properties.applyCurrentEdit();

    if (isEntity)
        _editor._currentEntityPage = target;
    else
        _editor._currentPrefabPage = target;

    refreshObjectList(type);
}

void EditorObjects::refreshObjectList(ObjectType type)
{
    bool isEntity = (type == ObjectType::Entity);
    int page = isEntity ? _editor._currentEntityPage : _editor._currentPrefabPage;
    std::vector<std::string> listNames = isEntity ? _editor._entityListNames : _editor._prefabListNames;

    float buttonHeight = 40.0f;
    float buttonSpacing = 16.0f;
    float firstRowY = static_cast<float>(TOP_BAR_HEIGHT + 100);
    float mainX = isEntity ? SIDEBAR_WIDTH / 2.0f : static_cast<float>(WINDOW_WIDTH) - SIDEBAR_WIDTH / 2.0f;
    float objectButtonWidth = SIDEBAR_WIDTH - 60.0f;

    for (std::size_t i = 0; i < listNames.size(); ++i)
        _editor.removeButton(listNames[i]);
    listNames.clear();

    std::vector<std::pair<int, std::string>> items;
    if (isEntity) {
        for (std::size_t i = 0; i < _editor._entities.size(); ++i)
            items.emplace_back(_editor._entities[i].id, _editor._entities[i].name);
    } else {
        for (std::size_t i = 0; i < _editor._prefabs.size(); ++i)
            items.emplace_back(_editor._prefabs[i].id, _editor._prefabs[i].name);
    }
    
    page = std::clamp(page, 0, getPageCount(type) - 1);
    if (isEntity)
        _editor._currentEntityPage = page;
    else
        _editor._currentPrefabPage = page;

     int start = static_cast<int>(page) * ITEMS_PER_PAGE;
     int end = std::min(start + ITEMS_PER_PAGE, static_cast<int>(items.size()));

     for (int i = start; i < end; ++i) {
         int id = items[i].first;
         float y = firstRowY + static_cast<float>(i - start) * (buttonHeight + buttonSpacing);
         std::string objectButton = getObjectButtonName(type, id);

        _editor.addButton(objectButton, {mainX, y}, objectButtonWidth, buttonHeight, 20,
                  [this, type, id]() {
                      _editor.closePluginPanel();
                      _editor._components.createComponents(
                          _editor._components.getObjectComponents(type, id), type, id);
                  },
                  BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, items[i].second);

        listNames.push_back(objectButton);
    }

    if (page == getPageCount(type) - 1) {
         std::string addName = isEntity ? "Add Entity" : "Add Prefab";
         float y = firstRowY + static_cast<float>(end - start) * (buttonHeight + buttonSpacing);

        _editor.addButton(addName, {mainX, y}, SIDEBAR_WIDTH / 4, buttonHeight / 2.0f, 18,
                  [this, isEntity]() { isEntity ? addNewEntity() : addNewPrefab(); },
                  BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
        listNames.push_back(addName);
    }

    if (isEntity)
        _editor._entityListNames = listNames;
    else
        _editor._prefabListNames = listNames;

     std::string pageTextName = isEntity ? "Entity Page" : "Prefab Page";
    _editor.removeText(pageTextName);
    _editor.addText(pageTextName,
            {mainX, static_cast<float>(WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20)}, 20,
            std::to_string(page + 1) + " / " + std::to_string(getPageCount(type)), TEXT_COLOR);

     std::string previousName = isEntity ? "Previous Entity Page" : "Previous Prefab Page";
     std::string nextName = isEntity ? "Next Entity Page" : "Next Prefab Page";

    if (_editor._buttons.count(previousName) > 0)
        _editor.unSelectButton(_editor._buttons[previousName]);
    if (_editor._buttons.count(nextName) > 0)
        _editor.unSelectButton(_editor._buttons[nextName]);
}

void EditorObjects::deleteSelectedObject()
{
    _editor._properties.applyCurrentEdit();

     ObjectType type = _editor._selectedObjectType;
     int id = _editor._selectedObjectId;

    _editor._components.clearDisplayedComponents();

    if (type == ObjectType::Entity) {
        _editor._entities.erase(std::remove_if(_editor._entities.begin(), _editor._entities.end(),
                        [id]( Entity &e) { return e.id == id; }), _editor._entities.end());
    } else {
        _editor._prefabs.erase(std::remove_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                       [id]( Prefab &p) { return p.id == id; }), _editor._prefabs.end());
    }

    _editor._selectedObjectId = -1;
    _editor._currentComponentPage = 0;

    refreshObjectList(type);
}

void EditorObjects::moveSelectedObject(int delta)
{
    _editor._properties.applyCurrentEdit();

    ObjectType type = _editor._selectedObjectType;
    int id = _editor._selectedObjectId;

    if (type == ObjectType::Entity) {
        auto entity = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                               [id](Entity &e) { return e.id == id; });
        int index = static_cast<int>(entity - _editor._entities.begin());
        int target = index + delta;

        if (entity != _editor._entities.end() && target >= 0 && target < static_cast<int>(_editor._entities.size())) {
            std::swap(_editor._entities[index], _editor._entities[target]);
            _editor._currentEntityPage = target / ITEMS_PER_PAGE;
        }
    } else if (type == ObjectType::Prefab) {
        auto prefab = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                               [id](Prefab &p) { return p.id == id; });
        int index = static_cast<int>(prefab - _editor._prefabs.begin());
        int target = index + delta;

        if (prefab != _editor._prefabs.end() && target >= 0 && target < static_cast<int>(_editor._prefabs.size())) {
            std::swap(_editor._prefabs[index], _editor._prefabs[target]);
            _editor._currentPrefabPage = target / ITEMS_PER_PAGE;
        }
    }

    refreshObjectList(type);

    for (std::string name : {"ObjectHeader_Up", "ObjectHeader_Down"}) {
        auto it = _editor._buttons.find(name);
        if (it != _editor._buttons.end())
            _editor.unSelectButton(it->second);
    }

    _movedButtonName = getObjectButtonName(type, id);
    _editor._pressedButtonName = _movedButtonName;
    _movedHighlightFrames = MOVE_HIGHLIGHT_FRAMES;
}

void EditorObjects::updateMoveHighlight()
{
    if (_movedHighlightFrames == 0)
        return;

    _movedHighlightFrames--;
    if (_movedHighlightFrames == 0 && _editor._pressedButtonName == _movedButtonName)
        _editor._pressedButtonName.clear();
}
