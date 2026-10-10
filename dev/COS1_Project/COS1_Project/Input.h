#pragma once

enum class Key { None, Char, Up, Down, Left, Right, Enter, Escape, Backspace };

struct KeyEvent {
    Key key = Key::None;
    char ch = 0;  // only used when key == Key::Char
};

// Lets you read single keypresses without waiting for Enter.
class Input {
public:
    Input();
    ~Input();
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    // Returns right away. Key::None means nothing was pressed.
    KeyEvent poll();
};