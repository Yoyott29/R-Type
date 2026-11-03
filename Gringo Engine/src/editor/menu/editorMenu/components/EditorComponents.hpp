/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorComponents
*/

#ifndef EDITORCOMPONENTS_HPP_
#define EDITORCOMPONENTS_HPP_

#include "Structs.hpp"
#include <string>
#include <vector>

class EditorMenu;
enum class ObjectType;

class EditorComponents
{
  public:
    EditorComponents(EditorMenu &editor) : _editor(editor) {}

    // COMPONENTS MANAGEMENT

    void updateObjectHeader();
    void changeComponentPage(int delta);
    ColorRGBA getComponentColor(const std::string &name);

    void clearDisplayedComponents();
    void createComponents(std::vector<Component> components, ObjectType objectType, int objectId);
    std::vector<Component> &getObjectComponents(ObjectType objectType, int objectId);
    void openComponentPicker();
    void closeComponentPicker();
    void addComponentToSelectedObject(const Component &componentToAdd);
    void deleteComponent(int index);
    // Redraws the entity list when a ChildOf was added or removed (its button moves in the tree)
    void refreshTreeIfChildOf(const std::string &componentName);

  private:
    EditorMenu &_editor;

    float _addButtonPositionX = 0.0f;
    float _addButtonPositionY = 0.0f;

    std::vector<std::string> _pickerButtonNames;

    bool _deleteObject = false;

};

#endif /* !EDITORCOMPONENTS_HPP_ */
