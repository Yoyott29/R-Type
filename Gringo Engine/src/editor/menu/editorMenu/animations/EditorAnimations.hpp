/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorAnimations
*/

#ifndef EDITORANIMATIONS_HPP_
#define EDITORANIMATIONS_HPP_

#include "Structs.hpp"
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

class EditorMenu;

// The Animation panel (top bar): a card like a component's, whose Name and Frames are the
// editor's input boxes. CREATE writes assets/<Name>.anim: one animation of a horizontal strip
// .png, each frame being the image's width / Frames x its height
class EditorAnimations
{
  public:
    EditorAnimations(EditorMenu &editor) : _editor(editor) {}

    // Opens the panel (called by EditorMenu::showPanel("Animation"))
    void show();
    // Removes the panel (called by EditorMenu::clearPanel)
    void clear();

  private:
    void redraw();
    void removeAll();
    // One row of the card: label on the left, value box on the right
    void addRow(float y, int index, const std::string &id, const std::string &label,
                const std::string &value, std::function<void()> onClick);
    // Starts typing in a value box, with the editor's text cursor
    void startTyping(const std::string &id, std::string &value, valueType type);
    void create();
    std::vector<std::string> pngFiles() const;
    std::filesystem::path assetsFolder() const;
    // Width and height from the PNG header, false if the file is not a PNG
    bool readPngSize(const std::filesystem::path &path, int &width, int &height) const;
    // Letters, digits, '_' and '-': the name is also the file's name
    bool isValidName(const std::string &name) const;

    EditorMenu &_editor;
    std::string _name;
    std::string _image;
    std::string _frames = "1";
    bool _loop = true;
    std::string _status;
    std::vector<std::string> _buttonNames;
    std::vector<std::string> _textNames;
    std::vector<std::string> _rectangleNames;
};

#endif /* !EDITORANIMATIONS_HPP_ */
