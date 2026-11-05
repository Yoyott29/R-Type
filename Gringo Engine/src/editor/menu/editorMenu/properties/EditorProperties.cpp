/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorProperties
*/

#include "EditorProperties.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
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
            _editor._currentlyUpdatedProperty.text =
                (_editor._selectedObjectType == ObjectType::Entity ? "Entity" : "Prefab") +
                std::to_string(_editor._selectedObjectId);
    } else if (type == valueType::Int || type == valueType::Float)
        currentText = cleanNumber(currentText);
    // A String can stay empty, e.g. a Button without text

    _editor.setButtonText(_editor._currentlyUpdatedProperty.name, currentText);
    updateObjectProperty();
    _editor._currentlyUpdatedProperty.active = false;
    _editor._currentlyUpdatedProperty.editingName = false;
}

void EditorProperties::createProperties(Component &component, int componentIndex, float positionX,
                                        float positionY, float componentWidth,
                                        ObjectType objectType, int objectId)
{
    float propertySpacing = 30.0f;
    float propertyFontSize = 24.0f;

    float buttonWidth = 200.0f;
    float buttonHeight = 30.0f;

    int propertyIndex = 0;
    std::string prefix = component.name + "_" + std::to_string(componentIndex) + "_";

    for (const Property &property : component.properties) {
        if (!isPropertyVisible(component, property))
            continue;
        const int propertyId = property.id;
        const std::string &propertyName = property.name;
        const std::string &propertyValue = property.value;
        const valueType propertyType = property.type;

        std::string propertyTextName = prefix + propertyName + "_Text";
        std::string propertyValueName = prefix + propertyName + "_Value";
        std::string propertyRowName = prefix + propertyName + "_Row";

        Vec2 buttonPosition = {positionX + 80.0f, positionY};

        std::function<void()> onClick =
            std::bind(&EditorProperties::selectProperty, this, propertyValueName, propertyType,
                      buttonPosition, propertyFontSize, objectType, objectId, componentIndex,
                      propertyName, propertyId);

        if (propertyType == valueType::Choice)
            onClick = std::bind(&EditorProperties::cycleChoice, this, propertyValueName, objectType,
                                objectId, componentIndex, propertyId);
        else if (propertyType == valueType::Bool)
            onClick = std::bind(&EditorProperties::toggleBool, this, propertyValueName, objectType,
                                objectId, componentIndex, propertyId);
        else if (propertyType == valueType::Color)
            onClick = std::bind(&EditorProperties::cycleColor, this, propertyValueName, objectType,
                                objectId, componentIndex, propertyId);
        else if (propertyType == valueType::File)
            onClick = std::bind(&EditorProperties::cycleFile, this, propertyValueName, objectType,
                                objectId, componentIndex, propertyId);

        ColorRGBA rowColor = (propertyIndex % 2 == 0) ? BUTTON_COLOR2 : BUTTON_COLOR1;

        _editor.addRectangle(propertyRowName, {positionX, positionY}, componentWidth,
                             propertySpacing, rowColor, 1);
        _editor.addText(propertyTextName, {positionX - 120.0f, positionY}, propertyFontSize,
                        propertyName, TEXT_COLOR);
        _editor.addButton(propertyValueName, {positionX + 80.0f, positionY}, buttonWidth,
                          buttonHeight, 20, onClick, rowColor, BUTTON_COLOR3, TEXT_COLOR,
                          propertyValue);

        _editor._componentsRectangleNames.push_back(propertyRowName);
        _editor._componentsTextNames.push_back(propertyTextName);
        _editor._componentsButtonNames.push_back(propertyValueName);

        positionY += propertySpacing;
        propertyIndex++;
    }
}

bool EditorProperties::isPropertyValid(ObjectType objectType, int objectId, int componentIndex,
                                       int propertyId)
{
    std::vector<Component> &components =
        _editor._components.getObjectComponents(objectType, objectId);

    if (componentIndex < 0 || componentIndex >= static_cast<int>(components.size()))
        return false;

    int propertyCount = static_cast<int>(components[componentIndex].properties.size());

    if (propertyId < 0 || propertyId >= propertyCount)
        return false;

    return true;
}

std::string &EditorProperties::getStoredValue(ObjectType objectType, int objectId,
                                              int componentIndex, int propertyId)
{
    std::vector<Component> &components =
        _editor._components.getObjectComponents(objectType, objectId);

    return components[componentIndex].properties[propertyId].value;
}

void EditorProperties::showNewValue(const std::string &propertyValueName,
                                    const std::string &newValue)
{
    _editor.setButtonText(propertyValueName, newValue);

    auto buttonIterator = _editor._buttons.find(propertyValueName);

    if (buttonIterator != _editor._buttons.end())
        _editor.unSelectButton(buttonIterator->second);
}

void EditorProperties::selectProperty(const std::string &propertyValueName, valueType type,
                                      Vec2 buttonPosition, float fontSize,
                                      ObjectType objectType, int objectId, int componentIndex,
                                      const std::string &propertyName, int propertyId)
{
    applyCurrentEdit();

    if (!isPropertyValid(objectType, objectId, componentIndex, propertyId))
        return;

    std::string &storedValue = getStoredValue(objectType, objectId, componentIndex, propertyId);

    _editor._currentlyUpdatedProperty.name = propertyValueName;
    _editor._currentlyUpdatedProperty.objectComponentIndex = componentIndex;
    _editor._currentlyUpdatedProperty.objectProperty = propertyName;

    _editor._currentlyUpdatedProperty.type = type;
    _editor._currentlyUpdatedProperty.position = buttonPosition;
    _editor._currentlyUpdatedProperty.fontSize = fontSize;

    _editor._currentlyUpdatedProperty.text = storedValue;
    _editor._currentlyUpdatedProperty.active = true;

    _editor._currentlyUpdatedProperty.cursorIndex =
        static_cast<int>(_editor._currentlyUpdatedProperty.text.size());
    _editor._currentlyUpdatedProperty.blinkFrames = 0;
}

void EditorProperties::cycleChoice(const std::string &propertyValueName, ObjectType objectType,
                                   int objectId, int componentIndex, int propertyId)
{
    applyCurrentEdit();

    if (!isPropertyValid(objectType, objectId, componentIndex, propertyId))
        return;

    Property &property = _editor._components.getObjectComponents(objectType, objectId)[componentIndex]
                             .properties[propertyId];
    property.value = nextOption(property.options, property.value);
    (void)propertyValueName;
    // Other properties can depend on this choice (Shape: Width/Height or Radius)
    _editor._components.createComponents(
        _editor._components.getObjectComponents(objectType, objectId), objectType, objectId);
}

bool EditorProperties::isPropertyVisible(const Component &component, const Property &property)
{
    if (property.visibleWhen.property.empty())
        return true;
    for (const Property &other : component.properties)
        if (other.name == property.visibleWhen.property)
            return other.value == property.visibleWhen.value;
    return true;
}

std::size_t EditorProperties::visiblePropertyCount(const Component &component)
{
    std::size_t count = 0;
    for (const Property &property : component.properties)
        if (isPropertyVisible(component, property))
            count++;
    return count;
}

void EditorProperties::toggleBool(const std::string &propertyValueName, ObjectType objectType,
                                  int objectId, int componentIndex, int propertyId)
{
    applyCurrentEdit();

    if (!isPropertyValid(objectType, objectId, componentIndex, propertyId))
        return;

    std::string &storedValue = getStoredValue(objectType, objectId, componentIndex, propertyId);

    storedValue = (storedValue == "True") ? "False" : "True";

    showNewValue(propertyValueName, storedValue);
}

void EditorProperties::cycleColor(const std::string &propertyValueName, ObjectType objectType,
                                  int objectId, int componentIndex, int propertyId)
{
    applyCurrentEdit();

    if (!isPropertyValid(objectType, objectId, componentIndex, propertyId))
        return;

    std::string &storedValue = getStoredValue(objectType, objectId, componentIndex, propertyId);

    storedValue = nextColorName(storedValue);

    showNewValue(propertyValueName, storedValue);
}

void EditorProperties::updateObjectProperty()
{
    if (_editor._currentlyUpdatedProperty.editingName) {
        if (_editor._selectedObjectType == ObjectType::Entity)
            _editor._objects.editEntityName(_editor._selectedObjectId,
                                            _editor._currentlyUpdatedProperty.text);
        else
            _editor._objects.editPrefabName(_editor._selectedObjectId,
                                            _editor._currentlyUpdatedProperty.text);
        return;
    }

    std::vector<Component> &components = _editor._components.getObjectComponents(
        _editor._selectedObjectType, _editor._selectedObjectId);
    int index = _editor._currentlyUpdatedProperty.objectComponentIndex;

    if (index < 0 || index >= static_cast<int>(components.size()))
        return;
    for (Property &property : components[index].properties) {
        if (property.name == _editor._currentlyUpdatedProperty.objectProperty) {
            property.value = _editor._currentlyUpdatedProperty.text;
            _editor.setButtonText(_editor._currentlyUpdatedProperty.name, property.value);
            // A new parent moves the entity's button in the list right away
            if (components[index].name == "ChildOf" &&
                _editor._selectedObjectType == ObjectType::Entity)
                _editor._objects.refreshObjectList(ObjectType::Entity);
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
        bool nameTooLong = _editor._currentlyUpdatedProperty.editingName &&
                           static_cast<int>(currentContent.size()) >= MAX_NAME_LENGTH;
        char character = static_cast<char>(pressedCharacter);

        if (isPrintable && !nameTooLong &&
            isCharacterAllowed(character, _editor._currentlyUpdatedProperty.type, currentContent,
                               cursorIndex)) {
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
        cursorIndex < static_cast<int>(currentContent.size())) {
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

bool EditorProperties::isCharacterAllowed(char character, valueType type,
                                          std::string &currentContent, int cursorIndex)
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

    bool cursorVisible =
        (_editor._currentlyUpdatedProperty.blinkFrames / CURSOR_BLINK_FRAMES) % 2 == 0;

    if (cursorVisible == false)
        return;

    auto buttonIterator = _editor._buttons.find(_editor._currentlyUpdatedProperty.name);

    if (buttonIterator == _editor._buttons.end())
        return;

    Text &label = buttonIterator->second.text;

    Vec2 fullTextSize = _editor._renderer.getTextSize(label.text, label.fontSize);
    float textLeft = label.position.x - fullTextSize.x / 2.0f;

    std::string textBeforeCursor =
        label.text.substr(0, _editor._currentlyUpdatedProperty.cursorIndex);
    Vec2 textBeforeCursorSize = _editor._renderer.getTextSize(textBeforeCursor, label.fontSize);
    float cursorX = textLeft + textBeforeCursorSize.x;

    _editor._renderer.drawRectangle({cursorX, label.position.y}, CURSOR_WIDTH, label.fontSize,
                                    label.color);
}

std::string EditorProperties::nextOption(const std::vector<std::string> &options,
                                         const std::string &current)
{
    if (options.empty())
        return current;
    auto iterator = std::find(options.begin(), options.end(), current);
    if (iterator == options.end() || std::next(iterator) == options.end())
        return options.front();
    return *std::next(iterator);
}

std::string EditorProperties::nextColorName(std::string current)
{
    int colorCount = static_cast<int>(_colorOptions.size());

    for (int i = 0; i < colorCount; ++i) {
        if (_colorOptions[i].first == current)
            return _colorOptions[(i + 1) % colorCount].first;
    }
    return _colorOptions.front().first;
}

std::vector<std::string> EditorProperties::getProjectFiles(const std::vector<std::string> &options)
{
    std::vector<std::string> files;
    if (options.size() >= 2) {
        const std::filesystem::path folder =
            Parser::getProjectFolder(_editor._projectFilePath, _editor._projectName) / options[0];
        std::error_code error;
        for (const auto &entry : std::filesystem::directory_iterator(folder, error))
            if (entry.is_regular_file() &&
                std::find(options.begin() + 1, options.end(), entry.path().extension()) !=
                    options.end())
                files.push_back(entry.path().filename().string());
        std::sort(files.begin(), files.end());
    }
    files.insert(files.begin(), NO_FILE);
    return files;
}

void EditorProperties::cycleFile(const std::string &, ObjectType objectType, int objectId,
                                 int componentIndex, int propertyId)
{
    applyCurrentEdit();

    if (!isPropertyValid(objectType, objectId, componentIndex, propertyId))
        return;

    Component &component =
        _editor._components.getObjectComponents(objectType, objectId)[componentIndex];
    Property &property = component.properties[propertyId];
    property.value = nextOption(getProjectFiles(property.options), property.value);
    // A script file can declare parameters: they become properties of its component (Script,
    // or Button for its button script)
    if (isScriptFile(property))
        applyScriptParameters(component, property.value);

    _editor._components.createComponents(
        _editor._components.getObjectComponents(objectType, objectId), objectType, objectId);
}

bool EditorProperties::isScriptFile(const Property &property)
{
    return property.type == valueType::File && !property.options.empty() &&
           property.options.front() == "scripts";
}

std::size_t EditorProperties::ownPropertyCount(const Component &component) const
{
    for (const Component &componentTemplate : _editor._componentTemplates)
        if (componentTemplate.name == component.name)
            return std::min(componentTemplate.properties.size(), component.properties.size());
    return component.properties.size();
}

void EditorProperties::applyScriptParameters(Component &component, const std::string &scriptName)
{
    // Keeps the component's own properties, drops the parameters of the previous script
    component.properties.erase(
        component.properties.begin() + static_cast<std::ptrdiff_t>(ownPropertyCount(component)),
        component.properties.end());

    if (scriptName == NO_FILE)
        return;

    std::filesystem::path scriptFile =
        Parser::getProjectFolder(_editor._projectFilePath, _editor._projectName) / "scripts" /
        scriptName;

    for (Property &parameter : Parser::readScriptParameters(scriptFile)) {
        parameter.id = static_cast<int>(component.properties.size());
        component.properties.push_back(std::move(parameter));
    }
}
