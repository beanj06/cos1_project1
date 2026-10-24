#include "Art.h"

static const char* kEars = "  /\\_/\\";

static ArtFrame makeFrame(const char* top, const char* ears, const char* face,
    const char* body, float seconds) {
    return ArtFrame{ {top, ears, face, body}, seconds };
}

const Animation& animationFor(PetState state) {
    // Edit the pictures here. Keep each one 4 lines and under ~12 characters wide.
    static const Animation content = { {
        makeFrame("", kEars, " ( o.o )", "  > ^ <", 1.6f),
        makeFrame("", kEars, " ( -.- )", "  > ^ <", 0.2f),      // blink
    }, Color::Yellow };

    static const Animation happy = { {
        makeFrame("   <3", kEars, " ( ^.^ )", "  > ^ <", 0.6f),
        makeFrame("  <3  ", kEars, " ( ^o^ )", "  > ^ <", 0.6f),
    }, Color::Green };

    static const Animation hungry = { {
        makeFrame("  *grumble*", kEars, " ( o.o )", "  > o <", 0.7f),
        makeFrame("", kEars, " ( o.o )", "  > o <", 0.5f),
    }, Color::Yellow };

    static const Animation tired = { {
        makeFrame("", kEars, " ( -.- )", "  > ^ <", 1.0f),
        makeFrame("  *yawn*", kEars, " ( -o- )", "  > ^ <", 0.8f),
    }, Color::Blue };

    static const Animation dirty = { {
        makeFrame("  ~ ~ ~", kEars, " ( o.o )", " ,> ^ <,", 0.5f),
        makeFrame(" ~ ~ ~ ", kEars, " ( o.o )", " ,> ^ <,", 0.5f),
    }, Color::Magenta };

    static const Animation sad = { {
        makeFrame("", kEars, " ( T.T )", "  > ^ <", 0.8f),
        makeFrame("   .", kEars, " ( ;.; )", "  > ^ <", 0.8f),
    }, Color::Blue };

    static const Animation sick = { {
        makeFrame("  + +", kEars, " ( @.@ )", "  > ~ <", 0.6f),
        makeFrame("   + +", kEars, " ( x.@ )", "  > ~ <", 0.6f),
    }, Color::Green };

    static const Animation sleeping = { {
        makeFrame("", kEars, " ( -.- )", "  > ^ <", 0.7f),
        makeFrame("    z", kEars, " ( -.- )", "  > ^ <", 0.7f),
        makeFrame("   zZ", kEars, " ( -.- )", "  > ^ <", 0.7f),
        makeFrame("  zZZ", kEars, " ( -.- )", "  > ^ <", 0.7f),
    }, Color::Blue };

    static const Animation dead = { {
        makeFrame("   RIP", kEars, " ( x.x )", "  > - <", 1.0f),
    }, Color::Red };

    static const Animation eating = { {
        makeFrame("  nom nom", kEars, " ( ^.^ )", "  > o <", 0.25f),
        makeFrame(" nom nom", kEars, " ( ^.^ )", "  > - <", 0.25f),
    }, Color::Yellow };

    static const Animation playing = { {
        makeFrame("  *   *", kEars, " ( ^o^ )", "  > ^ <", 0.25f),
        makeFrame("   *   *", "   /\\_/\\", "  ( ^o^ )", "   > ^ <", 0.25f),
    }, Color::Green };

    static const Animation cleaning = { {
        makeFrame(" o O o O", kEars, " ( ^.^ )", "  > ^ <", 0.25f),
        makeFrame(" O o O o", kEars, " ( ^.^ )", "  > ^ <", 0.25f),
    }, Color::Cyan };

    static const Animation medicating = { {
        makeFrame("  + + +", kEars, " ( >.< )", "  > ^ <", 0.3f),
        makeFrame(" + + +", kEars, " ( >.< )", "  > ^ <", 0.3f),
    }, Color::Magenta };

    switch (state) {
    case PetState::Happy:      return happy;
    case PetState::Content:    return content;
    case PetState::Hungry:     return hungry;
    case PetState::Tired:      return tired;
    case PetState::Dirty:      return dirty;
    case PetState::Sad:        return sad;
    case PetState::Sick:       return sick;
    case PetState::Sleeping:   return sleeping;
    case PetState::Dead:       return dead;
    case PetState::Eating:     return eating;
    case PetState::Playing:    return playing;
    case PetState::Cleaning:   return cleaning;
    case PetState::Medicating: return medicating;
    }
    return content;
}

// ------------------------------------------------------------------ Animator

void Animator::update(double dt, PetState state) {
    if (state != state_) {  // new state: start its animation from the first frame
        state_ = state;
        index_ = 0;
        timer_ = 0.0f;
    }

    const Animation& animation = animationFor(state_);
    timer_ += static_cast<float>(dt);

    // Move on to the next frame(s) when the current one has been shown long enough.
    while (timer_ >= animation.frames[index_].duration) {
        timer_ -= animation.frames[index_].duration;
        index_ = (index_ + 1) % animation.frames.size();
    }
}

const ArtFrame& Animator::frame() const {
    return animationFor(state_).frames[index_];
}

Color Animator::color() const {
    return animationFor(state_).color;
}