/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** keyboard
*/

#pragma once
#include <cstdint>

// Keys by their label on an AZERTY keyboard, wherever they are on the user's one: Z is the key
// left of E, Q the one left of S. New keys go before Unknown: KeyboardInput indexes a bitset with
// them, Unknown is the count
enum class Key : uint16_t {
    Z,
    S,
    Q,
    D,
    Up,
    Down,
    Left,
    Right,
    Space,
    LeftShift,
    E,
    A,
    B,
    C,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    R,
    T,
    U,
    V,
    W,
    X,
    Y,
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    Superscript2,
    RightParenthesis,
    Equal,
    Caret,
    Dollar,
    UGrave,
    Asterisk,
    Comma,
    Semicolon,
    Colon,
    Exclamation,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu,
    Keypad0,
    Keypad1,
    Keypad2,
    Keypad3,
    Keypad4,
    Keypad5,
    Keypad6,
    Keypad7,
    Keypad8,
    Keypad9,
    KeypadDecimal,
    KeypadDivide,
    KeypadMultiply,
    KeypadSubtract,
    KeypadAdd,
    KeypadEnter,
    KeypadEqual,
    Unknown
};

enum class Mouse : uint8_t { Left, Right, Middle };

// A point in world coordinates (the size given to openWindow), not window pixels
struct MousePosition {
    float x;
    float y;
};

struct MouseDelta {
    float x;
    float y;
};

struct InputEvent {
    enum class Type : uint8_t { KeyDown, KeyUp, Quit } type;
    Key key = Key::Unknown;
};
