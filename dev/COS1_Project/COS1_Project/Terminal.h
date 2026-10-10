#pragma once
#include <string>
#include <utility>

enum class Color { Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

/*
* usage:
* terminal renderer;
*/
class Terminal {
public:
    Terminal();
    ~Terminal();
    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    void beginFrame();  // clears the screen buffer
    void endFrame();    // flushes the buffer to the terminal

    // Coordinates are 0-based: row 0, col 0 is the top-left corner. need to remember this...
    void moveTo(int row, int col);
    void setColor(Color fg, Color bg = Color::Default);
    void resetStyle();
    void write(const std::string& text);
    void writeAt(int row, int col, const std::string& text,
        Color fg = Color::Default, Color bg = Color::Default);

    // Draws "[#####.....]" with `width` inner cells; ratio is 0.0 - 1.0 added in Pet.h.
    // Colour goes green -> yellow -> red as the ratio drops.
    void drawBar(int row, int col, int width, float ratio);

    // Returns {rows, cols}; falls back to {24, 80} if unknown.
    std::pair<int, int> size() const;

private:
    std::string buffer_;
};