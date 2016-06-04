#ifndef MOUSE_H
#define MOUSE_H

#include "../reflect/Reflect.h"
#include "../Vector.h"
#include "Button.h"

class LuaState;

class Mouse
{
    public: static Reflect Meta;
    public:
        enum ButtonId
        {
            MOUSE_BUTTON_LEFT,
            MOUSE_BUTTON_MIDDLE,
            MOUSE_BUTTON_RIGHT,
            BUTTON_ID_COUNT
        };

        static const char* ButtonIdStr[BUTTON_ID_COUNT];
        static void Bind(LuaState* state);

        Button mButtonData[BUTTON_ID_COUNT];

        void Update();
        bool IsButtonHeld(int button) const;
        bool IsButtonJustPressed(int button) const;
        bool IsButtonJustReleased(int button) const;
        void OnMouseEvent(float mouseX,
                          float mouseY,
                          bool leftDown,
                          bool middleDown,
                          bool rightDown);
        Mouse();
    private:
        Vector mPrevPosition;
        Vector mPosition;
        Vector mDifference;
    public:
        const Vector& PrevPosition() const { return mPrevPosition; }
        const Vector& Position() const { return mPosition; }
        const Vector& Difference() const { return mDifference; }
};
#endif
