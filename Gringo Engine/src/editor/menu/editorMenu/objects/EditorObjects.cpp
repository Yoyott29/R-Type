/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorObjects
*/

#include "EditorObjects.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <functional>
#include <utility>

void EditorObjects::addNewEntity()
{
    _editor._properties.applyCurrentEdit();

    // Biggest id + 1: entities can be reordered, the last one is not always the biggest, and
    // ChildOf needs every id to be unique
    int newId = 1;
    for (const Entity &entity : _editor._entities)
        newId = std::max(newId, entity.id + 1);
    _editor._entities.push_back({newId,
                                 "Entity" + std::to_string(newId),
                                 {_editor.createComponent(EditorMenu::REQUIRED_COMPONENT)}});

    _editor._currentEntityPage = static_cast<int>((_editor._entities.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Entity);
}

void EditorObjects::addNewPrefab()
{
    _editor._properties.applyCurrentEdit();

    int newId = _editor._prefabs.empty() ? 1 : _editor._prefabs.back().id + 1;
    _editor._prefabs.push_back({newId,
                                "Prefab" + std::to_string(newId),
                                {_editor.createComponent(EditorMenu::REQUIRED_COMPONENT)}});

    _editor._currentPrefabPage = static_cast<int>((_editor._prefabs.size() - 1) / ITEMS_PER_PAGE);
    refreshObjectList(ObjectType::Prefab);
}

void EditorObjects::editEntityName(int entityId, std::string &newName)
{
    auto it = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                           [entityId](Entity &entity) { return entity.id == entityId; });

    if (it != _editor._entities.end()) {
        it->name = newName;
        _editor.setButtonText(getObjectButtonName(ObjectType::Entity, entityId), newName);

        if (_editor._selectedObjectType == ObjectType::Entity &&
            _editor._selectedObjectId == entityId)
            _editor._components.updateObjectHeader();
    }
}

void EditorObjects::editPrefabName(int prefabId, std::string &newName)
{
    auto it = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                           [prefabId](Prefab &prefab) { return prefab.id == prefabId; });

    if (it != _editor._prefabs.end()) {
        it->name = newName;
        _editor.setButtonText(getObjectButtonName(ObjectType::Prefab, prefabId), newName);

        if (_editor._selectedObjectType == ObjectType::Prefab &&
            _editor._selectedObjectId == prefabId)
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
    _editor._currentlyUpdatedProperty.commit = {};
    _editor._currentlyUpdatedProperty.cursorIndex =
        static_cast<int>(_editor._currentlyUpdatedProperty.text.size());
    _editor._currentlyUpdatedProperty.blinkFrames = 0;
    _editor._currentlyUpdatedProperty.active = true;
}

std::string EditorObjects::getObjectName(ObjectType type, int id)
{
    if (type == ObjectType::Entity) {
        auto it = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                               [id](Entity &e) { return e.id == id; });
        if (it != _editor._entities.end())
            return it->name;
    } else {
        auto it = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                               [id](Prefab &p) { return p.id == id; });
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
    std::vector<std::string> listNames =
        isEntity ? _editor._entityListNames : _editor._prefabListNames;

    float buttonHeight = 40.0f;
    float buttonSpacing = 16.0f;
    float firstRowY = static_cast<float>(TOP_BAR_HEIGHT + 100);
    float mainX =
        isEntity ? SIDEBAR_WIDTH / 2.0f : static_cast<float>(WINDOW_WIDTH) - SIDEBAR_WIDTH / 2.0f;
    float objectButtonWidth = SIDEBAR_WIDTH - 60.0f;

    for (std::size_t i = 0; i < listNames.size(); ++i)
        _editor.removeButton(listNames[i]);
    listNames.clear();

    // Entities as a tree: each child right under its parent, one tab further per level
    std::vector<ListItem> items;
    if (isEntity) {
        items = entityTree();
    } else {
        for (std::size_t i = 0; i < _editor._prefabs.size(); ++i)
            items.push_back({_editor._prefabs[i].id, _editor._prefabs[i].name, 0});
    }

    page = std::clamp(page, 0, getPageCount(type) - 1);
    if (isEntity)
        _editor._currentEntityPage = page;
    else
        _editor._currentPrefabPage = page;

    int start = static_cast<int>(page) * ITEMS_PER_PAGE;
    int end = std::min(start + ITEMS_PER_PAGE, static_cast<int>(items.size()));

    for (int i = start; i < end; ++i) {
        int id = items[i].id;
        float y = firstRowY + static_cast<float>(i - start) * (buttonHeight + buttonSpacing);
        std::string objectButton = getObjectButtonName(type, id);
        // A tab per level: the left edge moves right, the right edge stays
        const float indent =
            static_cast<float>(std::min(items[i].depth, MAX_TREE_INDENTS)) * TREE_TAB;

        _editor.addButton(
            objectButton, {mainX + indent / 2.0f, y}, objectButtonWidth - indent, buttonHeight, 20,
            [this, type, id]() {
                _editor.closePluginPanel();
                _editor._components.createComponents(
                    _editor._components.getObjectComponents(type, id), type, id);
            },
            BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, items[i].name);

        listNames.push_back(objectButton);
    }

    if (page == getPageCount(type) - 1) {
        std::string addName = isEntity ? "Add Entity" : "Add Prefab";
        float y = firstRowY + static_cast<float>(end - start) * (buttonHeight + buttonSpacing);

        _editor.addButton(
            addName, {mainX, y}, SIDEBAR_WIDTH / 4, buttonHeight / 2.0f, 18,
            [this, isEntity]() { isEntity ? addNewEntity() : addNewPrefab(); }, BUTTON_COLOR4,
            BUTTON_COLOR5, TEXT_COLOR, "+");
        listNames.push_back(addName);
    }

    if (isEntity)
        _editor._entityListNames = listNames;
    else
        _editor._prefabListNames = listNames;

    std::string pageTextName = isEntity ? "Entity Page" : "Prefab Page";
    _editor.removeText(pageTextName);
    _editor.addText(pageTextName, {mainX, static_cast<float>(WINDOW_HEIGHT - TOP_BAR_HEIGHT - 20)},
                    20, std::to_string(page + 1) + " / " + std::to_string(getPageCount(type)),
                    TEXT_COLOR);

    std::string previousName = isEntity ? "Previous Entity Page" : "Previous Prefab Page";
    std::string nextName = isEntity ? "Next Entity Page" : "Next Prefab Page";

    if (_editor._buttons.count(previousName) > 0)
        _editor.unSelectButton(_editor._buttons[previousName]);
    if (_editor._buttons.count(nextName) > 0)
        _editor.unSelectButton(_editor._buttons[nextName]);
}

int EditorObjects::parentIdOf(const Entity &entity)
{
    for (const Component &component : entity.components) {
        if (component.name != "ChildOf")
            continue;
        for (const Property &property : component.properties)
            if (property.name == "Parent") {
                try {
                    return std::stoi(property.value);
                } catch (const std::exception &) {
                    return -1;
                }
            }
    }
    return -1;
}

std::vector<EditorObjects::ListItem> EditorObjects::entityTree() const
{
    std::vector<ListItem> items;
    std::vector<int> placed;
    const auto isPlaced = [&placed](int id) {
        return std::find(placed.begin(), placed.end(), id) != placed.end();
    };
    const auto exists = [this](int id) {
        return std::any_of(_editor._entities.begin(), _editor._entities.end(),
                           [id](const Entity &entity) { return entity.id == id; });
    };
    // Adds an entity then, under it, its children in list order (and theirs)
    const std::function<void(const Entity &, int)> place = [&](const Entity &entity, int depth) {
        placed.push_back(entity.id);
        items.push_back({entity.id, entity.name, depth});
        for (const Entity &child : _editor._entities)
            if (parentIdOf(child) == entity.id && !isPlaced(child.id))
                place(child, depth + 1);
    };

    // Roots: no parent, or a parent that does not exist
    for (const Entity &entity : _editor._entities)
        if (!isPlaced(entity.id) && !exists(parentIdOf(entity)))
            place(entity, 0);
    // Entities in a ChildOf loop have no root: shown at the top level so they can be fixed
    for (const Entity &entity : _editor._entities)
        if (!isPlaced(entity.id))
            place(entity, 0);
    return items;
}

std::vector<int> EditorObjects::familyOf(int id) const
{
    std::vector<int> family = {id};
    // Grows while children of the entities already in it are found; each id once, so a ChildOf
    // loop cannot run forever
    for (std::size_t index = 0; index < family.size(); ++index)
        for (const Entity &entity : _editor._entities)
            if (parentIdOf(entity) == family[index] &&
                std::find(family.begin(), family.end(), entity.id) == family.end())
                family.push_back(entity.id);
    return family;
}

void EditorObjects::deleteSelectedObject()
{
    _editor._properties.applyCurrentEdit();

    ObjectType type = _editor._selectedObjectType;
    int id = _editor._selectedObjectId;

    _editor._components.clearDisplayedComponents();

    if (type == ObjectType::Entity) {
        const std::vector<int> family = familyOf(id);
        _editor._entities.erase(std::remove_if(_editor._entities.begin(), _editor._entities.end(),
                                               [&family](const Entity &e) {
                                                   return std::find(family.begin(), family.end(),
                                                                    e.id) != family.end();
                                               }),
                                _editor._entities.end());
    } else {
        _editor._prefabs.erase(std::remove_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                                              [id](Prefab &p) { return p.id == id; }),
                               _editor._prefabs.end());
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

        if (entity != _editor._entities.end() && target >= 0 &&
            target < static_cast<int>(_editor._entities.size())) {
            std::swap(_editor._entities[index], _editor._entities[target]);
            _editor._currentEntityPage = target / ITEMS_PER_PAGE;
        }
    } else if (type == ObjectType::Prefab) {
        auto prefab = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                                   [id](Prefab &p) { return p.id == id; });
        int index = static_cast<int>(prefab - _editor._prefabs.begin());
        int target = index + delta;

        if (prefab != _editor._prefabs.end() && target >= 0 &&
            target < static_cast<int>(_editor._prefabs.size())) {
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
