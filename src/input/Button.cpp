#include "Button.h"

void Button::Update(bool pressed)
{
    mJustPressed = !mIsDown && pressed;
    mJustReleased = mIsDown && !pressed;
    mIsDown = pressed;
}