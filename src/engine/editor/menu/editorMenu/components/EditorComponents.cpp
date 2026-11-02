/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorComponents
*/

#include "EditorComponents.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

void EditorComponents::updateObjectHeader()
{
    if (_editor._buttons.count("ObjectHeader_Name") == 0)
        return;

    _editor.setButtonText("ObjectHeader_Name", _editor._objects.getObjectName(_editor._selectedObjectType, _editor._selectedObjectId));
}

void EditorComponents::clearDisplayedComponents()
{
    _editor.removeText("PanelText");
    _editor._currentlyUpdatedProperty.active = false;
    _deleteObject = false;
    for ( std::string &textName : _editor._componentsTextNames)
        _editor.removeText(textName);
    for ( std::string &rectangleName : _editor._componentsRectangleNames)
        _editor.removeRectangle(rectangleName);
    for ( std::string &buttonName : _editor._componentsButtonNames)
        _editor.removeButton(buttonName);

    _editor._componentsTextNames.clear();
    _editor._componentsRectangleNames.clear();
    _editor._componentsButtonNames.clear();
    _pickerButtonNames.clear();
}

ColorRGBA EditorComponents::getComponentColor(const std::string &name)
{
    auto component = std::find_if(_editor._componentDescriptors.begin(), _editor._componentDescriptors.end(),
                                  [&name](const ComponentDescriptor &descriptor) {
                                      return descriptor.name == name;
                                  });

    return (component != _editor._componentDescriptors.end()) ? component->color : ColorRGBA{58, 57, 61, 255};
}

void EditorComponents::createComponents(std::vector<Component> components, ObjectType objectType, int objectId)
{
    _editor._properties.applyCurrentEdit();

    if (objectType != _editor._selectedObjectType || objectId != _editor._selectedObjectId)
        _editor._currentComponentPage = 0;

    _editor._selectedObjectType = objectType;
    _editor._selectedObjectId = objectId;

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

    _editor.addRectangle("ObjectHeader_Box", {objectHeaderX, objectHeaderY}, objectHeaderWidth, objectHeaderHeight, BUTTON_COLOR4, 1);
    _editor._componentsRectangleNames.push_back("ObjectHeader_Box");

    _editor.addButton("ObjectHeader_Name", {objectHeaderX, objectHeaderY}, objectHeaderWidth - 300.0f, objectHeaderHeight - 10.0f, 30,
              nullptr, BUTTON_COLOR4, BUTTON_COLOR4, TEXT_COLOR, _editor._objects.getObjectName(objectType, objectId));
    _editor._componentsButtonNames.push_back("ObjectHeader_Name");

    _editor.addButton("ObjectHeader_Rename", {objectHeaderX - objectHeaderWidth / 2.0f + 80.0f, objectHeaderY}, 120.0f, 36.0f, 20,
              [this]() { _editor._objects.editObject(_editor._selectedObjectType, _editor._selectedObjectId); },
              BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, "Rename");
    _editor._componentsButtonNames.push_back("ObjectHeader_Rename");

    _editor.addButton("ObjectHeader_Up", {objectHeaderX + objectHeaderWidth / 2.0f - 105.0f, objectHeaderY}, 60.0f, 36.0f, 18,
              [this]() { _editor._objects.moveSelectedObject(-1); },
              BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, "Up");
    _editor._componentsButtonNames.push_back("ObjectHeader_Up");

    _editor.addButton("ObjectHeader_Down", {objectHeaderX + objectHeaderWidth / 2.0f - 40.0f, objectHeaderY}, 60.0f, 36.0f, 18,
              [this]() { _editor._objects.moveSelectedObject(1); },
              BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, "Down");
    _editor._componentsButtonNames.push_back("ObjectHeader_Down");

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

    _editor._currentComponentPage = std::clamp(_editor._currentComponentPage, 0, static_cast<int>(pageCount) - 1);
     int currentPage = static_cast<int>(_editor._currentComponentPage);

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

        _editor.addRectangle(headerName, {positionX, headerPositionY}, componentWidth, headerHeight, component.color, 1);
        _editor._componentsRectangleNames.push_back(headerName);

        _editor.addText(componentTitle, {positionX, headerPositionY}, 24, component.name, TEXT_COLOR);
        _editor._componentsTextNames.push_back(componentTitle);

         std::string deleteName = prefix + "_Delete";
        _editor.addButton(deleteName, {positionX + componentWidth / 2.0f - 25.0f, headerPositionY}, 30.0f, 30.0f, 18,
                  [this, index]() { deleteComponent(index); },
                  BUTTON_COLOR2, BUTTON_COLOR3, TEXT_COLOR, "X");
        _editor._componentsButtonNames.push_back(deleteName);

        _editor._properties.createProperties(component, index, positionX, firstPropertyPositionY, componentWidth, objectType, objectId);
    }

    if (currentPage == pageCount - 1) {
        _addButtonPositionX = firstPositionX + shortestColumn * (componentWidth + xSpacing);
        _addButtonPositionY = columnTops[shortestColumn] + addButtonHeight / 2.0f;

        _editor.addButton("AddComponent", {_addButtonPositionX, _addButtonPositionY}, 60.0f, addButtonHeight, 28,
                  [this]() { openComponentPicker(); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
        _editor._componentsButtonNames.push_back("AddComponent");
    }

    _editor.addButton("DeleteObject", {objectHeaderX + objectHeaderWidth / 2.0f - 70.0f, navY}, 140.0f, 30.0f, 18,
              [this]() {
                  if (!_deleteObject) {
                      _deleteObject = true;
                      _editor.setButtonText("DeleteObject", "Confirm?");
                  } else {
                      _editor._objects.deleteSelectedObject();
                  }
              },
              ColorRGBA{150, 40, 40, 255}, ColorRGBA{190, 60, 60, 255}, TEXT_COLOR, "Delete");
    _editor._componentsButtonNames.push_back("DeleteObject");


    if (pageCount > 1) {
        _editor.addButton("ComponentPage_Previous", {objectHeaderX - 80.0f, navY}, 50.0f, 30.0f, 18,
                  [this]() { changeComponentPage(-1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "<");
        _editor.addButton("ComponentPage_Next", {objectHeaderX + 80.0f, navY}, 50.0f, 30.0f, 18,
                  [this]() { changeComponentPage(1); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, ">");
        _editor.addText("ComponentPage_Label", {objectHeaderX, navY}, 20,
                std::to_string(currentPage + 1) + " / " + std::to_string(pageCount), TEXT_COLOR);

        _editor._componentsButtonNames.push_back("ComponentPage_Previous");
        _editor._componentsButtonNames.push_back("ComponentPage_Next");
        _editor._componentsTextNames.push_back("ComponentPage_Label");
    }
}

void EditorComponents::changeComponentPage(int delta)
{
    _editor._properties.applyCurrentEdit();
    _editor._currentComponentPage += delta;
    createComponents(getObjectComponents(_editor._selectedObjectType, _editor._selectedObjectId),
                     _editor._selectedObjectType, _editor._selectedObjectId);
}

void EditorComponents::openComponentPicker()
{
    _editor.removeButton("AddComponent");

    std::vector<Component> &currentComponents = getObjectComponents(_editor._selectedObjectType, _editor._selectedObjectId);

    std::vector<Component> available;
    for ( Component &componentTemplate : _editor._componentTemplates) {
        const auto descriptor = std::find_if(
            _editor._componentDescriptors.begin(), _editor._componentDescriptors.end(),
            [&componentTemplate](const ComponentDescriptor &candidate) {
                return candidate.name == componentTemplate.name;
            });
        std::string requiredPlugin =
            descriptor != _editor._componentDescriptors.end() ? descriptor->plugin : "";
        if (requiredPlugin.empty()) {
            if (componentTemplate.name == "Velocity" || componentTemplate.name == "Collider2D")
                requiredPlugin = "Physics";
            else if (componentTemplate.name == "Sprite" ||
                     componentTemplate.name == "SoundEffect")
                requiredPlugin = "RayLibRenderer";
        }
        if (!requiredPlugin.empty() && !_editor.hasActivePlugin(requiredPlugin))
            continue;

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

    _editor.addRectangle("ComponentPicker_Background", {_addButtonPositionX, panelCenterY},
                 panelWidth, panelHeight, BUTTON_COLOR3, 2);
    _editor._componentsRectangleNames.push_back("ComponentPicker_Background");

    for (int i = 0; i < count; ++i) {
        int column = i % columns;
        int row = i / columns;

        float x = gridLeft + buttonWidth / 2.0f + column * (buttonWidth + gap);
        float y = panelTop + padding + buttonHeight / 2.0f + row * (buttonHeight + gap);

        std::string buttonName = "ComponentPicker_" + available[i].name;
        Component componentToAdd = available[i];

        _editor.addButton(buttonName, {x, y}, buttonWidth, buttonHeight, 18,
                  [this, componentToAdd]() { addComponentToSelectedObject(componentToAdd); },
                  BUTTON_COLOR1, BUTTON_COLOR2, TEXT_COLOR, componentToAdd.name);

        _editor._componentsButtonNames.push_back(buttonName);
        _pickerButtonNames.push_back(buttonName);
    }

    float cancelY = panelTop + padding + gridHeight + (rows > 0 ? gap : 0.0f) + cancelHeight / 2.0f;

    _editor.addButton("ComponentPicker_Cancel", {_addButtonPositionX, cancelY}, gridWidth, cancelHeight, 18,
              [this]() { closeComponentPicker(); },
              BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "Cancel");

    _editor._componentsButtonNames.push_back("ComponentPicker_Cancel");
    _pickerButtonNames.push_back("ComponentPicker_Cancel");
}

void EditorComponents::closeComponentPicker()
{
    for ( std::string &buttonName : _pickerButtonNames) {
        _editor.removeButton(buttonName);
        _editor._componentsButtonNames.erase(
            std::remove(_editor._componentsButtonNames.begin(), _editor._componentsButtonNames.end(), buttonName),
            _editor._componentsButtonNames.end());
    }
    _pickerButtonNames.clear();

    _editor.removeRectangle("ComponentPicker_Background");
    _editor._componentsRectangleNames.erase(
        std::remove(_editor._componentsRectangleNames.begin(), _editor._componentsRectangleNames.end(), "ComponentPicker_Background"),
        _editor._componentsRectangleNames.end());

    _editor.addButton("AddComponent", {_addButtonPositionX, _addButtonPositionY}, 60.0f, 40.0f, 28,
              [this]() { openComponentPicker(); }, BUTTON_COLOR4, BUTTON_COLOR5, TEXT_COLOR, "+");
    _editor._componentsButtonNames.push_back("AddComponent");
}

void EditorComponents::addComponentToSelectedObject(const Component &componentToAdd)
{
    std::vector<Component> &currentComponents = getObjectComponents(_editor._selectedObjectType, _editor._selectedObjectId);
    currentComponents.push_back(componentToAdd);
    _editor._currentComponentPage = 9999;
    createComponents(currentComponents, _editor._selectedObjectType, _editor._selectedObjectId);
}

std::vector<Component> &EditorComponents::getObjectComponents(ObjectType objectType, int objectId)
{
    if (objectType == ObjectType::Entity) {
        auto it = std::find_if(_editor._entities.begin(), _editor._entities.end(),
                               [objectId]( Entity &e) { return e.id == objectId; });
        if (it != _editor._entities.end())
            return it->components;
    } else {
        auto it = std::find_if(_editor._prefabs.begin(), _editor._prefabs.end(),
                               [objectId]( Prefab &p) { return p.id == objectId; });
        if (it != _editor._prefabs.end())
            return it->components;
    }

    return _editor._componentTemplates;
}

void EditorComponents::deleteComponent(int index)
{
    _editor._properties.applyCurrentEdit();

    std::vector<Component> &components = getObjectComponents(_editor._selectedObjectType, _editor._selectedObjectId);

    if (index >= components.size())
        return;

    components.erase(components.begin() + index);

    createComponents(components, _editor._selectedObjectType, _editor._selectedObjectId);
}
