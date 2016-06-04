#ifndef TOUCH_H
#define TOUCH_H

#include "../reflect/Reflect.h"

class LuaState;

namespace TouchEvent
{
    enum Enum
    {
        None        = 0,
        Pressed     = 1,
        Released    = 2,
        Moving      = 3,
    };
}

struct TouchMessage
{
    TouchEvent::Enum mState;
    float mX;
    float mY;
};

// If we do multitouch, a seperate module is probably best
// GetNumTouches or something like that? Using a key to get data

class Touch
{
public: static Reflect Meta;

private:
    TouchEvent::Enum mState;
    float mX;
    float mY;
    TouchMessage* mTouchMessage;
public:
    static void Bind(LuaState* state);

    Touch() :
        mState(TouchEvent::None), mX(0), mY(0), mTouchMessage(NULL) {}

    float X() const { return mX; }
    float Y() const { return mY; }
    void OnTouchEvent(TouchMessage& msg) { mTouchMessage = &msg; }
    void Update();
    bool IsPressed() { return mState == TouchEvent::Pressed; }
    bool IsReleased() { return mState == TouchEvent::Released; }
    bool IsHeld() { return mState == TouchEvent::Moving; }
};
#endif
