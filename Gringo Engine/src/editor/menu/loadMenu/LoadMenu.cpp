/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** LoadMenu
*/

#include "LoadMenu.hpp"

LoadMenu::LoadMenu(IEditorRenderer &renderer, Parser &parser) : AMenu(renderer), _parser(parser)
{
    _currentMenu = MenuType::LoadMenu;

    _savedFiles = _parser.getSavedFiles();
    std::sort(_savedFiles.begin(), _savedFiles.end());
    LoadMenu::initTexts();
    LoadMenu::initButtons();
    LoadMenu::initRectangles();
}

LoadMenu::~LoadMenu() {}

void LoadMenu::initTexts()
{
    addText("Title", {960, 200}, 60, "Choose a project to load", TEXT_COLOR);
}

void LoadMenu::initButtons()
{
    float button_x = 450;
    float button_y = 400;
    int button_width = 300;
    int button_height = 100;
    int button_spacing = 20;
    int columns = 4;
    int button_count = 0;
    addButton(
        "Back", {1800, 50}, 200, 50, 20, [this]() { _switchMenu = MenuType::MainMenu; },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    for (const auto &file : _savedFiles) {
        std::filesystem::path projectPath(file);
        std::string projectName = projectPath.stem().string();
        addButton(
            file, {button_x, button_y}, button_width, button_height, 20,
            [this, file]() {
                _sceneObjects = _parser.loadFile(file);
                _switchMenu = MenuType::EditorMenu;
            },
            BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR, projectName);

        button_x += button_width + button_spacing;
        button_count++;
        if (button_count % columns == 0) {
            button_x = 450;
            button_y += button_height + button_spacing;
        }
    }
}

void LoadMenu::initRectangles() {}
