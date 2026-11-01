/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorObjects
*/

#ifndef EDITOROBJECTS_HPP_
#define EDITOROBJECTS_HPP_

#define MOVE_HIGHLIGHT_FRAMES 20

#include <string>
#include <vector>
#include "Structs.hpp"

class EditorMenu;
enum class ObjectType;

class EditorObjects
{
  public:
    EditorObjects(EditorMenu &editor) : _editor(editor) {}

    //ADDING AND REMOVING ENTITIES AND PREFABS

    void addNewEntity();
    void addNewPrefab();
    void editEntityName(int entityId,  std::string &newName);
    void editPrefabName(int prefabId,  std::string &newName);
    void editObject(ObjectType objectType, int objectId);

    std::string getObjectName(ObjectType type, int id);
    std::string getObjectButtonName(ObjectType type, int id);

    void refreshObjectList(ObjectType type);
    void changePage(ObjectType type, int delta);
    int getPageCount(ObjectType type);

    void deleteSelectedObject();
    void moveSelectedObject(int delta);
    void updateMoveHighlight();

  private:
      EditorMenu &_editor;
      std::string _movedButtonName;
      int _movedHighlightFrames = 0;
};

#endif /* !EDITOROBJECTS_HPP_ */
