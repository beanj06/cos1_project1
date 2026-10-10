#pragma once
#include <memory>

// Represents a keypress event.
enum class Key { None, Char, Up, Down, Left, Right, Enter, Escape, Backspace };

struct KeyEvent {
    Key key = Key::None;
    char ch = 0;  // valid when key == Key::Char (letters, digits, space, ...)
};

// Puts the terminal into raw, non-blocking mode for as long as it lives
// and restores the original settings in the destructor.
class Input {
public:
    Input();
    ~Input();
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    // Returns immediately. Key::None means no key was pressed.
    // Call repeatedly (e.g. in a loop) to drain queued keypresses.
    KeyEvent poll();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};