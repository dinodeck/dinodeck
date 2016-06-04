#ifndef BUTTON_H
#define BUTTON_H

// Buttons are used by a number of the input devices (mice, control pads, keyboard)

struct Button
{
public:
    bool mIsDown;
    bool mJustReleased;
    bool mJustPressed;
    Button() :
        mIsDown(false),
        mJustReleased(false),
        mJustPressed(false) {}

    // @pressed - is the button pressed this frame
    void Update(bool pressed);
};

#endif