#include "Input.h"
#include <conio.h>

Input::Input() {}   // Windows needs no setup
Input::~Input() {}

KeyEvent Input::poll() {
    if (!_kbhit()) return {};  // nothing typed

    int c = _getch();

    // Arrow keys send two values: 0 or 224, then a code
    if (c == 0 || c == 224) {
        int code = _getch();
        if (code == 72) return { Key::Up, 0 };
        if (code == 80) return { Key::Down, 0 };
        if (code == 75) return { Key::Left, 0 };
        if (code == 77) return { Key::Right, 0 };
        return {};
    }

    if (c == 27) return { Key::Escape, 0 };
    if (c == '\r' || c == '\n') return { Key::Enter, 0 };
    if (c == 8) return { Key::Backspace, 0 };
    return { Key::Char, static_cast<char>(c) };
}
