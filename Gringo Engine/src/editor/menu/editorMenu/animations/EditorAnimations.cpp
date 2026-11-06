/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** EditorAnimations
*/

#include "EditorAnimations.hpp"
#include "EditorMenu.hpp"
#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <nlohmann/json.hpp>

// Every animation created here plays at this speed: change "fps" in the .anim file for another
const int DEFAULT_FPS = 12;

// Same sizes as a component's card (EditorComponents, EditorProperties)
const float CARD_WIDTH = 500.0f;
const float HEADER_HEIGHT = 40.0f;
const float ROW_HEIGHT = 30.0f;
const float LABEL_FONT_SIZE = 24.0f;
const float VALUE_WIDTH = 200.0f;
const float VALUE_FONT_SIZE = 20.0f;
const float CARD_TOP = 160.0f;
const ColorRGBA HEADER_COLOR = {56, 51, 70, 255};
// Name of the panel's value boxes: "Animation_Value_Name", "Animation_Value_Frames"...
const std::string VALUE_PREFIX = "Animation_Value_";

void EditorAnimations::show()
{
    if (_image.empty()) {
        const std::vector<std::string> images = pngFiles();
        _image = images.empty() ? "" : images.front();
    }
    _status.clear();
    redraw();
}

void EditorAnimations::clear()
{
    // Typing in one of the panel's boxes: the text is kept before the boxes go away
    const EditingText &editing = _editor._currentlyUpdatedProperty;
    if (editing.active && editing.name.starts_with(VALUE_PREFIX))
        _editor._properties.applyCurrentEdit();
    removeAll();
}

void EditorAnimations::removeAll()
{
    for (const std::string &name : _buttonNames)
        _editor.removeButton(name);
    for (const std::string &name : _textNames)
        _editor.removeText(name);
    for (const std::string &name : _rectangleNames)
        _editor.removeRectangle(name);
    _buttonNames.clear();
    _textNames.clear();
    _rectangleNames.clear();
}

void EditorAnimations::redraw()
{
    // Keeps what is being typed, then rebuilds every box with the new values
    _editor._properties.applyCurrentEdit();
    removeAll();

    const float centerX = WINDOW_WIDTH / 2.0f;
    const float headerY = CARD_TOP + HEADER_HEIGHT / 2.0f;
    _editor.addRectangle("Animation_Header", {centerX, headerY}, CARD_WIDTH, HEADER_HEIGHT,
                         HEADER_COLOR, 1);
    _editor.addText("Animation_Title", {centerX, headerY}, 24, "New Animation", TEXT_COLOR);
    _rectangleNames.push_back("Animation_Header");
    _textNames.push_back("Animation_Title");

    float y = headerY + HEADER_HEIGHT / 2.0f + ROW_HEIGHT / 2.0f;
    addRow(y, 0, "Name", "Name", _name,
           [this]() { startTyping("Name", _name, valueType::String); });
    y += ROW_HEIGHT;
    addRow(y, 1, "Image", "Image", _image.empty() ? NO_FILE : _image, [this]() {
        _image = EditorProperties::nextOption(pngFiles(), _image);
        redraw();
    });
    y += ROW_HEIGHT;
    addRow(y, 2, "Frames", "Frames", _frames,
           [this]() { startTyping("Frames", _frames, valueType::Int); });
    y += ROW_HEIGHT;
    addRow(y, 3, "Loop", "Loop", _loop ? "True" : "False", [this]() {
        _loop = !_loop;
        redraw();
    });
    y += ROW_HEIGHT + 45.0f;

    _editor.addButton(
        "Animation_Create", {centerX, y}, 160, 40, 22, [this]() { create(); }, BUTTON_COLOR4,
        BUTTON_COLOR5, TEXT_COLOR, "CREATE");
    _buttonNames.push_back("Animation_Create");
    if (!_status.empty()) {
        _editor.addText("Animation_Status", {centerX, y + 50.0f}, 20, _status, TEXT_COLOR);
        _textNames.push_back("Animation_Status");
    }
}

void EditorAnimations::addRow(float y, int index, const std::string &id, const std::string &label,
                              const std::string &value, std::function<void()> onClick)
{
    const float centerX = WINDOW_WIDTH / 2.0f;
    const ColorRGBA rowColor = index % 2 == 0 ? BUTTON_COLOR2 : BUTTON_COLOR1;
    const std::string rowName = "Animation_Row_" + id;
    const std::string labelName = "Animation_Label_" + id;
    const std::string valueName = VALUE_PREFIX + id;

    _editor.addRectangle(rowName, {centerX, y}, CARD_WIDTH, ROW_HEIGHT, rowColor, 1);
    _editor.addText(labelName, {centerX - 120.0f, y}, LABEL_FONT_SIZE, label, TEXT_COLOR);
    _editor.addButton(valueName, {centerX + 80.0f, y}, VALUE_WIDTH, ROW_HEIGHT, VALUE_FONT_SIZE,
                      std::move(onClick), rowColor, BUTTON_COLOR3, TEXT_COLOR, value);
    _rectangleNames.push_back(rowName);
    _textNames.push_back(labelName);
    _buttonNames.push_back(valueName);
}

void EditorAnimations::startTyping(const std::string &id, std::string &value, valueType type)
{
    _editor._properties.applyCurrentEdit();
    EditingText &editing = _editor._currentlyUpdatedProperty;
    editing.name = VALUE_PREFIX + id;
    editing.type = type;
    editing.text = value;
    editing.commit = [&value](const std::string &text) { value = text; };
    editing.editingName = false;
    editing.cursorIndex = static_cast<int>(value.size());
    editing.blinkFrames = 0;
    editing.active = true;
}

void EditorAnimations::create()
{
    _editor._properties.applyCurrentEdit();
    const std::filesystem::path folder = assetsFolder();
    const std::filesystem::path file = folder / (_name + ".anim");
    int width = 0;
    int height = 0;
    int frames = 0;
    try {
        frames = std::stoi(_frames);
    } catch (const std::exception &) {
        frames = 0;
    }

    // Nothing is written (and no message shown) while a field is missing or invalid
    _status.clear();
    if (!_name.empty() && isValidName(_name) && !_image.empty() && _image != NO_FILE &&
        frames > 0 && readPngSize(folder / _image, width, height)) {
        // A width that does not divide by the frames gives fractional frames: they look off,
        // but the file is still written
        const double frameWidth = static_cast<double>(width) / frames;
        const nlohmann::json json = {{"texture", _image},     {"frameWidth", frameWidth},
                                     {"frameHeight", height}, {"frames", frames},
                                     {"fps", DEFAULT_FPS},    {"loop", _loop}};
        std::ofstream output(file);
        output << json.dump(4) << '\n';
        if (output)
            _status = "Animation created";
    }
    redraw();
}

std::vector<std::string> EditorAnimations::pngFiles() const
{
    std::vector<std::string> files;
    std::error_code error;
    for (const auto &entry : std::filesystem::directory_iterator(assetsFolder(), error))
        if (entry.is_regular_file() && entry.path().extension() == ".png")
            files.push_back(entry.path().filename().string());
    std::sort(files.begin(), files.end());
    return files;
}

std::filesystem::path EditorAnimations::assetsFolder() const
{
    return Parser::getProjectFolder(_editor._projectFilePath, _editor._projectName) / "assets";
}

bool EditorAnimations::isValidName(const std::string &name) const
{
    return std::all_of(name.begin(), name.end(), [](unsigned char character) {
        return std::isalnum(character) || character == '_' || character == '-';
    });
}

bool EditorAnimations::readPngSize(const std::filesystem::path &path, int &width, int &height) const
{
    // Signature (8 bytes), then the IHDR chunk: length (4), "IHDR" (4), width (4), height (4),
    // big endian
    std::array<unsigned char, 24> header{};
    std::ifstream file(path, std::ios::binary);
    if (!file.read(reinterpret_cast<char *>(header.data()), header.size()))
        return false;
    const std::array<unsigned char, 8> SIGNATURE = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    if (!std::equal(SIGNATURE.begin(), SIGNATURE.end(), header.begin()))
        return false;
    const auto readInt = [&header](std::size_t offset) {
        return (header[offset] << 24) | (header[offset + 1] << 16) | (header[offset + 2] << 8) |
               header[offset + 3];
    };
    width = readInt(16);
    height = readInt(20);
    return width > 0 && height > 0;
}
