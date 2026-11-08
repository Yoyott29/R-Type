/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorObjects
*/

#ifndef EDITOROBJECTS_HPP_
#define EDITOROBJECTS_HPP_

#define MOVE_HIGHLIGHT_FRAMES 20

#include "Structs.hpp"
#include <string>
#include <vector>

class EditorMenu;
enum class ObjectType;

class EditorObjects
{
  public:
    EditorObjects(EditorMenu &editor) : _editor(editor) {}

    // ADDING AND REMOVING ENTITIES AND PREFABS

    void addNewEntity();
    void addNewPrefab();
    void editEntityName(int entityId, std::string &newName);
    void editPrefabName(int prefabId, std::string &newName);
    void editObject(ObjectType objectType, int objectId);

    std::string getObjectName(ObjectType type, int id);
    std::string getObjectButtonName(ObjectType type, int id);

    void refreshObjectList(ObjectType type);
    void changePage(ObjectType type, int delta);
    int getPageCount(ObjectType type);

    // Deleting an entity also deletes its children (ChildOf), and theirs
    void deleteSelectedObject();
    void moveSelectedObject(int delta);
    void updateMoveHighlight();

  private:
    EditorMenu &_editor;
    // The parent id written in an entity's ChildOf, -1 if it has none
    static int parentIdOf(const Entity &entity);
    // id and every entity under it
    std::vector<int> familyOf(int id) const;

    // One button of the entity or prefab list; depth = number of parents above it
    struct ListItem {
        int id;
        std::string name;
        int depth;
    };
    // Entities ordered as a tree: every child right under its parent
    std::vector<ListItem> entityTree() const;
    // How much a child's button is shifted and shortened per level, up to MAX_TREE_INDENTS
    static inline const float TREE_TAB = 30.0f;
    static inline const int MAX_TREE_INDENTS = 4;
    std::string _movedButtonName;
    int _movedHighlightFrames = 0;
};

#endif /* !EDITOROBJECTS_HPP_ */
