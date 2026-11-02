/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorProperties
*/

#include "EditorProperties.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <utility>

std::string EditorProperties::cleanNumber(std::string text)
{
    if (!text.empty() && text.back() == '.')
        text.pop_back();

    bool hasMinusSign = !text.empty() && text.front() == '-';
    int digitsStart = hasMinusSign ? 1 : 0;

    if (static_cast<int>(text.size()) > digitsStart && text[digitsStart] == '.')
        text.insert(digitsStart, "0");

    if (text.empty() || text == "-")
        return "0";

    return text;
}

void EditorProperties::applyCurrentEdit()
{
    if (!_editor._currentlyUpdatedProperty.active)
        return;

    std::string &currentText = _editor._currentlyUpdatedProperty.text;
    valueType type = _editor._currentlyUpdatedProperty.type;

   
    if (_editor._currentlyUpdatedProperty.editingName) {
        if (currentText.empty())
            _editor._currentlyUpdatedProperty.text = (_editor._selectedObjectType == ObjectType::Entity ? "Entity" : "Prefab") + std::to_string(_editor._selectedObjectId);
    }
    else if (type == valueType::Int || type == valueType::Float) 
        currentText = cleanNumber(currentText);
    else if (currentText.empty())
        currentText = "0";

    _editor.setButtonText(_editor._currentlyUpdatedProperty.name, currentText);
    updateObjectProperty();
    _editor._currentlyUpdatedProperty.active = false;
    _editor._currentlyUpdatedProperty.editingName = false;
}

void EditorProperties::createProperties( Component &component, int componentIndex, float positionX, float positionY, float componentWidth, ObjectType objectType, int objectId)
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

            std::vector<Component> &components = _editor._components.getObjectComponents(currentObjectType, currentObjectId);

            _editor._currentlyUpdatedProperty.name = propertyValueName;

            _editor._currentlyUpdatedProperty.objectComponentIndex = componentIndex;
            _editor._currentlyUpdatedProperty.objectProperty = currentPropertyName;

            _editor._currentlyUpdatedProperty.type = currentType;
            _editor._currentlyUpdatedProperty.text = components[componentIndex].properties[id].value;
            _editor._currentlyUpdatedProperty.position = buttonPosition;
            _editor._currentlyUpdatedProperty.fontSize = propertyFontSize;
            _editor._currentlyUpdatedProperty.active = true;

            _editor._currentlyUpdatedProperty.cursorIndex = _editor._currentlyUpdatedProperty.text.size();
            _editor._currentlyUpdatedProperty.blinkFrames = 0;
        };

        auto cycleShape = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = _editor._components.getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex < 0 || static_cast<std::size_t>(componentIndex) >= components.size() ||
                id < 0 || static_cast<std::size_t>(id) >= components[componentIndex].properties.size())
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = nextShapeName(stored);
            _editor.setButtonText(propertyValueName, stored);

            auto it = _editor._buttons.find(propertyValueName);
            if (it != _editor._buttons.end())
                _editor.unSelectButton(it->second);
        };

        auto toggleBool = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = _editor._components.getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex < 0 || static_cast<std::size_t>(componentIndex) >= components.size() ||
                id < 0 || static_cast<std::size_t>(id) >= components[componentIndex].properties.size())
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = (stored == "True") ? "False" : "True";
            _editor.setButtonText(propertyValueName, stored);

            auto it = _editor._buttons.find(propertyValueName);
            if (it != _editor._buttons.end())
                _editor.unSelectButton(it->second);
        };

        auto cycleColor = [this, propertyValueName, currentObjectType, currentObjectId, componentIndex, id]() {
            applyCurrentEdit();

            std::vector<Component> &components = _editor._components.getObjectComponents(currentObjectType, currentObjectId);
            if (componentIndex < 0 || static_cast<std::size_t>(componentIndex) >= components.size() ||
                id < 0 || static_cast<std::size_t>(id) >= components[componentIndex].properties.size())
                return;

            std::string &stored = components[componentIndex].properties[id].value;
            stored = nextColorName(stored);
            _editor.setButtonText(propertyValueName, stored);

            auto it = _editor._buttons.find(propertyValueName);
            if (it != _editor._buttons.end())
                _editor.unSelectButton(it->second);
        };

        

        std::function<void()> onClick = selectProperty;

         if (currentType == valueType::Shape)
            onClick = cycleShape;
        else if (currentType == valueType::Bool)
            onClick = toggleBool;
        else if (currentType == valueType::Color)
            onClick = cycleColor;


        ColorRGBA rowColor = (propertyIndex % 2 == 0) ? BUTTON_COLOR2 : BUTTON_COLOR1;

        _editor.addRectangle(propertyRowName, {positionX, positionY}, componentWidth, propertySpacing, rowColor, 1);
        _editor.addText(propertyTextName, {positionX - 120.0f, positionY}, propertyFontSize, propertyName, TEXT_COLOR);
        _editor.addButton(propertyValueName, {positionX + 80.0f, positionY}, buttonWidth, buttonHeight, 20,
                  onClick, rowColor, BUTTON_COLOR3, TEXT_COLOR, propertyValue);

        _editor._componentsRectangleNames.push_back(propertyRowName);
        _editor._componentsTextNames.push_back(propertyTextName);
        _editor._componentsButtonNames.push_back(propertyValueName);

        positionY += propertySpacing;
        propertyIndex++;
    }
}

void EditorProperties::updateObjectProperty()
{
    if (_editor._currentlyUpdatedProperty.editingName) {
        if (_editor._selectedObjectType == ObjectType::Entity)
            _editor._objects.editEntityName(_editor._selectedObjectId, _editor._currentlyUpdatedProperty.text);
        else
            _editor._objects.editPrefabName(_editor._selectedObjectId, _editor._currentlyUpdatedProperty.text);
        return;
    }

    std::vector<Component> &components = _editor._components.getObjectComponents(_editor._selectedObjectType, _editor._selectedObjectId);
    int index = _editor._currentlyUpdatedProperty.objectComponentIndex;

    if (index < 0 || static_cast<std::size_t>(index) >= components.size())
        return;
    for (auto &[id, name, value, type] : components[index].properties) {
        if (name == _editor._currentlyUpdatedProperty.objectProperty) {
            value = _editor._currentlyUpdatedProperty.text;
            _editor.setButtonText(_editor._currentlyUpdatedProperty.name, value);
            return;
        }
    }
}

void EditorProperties::manageInputs()
{
    if (_editor._currentlyUpdatedProperty.active == false)
        return;

    std::string &currentContent = _editor._currentlyUpdatedProperty.text;
    int &cursorIndex = _editor._currentlyUpdatedProperty.cursorIndex;

    bool contentChanged = false;
    bool cursorMoved = false;

    int pressedCharacter = _editor._renderer.getNextCharacter();

    while (pressedCharacter > 0) {
        bool isPrintable = pressedCharacter >= 32 && pressedCharacter <= 126;
        bool nameTooLong = _editor._currentlyUpdatedProperty.editingName && currentContent.size() >= MAX_NAME_LENGTH;
        char character = static_cast<char>(pressedCharacter);

        if (isPrintable && !nameTooLong && isCharacterAllowed(character, _editor._currentlyUpdatedProperty.type, currentContent, cursorIndex)) {
            currentContent.insert(cursorIndex, 1, character);
            cursorIndex++;
            contentChanged = true;
        }
        pressedCharacter = _editor._renderer.getNextCharacter();
    }

    if (_editor._renderer.isKeyPressed(KeyboardKeys::KEY_BACKSPACE) && cursorIndex > 0) {
        currentContent.erase(cursorIndex - 1, 1);
        cursorIndex--;
        contentChanged = true;
    }

    if (_editor._renderer.isKeyPressed(KeyboardKeys::KEY_LEFT) && cursorIndex > 0) {
        cursorIndex--;
        cursorMoved = true;
    }

    if (_editor._renderer.isKeyPressed(KeyboardKeys::KEY_RIGHT) &&
        static_cast<std::size_t>(cursorIndex) < currentContent.size()) {
        cursorIndex++;
        cursorMoved = true;
    }

    if (contentChanged) {
        _editor.setButtonText(_editor._currentlyUpdatedProperty.name, currentContent);
        updateObjectProperty();
    }

    if (contentChanged || cursorMoved)
        _editor._currentlyUpdatedProperty.blinkFrames = 0;
}

bool EditorProperties::isCharacterAllowed(char character, valueType type,  std::string &currentContent, int cursorIndex)
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

void EditorProperties::drawTextCursor()
{
    if (_editor._currentlyUpdatedProperty.active == false)
        return;

    _editor._currentlyUpdatedProperty.blinkFrames++;

    bool cursorVisible = (_editor._currentlyUpdatedProperty.blinkFrames / CURSOR_BLINK_FRAMES) % 2 == 0;

    if (cursorVisible == false)
        return;

    auto buttonIterator = _editor._buttons.find(_editor._currentlyUpdatedProperty.name);

    if (buttonIterator == _editor._buttons.end())
        return;

     Text &label = buttonIterator->second.text;

    Position fullTextSize = _editor._renderer.getTextSize(label.text, label.fontSize);
    float textLeft = label.position.x - fullTextSize.x / 2.0f;

    std::string textBeforeCursor = label.text.substr(0, _editor._currentlyUpdatedProperty.cursorIndex);
    Position textBeforeCursorSize = _editor._renderer.getTextSize(textBeforeCursor, label.fontSize);
    float cursorX = textLeft + textBeforeCursorSize.x;

    _editor._renderer.drawRectangle({cursorX, label.position.y}, CURSOR_WIDTH, label.fontSize, label.color);
}

std::string EditorProperties::nextShapeName(std::string current)
{
    for (std::size_t i = 0; i < _shapeOptions.size(); ++i) {
        if (_shapeOptions[i].second == current)
            return _shapeOptions[(i + 1) % _shapeOptions.size()].second;
    }
    return _shapeOptions.front().second;
}

std::string EditorProperties::nextColorName(std::string current)
{
    for (std::size_t i = 0; i < _colorOptions.size(); ++i) {
        if (_colorOptions[i].first == current)
            return _colorOptions[(i + 1) % _colorOptions.size()].first;
    }
    return _colorOptions.front().first;
}
