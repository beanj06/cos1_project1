#include "Terminal.h"
#include <cstdio>


#include <windows.h>

static int colorCode(Color c) {
    if (c == Color::Default) return 39;
    return 30 + (static_cast<int>(c) - 1);  // Black = 30 ... White = 37
}

Terminal::Terminal() {
#ifdef _WIN32
    // Windows consoles ignore color/cursor codes until this is switched on
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(out, &mode);
    SetConsoleMode(out, mode | 0x0004);  // ENABLE_VIRTUAL_TERMINAL_PROCESSING
#endif
    std::printf("\033[?1049h");  // use the alternate screen
    std::printf("\033[2J");      // clear it once (and never again)
    std::printf("\033[?25l");    // hide cursor
    std::fflush(stdout);
}

Terminal::~Terminal() {
    std::printf("\033[0m");      // reset colors
    std::printf("\033[?25h");    // show cursor
    std::printf("\033[?1049l");  // back to the normal screen
    std::fflush(stdout);
}

void Terminal::beginFrame() {
    for (int r = 0; r < kRows; ++r) {
        for (int c = 0; c < kCols; ++c) grid_[r][c] = Cell();
    }
}

void Terminal::put(int row, int col, char ch, Color color) {
    if (row < 0 || row >= kRows || col < 0 || col >= kCols) return;  // off screen
    grid_[row][col].ch = ch;
    grid_[row][col].color = color;
}

void Terminal::writeAt(int row, int col, const std::string& text, Color color) {
    for (size_t i = 0; i < text.size(); ++i) {
        put(row, col + static_cast<int>(i), text[i], color);
    }
}

void Terminal::drawBar(int row, int col, int width, float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    int filled = static_cast<int>(ratio * width + 0.5f);

    // green when healthy, yellow when low, red when almost empty
    Color color = Color::Green;
    if (ratio < 0.25f) color = Color::Red;
    else if (ratio < 0.5f) color = Color::Yellow;

    put(row, col, '[', Color::Default);
    for (int i = 0; i < width; ++i) {
        if (i < filled) put(row, col + 1 + i, '#', color);
        else put(row, col + 1 + i, '.', Color::Default);
    }
    put(row, col + 1 + width, ']', Color::Default);
}

void Terminal::endFrame() {
    std::string out;

    for (int r = 0; r < kRows; ++r) {
        // find the last character on this row so we don't print trailing spaces
        int last = -1;
        for (int c = 0; c < kCols; ++c) {
            if (grid_[r][c].ch != ' ') last = c;
        }

        // turn the row into text with color codes
        std::string line;
        Color current = Color::Default;
        for (int c = 0; c <= last; ++c) {
            if (grid_[r][c].color != current) {
                current = grid_[r][c].color;
                line += "\033[" + std::to_string(colorCode(current)) + "m";
            }
            line += grid_[r][c].ch;
        }
        line += "\033[0m\033[K";  // reset color, erase the rest of the row

        // only send rows that are different from what's already on screen
        if (line != shown_[r]) {
            out += "\033[" + std::to_string(r + 1) + ";1H";
            out += line;
            shown_[r] = line;
        }
    }

    if (!out.empty()) {
        std::fwrite(out.data(), 1, out.size(), stdout);  // one write for the whole frame
        std::fflush(stdout);
    }
}
