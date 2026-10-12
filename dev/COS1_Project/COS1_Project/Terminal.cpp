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

void Terminal::writeAt(int row, int col, const std::string& text, Color color) {
    moveTo(row, col);
    std::printf("\033[%dm%s\033[0m", colorCode(color), text.c_str());
}

void Terminal::drawBar(int row, int col, int width, float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    int filled = static_cast<int>(ratio * width + 0.5f);

    // green when healthy, yellow when low, red when almost empty
    Color color = Color::Green;
    if (ratio < 0.25f) color = Color::Red;
    else if (ratio < 0.5f) color = Color::Yellow;

    moveTo(row, col);
    std::printf("[\033[%dm", colorCode(color));
    for (int i = 0; i < filled; ++i) std::putchar('#');
    std::printf("\033[0m");
    for (int i = filled; i < width; ++i) std::putchar('.');
    std::putchar(']');
}
