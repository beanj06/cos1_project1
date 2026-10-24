#pragma once
#include <string>
#include <vector>

#include "Pet.h"
#include "Terminal.h"

// One picture of the pet. Every frame has 4 lines:
//   line 0: effects above the head (zzz, hearts, ...)
//   line 1: ears
//   line 2: face
//   line 3: body
struct ArtFrame {
    std::vector<std::string> lines;
    float duration;  // seconds this frame stays on screen
};

// A looping set of frames plus the color to draw them in.
struct Animation {
    std::vector<ArtFrame> frames;
    Color color;
};

// The animation for each pet state.
const Animation& animationFor(PetState state);

// Plays the animation for whatever state the pet is in.
// Call update() every frame, then draw frame() in color().
class Animator {
public:
    void update(double dt, PetState state);
    const ArtFrame& frame() const;
    Color color() const;

private:
    PetState state_ = PetState::Content;
    size_t index_ = 0;
    float timer_ = 0.0f;
};
