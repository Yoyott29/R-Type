/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorComponents
*/

#ifndef EDITORCOMPONENTS_HPP_
#define EDITORCOMPONENTS_HPP_

#include <map>
#include <string>
#include <vector>
#include "Structs.hpp"

class EditorMenu;
enum class ObjectType;

class EditorComponents
{
  public:
    EditorComponents(EditorMenu &editor) : _editor(editor) {}

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

  private:
    EditorMenu &_editor;

    float _addButtonPositionX = 0.0f;
    float _addButtonPositionY = 0.0f;

    std::vector<std::string> _pickerButtonNames;

    bool _deleteObject = false;

    std::map<std::string, ColorRGBA> _componentColors = {
        {"Position", {58, 53, 63, 255}},
        {"Velocity", {51, 59, 63, 255}},
        {"Sprite",   {51, 56, 70, 255}},
        {"Collider", {58, 57, 61, 255}},
        {"Script",   {56, 53, 69, 255}},
    };
};

#endif /* !EDITORCOMPONENTS_HPP_ */
