#include "Terminal.h"

#include <cstdio>

static int colorCode(Color c) {
    if (c == Color::Default) return 39;
    return 30 + (static_cast<int>(c) - 1);  // Black = 30 ... White = 37
}

static void moveTo(int row, int col) {
    std::printf("\033[%d;%dH", row + 1, col + 1);
}

Terminal::Terminal() {
    std::printf("\033[?1049h");  // use the alternate screen
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
    std::printf("\033[H\033[2J");  // cursor to top-left, clear screen
}

void Terminal::endFrame() {
    std::fflush(stdout);
}


