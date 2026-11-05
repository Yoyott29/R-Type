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

    // False while the property's visibleWhen condition is not met
    static bool isPropertyVisible(const Component &component, const Property &property);
    static std::size_t visiblePropertyCount(const Component &component);

    std::string nextColorName(std::string current);
    // The value after current in options, the first one if current is not there
    static std::string nextOption(const std::vector<std::string> &options,
                                  const std::string &current);
    // The project files of a File property: NO_FILE then <project>/<folder>/* with one of its
    // extensions
    std::vector<std::string> getProjectFiles(const std::vector<std::string> &options);
    // Replaces the parameters after the component's own properties by the ones scriptName
    // declares (<Script>_params)
    void applyScriptParameters(Component &component, const std::string &scriptName);
    // A File property of the scripts folder: Script.Path, Button.Script
    static bool isScriptFile(const Property &property);

    std::string cleanNumber(std::string text);
    void applyCurrentEdit();

  private:
    void selectProperty(const std::string &propertyValueName, valueType type,
                        Position buttonPosition, float fontSize, ObjectType objectType,
                        int objectId, int componentIndex, const std::string &propertyName,
                        int propertyId);
    void cycleChoice(const std::string &propertyValueName, ObjectType objectType, int objectId,
                     int componentIndex, int propertyId);
    void toggleBool(const std::string &propertyValueName, ObjectType objectType, int objectId,
                    int componentIndex, int propertyId);
    void cycleColor(const std::string &propertyValueName, ObjectType objectType, int objectId,
                    int componentIndex, int propertyId);
    void cycleFile(const std::string &propertyValueName, ObjectType objectType, int objectId,
                   int componentIndex, int propertyId);

    bool isPropertyValid(ObjectType objectType, int objectId, int componentIndex, int propertyId);
    std::string &getStoredValue(ObjectType objectType, int objectId, int componentIndex,
                                int propertyId);
    void showNewValue(const std::string &propertyValueName, const std::string &newValue);

    // Properties of the component's descriptor, the script parameters come after them
    std::size_t ownPropertyCount(const Component &component) const;

    EditorMenu &_editor;

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
