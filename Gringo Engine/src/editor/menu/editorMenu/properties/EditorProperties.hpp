/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorProperties
*/

#ifndef EDITORPROPERTIES_HPP_
#define EDITORPROPERTIES_HPP_

#include "Structs.hpp"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class EditorMenu;
enum class ObjectType;

class EditorProperties
{
  public:
    EditorProperties(EditorMenu &editor) : _editor(editor) {}

    void createProperties(Component &component, int componentIndex, float positionX,
                          float positionY, float componentWidth, ObjectType objectType,
                          int objectId);
    void updateObjectProperty();
    void manageInputs();
    void drawTextCursor();
    bool isCharacterAllowed(char character, valueType type, std::string &currentContent,
                            int cursorIndex);

    std::string nextShapeName(std::string current);
    std::string nextColorName(std::string current);
    std::string nextScriptName(std::string current);
    std::vector<std::string> getScriptNames();
    void applyScriptParameters(Component &component, const std::string &scriptName);

    std::string cleanNumber(std::string text);
    void applyCurrentEdit();

  private:
    void selectProperty(const std::string &propertyValueName, valueType type,
                        Position buttonPosition, float fontSize, ObjectType objectType,
                        int objectId, int componentIndex, const std::string &propertyName,
                        int propertyId);
    void cycleShape(const std::string &propertyValueName, ObjectType objectType, int objectId,
                    int componentIndex, int propertyId);
    void toggleBool(const std::string &propertyValueName, ObjectType objectType, int objectId,
                    int componentIndex, int propertyId);
    void cycleColor(const std::string &propertyValueName, ObjectType objectType, int objectId,
                    int componentIndex, int propertyId);
    void cycleScript(const std::string &propertyValueName, ObjectType objectType, int objectId,
                     int componentIndex, int propertyId);

    bool isPropertyValid(ObjectType objectType, int objectId, int componentIndex, int propertyId);
    std::string &getStoredValue(ObjectType objectType, int objectId, int componentIndex,
                                int propertyId);
    void showNewValue(const std::string &propertyValueName, const std::string &newValue);

    EditorMenu &_editor;

    std::vector<std::pair<Shape, std::string>> _shapeOptions = {
        {Shape::BoxShape, "Box"},
        {Shape::CircleShape, "Circle"},
    };

    std::vector<std::pair<std::string, ColorRGBA>> _colorOptions = {
        {"Red", {255, 0, 0, 255}},       {"Orange", {255, 165, 0, 255}},
        {"Yellow", {255, 255, 0, 255}},  {"Lime", {0, 255, 0, 255}},
        {"Green", {0, 128, 0, 255}},     {"Cyan", {0, 255, 255, 255}},
        {"Blue", {0, 0, 255, 255}},      {"Purple", {128, 0, 128, 255}},
        {"Magenta", {255, 0, 255, 255}}, {"Pink", {255, 192, 203, 255}},
        {"Brown", {139, 69, 19, 255}},   {"White", {255, 255, 255, 255}},
        {"Gray", {128, 128, 128, 255}},  {"Black", {0, 0, 0, 255}},
        {"Transparent", {0, 0, 0, 0}},
    };
};

#endif /* !EDITORPROPERTIES_HPP_ */
