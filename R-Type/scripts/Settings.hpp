#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "Scene.hpp"
#include "keyboard.hpp"
#include <cstddef>
#include <iterator>
#include <string>

// Resource: the settings panel is open (the lobby is hidden behind it)
struct SettingsOpen {
};

// Resource: the next key pressed is bound to this action
struct RebindRequest {
    std::size_t action;
};

namespace Settings
{
inline constexpr std::size_t KEY_COUNT = static_cast<std::size_t>(Key::Unknown);

// Labels of Key, in its order (AZERTY labels)
inline constexpr const char *KEY_NAMES[] = {"Z",
                                            "S",
                                            "Q",
                                            "D",
                                            "UP",
                                            "DOWN",
                                            "LEFT",
                                            "RIGHT",
                                            "SPACE",
                                            "LSHIFT",
                                            "E",
                                            "A",
                                            "B",
                                            "C",
                                            "F",
                                            "G",
                                            "H",
                                            "I",
                                            "J",
                                            "K",
                                            "L",
                                            "M",
                                            "N",
                                            "O",
                                            "P",
                                            "R",
                                            "T",
                                            "U",
                                            "V",
                                            "W",
                                            "X",
                                            "Y",
                                            "0",
                                            "1",
                                            "2",
                                            "3",
                                            "4",
                                            "5",
                                            "6",
                                            "7",
                                            "8",
                                            "9",
                                            "SUP 2",
                                            ")",
                                            "=",
                                            "^",
                                            "$",
                                            "U GRAVE",
                                            "*",
                                            ",",
                                            ";",
                                            ":",
                                            "!",
                                            "F1",
                                            "F2",
                                            "F3",
                                            "F4",
                                            "F5",
                                            "F6",
                                            "F7",
                                            "F8",
                                            "F9",
                                            "F10",
                                            "F11",
                                            "F12",
                                            "ESCAPE",
                                            "ENTER",
                                            "TAB",
                                            "BACKSPACE",
                                            "INSERT",
                                            "DELETE",
                                            "HOME",
                                            "END",
                                            "PAGE UP",
                                            "PAGE DOWN",
                                            "CAPS LOCK",
                                            "SCROLL LOCK",
                                            "NUM LOCK",
                                            "PRINT SCREEN",
                                            "PAUSE",
                                            "LCTRL",
                                            "LALT",
                                            "LSUPER",
                                            "RSHIFT",
                                            "RCTRL",
                                            "RALT",
                                            "RSUPER",
                                            "MENU",
                                            "PAD 0",
                                            "PAD 1",
                                            "PAD 2",
                                            "PAD 3",
                                            "PAD 4",
                                            "PAD 5",
                                            "PAD 6",
                                            "PAD 7",
                                            "PAD 8",
                                            "PAD 9",
                                            "PAD .",
                                            "PAD /",
                                            "PAD *",
                                            "PAD -",
                                            "PAD +",
                                            "PAD ENTER",
                                            "PAD ="};
static_assert(std::size(KEY_NAMES) == KEY_COUNT, "KEY_NAMES must have one label per Key");

inline std::string keyName(Key key)
{
    const auto index = static_cast<std::size_t>(key);
    return index < KEY_COUNT ? KEY_NAMES[index] : "?";
}

// Closes the panel and drops a rebind still waiting for its key
inline void close(Scene &scene)
{
    scene.removeResource<RebindRequest>();
    scene.removeResource<SettingsOpen>();
}
} // namespace Settings

#endif /* !SETTINGS_HPP_ */
