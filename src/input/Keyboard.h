#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../reflect/Reflect.h"
#include "Button.h"
#include "Keys.h"

class LuaState;

// If you're foing the pop-up keyboard for android or iphone
// Subclass this and make a VirtualKeyboard class.
class Keyboard
{
    public: static Reflect Meta;
private:
    Button mButton[KEY_LAST];
public:
    static void Bind(LuaState* state);
    Keyboard();
    void Update();
    void OnKeyDownEvent(int key);
    void OnKeyUpEvent(int key);
    bool IsButtonHeld(int key) const;
    bool IsButtonJustPressed(int key) const;
    bool IsButtonJustReleased(int key) const;
};

#endif