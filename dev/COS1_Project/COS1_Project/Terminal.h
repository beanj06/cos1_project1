#pragma once
#include <string>


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

    void beginFrame();  // clear the screen
    void endFrame();    // make sure everything is shown

    void writeAt(int row, int col, const std::string& text, Color color = Color::Default);

    // Draws something like [#####.....]. ratio is 0.0 to 1.0.
    void drawBar(int row, int col, int width, float ratio);
};
