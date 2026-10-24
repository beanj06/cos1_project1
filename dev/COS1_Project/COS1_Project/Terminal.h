#pragma once
#include <string>

enum class Color { Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

// Very small terminal helper.
//
// Per frame:
//   term.beginFrame();
//   term.writeAt(0, 0, "Hello", Color::Green);
//   term.endFrame();
//
// Drawing goes into an in-memory screen first. endFrame() then prints only the
// rows that changed since last time, so nothing flashes.
// Rows and columns start at 0 (top-left corner). The screen is 80x30.
class Terminal {
public:
    Terminal();
    ~Terminal();
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    void beginFrame();  // start a new, blank frame
    void endFrame();    // show it

    void writeAt(int row, int col, const std::string& text, Color color = Color::Default);

    // Draws something like [#####.....]. ratio is 0.0 to 1.0.
    void drawBar(int row, int col, int width, float ratio);

private:
    static constexpr int kRows = 30;
    static constexpr int kCols = 80;

    struct Cell {
        char ch = ' ';
        Color color = Color::Default;
    };

    void put(int row, int col, char ch, Color color);

    Cell grid_[kRows][kCols];     // the frame being drawn
    std::string shown_[kRows];    // what each row looks like on the real screen
};