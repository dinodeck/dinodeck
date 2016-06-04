#include "Mouse.h"

#include <assert.h>
#include <cmath>

#include "../DinodeckGL.h"
#include "../LuaState.h"
#include "../reflect/Reflect.h"
#include "../DDLog.h"

static Mouse* gMouse = NULL;
Reflect Mouse::Meta("Mouse", Mouse::Bind);

const char* Mouse::ButtonIdStr[Mouse::BUTTON_ID_COUNT] =
{
    "MOUSE_BUTTON_LEFT",
    "MOUSE_BUTTON_MIDDLE",
    "MOUSE_BUTTON_RIGHT",
};



Mouse::Mouse()
{
    assert(gMouse == NULL);
    // Is this hacky?
    // Gives lua simple fast access.
    gMouse = this;
}

void Mouse::Update()
{
}

void Mouse::OnMouseEvent(float mouseX,
                  float mouseY,
                  bool leftDown,
                  bool middleDown,
                  bool rightDown)
{
    mPrevPosition.x = mPosition.x;
    mPrevPosition.y = mPosition.y;
    mPosition.x = mouseX;
    mPosition.y = mouseY;
    Vector::Subtract(mDifference, mPosition, mPrevPosition);
    mButtonData[MOUSE_BUTTON_LEFT].Update(leftDown);
    mButtonData[MOUSE_BUTTON_MIDDLE].Update(middleDown);
    mButtonData[MOUSE_BUTTON_RIGHT].Update(rightDown);
}

bool Mouse::IsButtonHeld(int button) const
{
    assert(button < (int) BUTTON_ID_COUNT);
    return mButtonData[button].mIsDown;
}

bool Mouse::IsButtonJustPressed(int button) const
{
    assert(button < (int) BUTTON_ID_COUNT);
    return mButtonData[button].mJustPressed;
}
bool Mouse::IsButtonJustReleased(int button) const
{
    assert(button < (int) BUTTON_ID_COUNT);
    return mButtonData[button].mJustReleased;
}

static int lua_Mouse_X(lua_State* state)
{
    lua_pushnumber(state, gMouse->Position().x);
    return 1;
}

static int lua_Mouse_Y(lua_State* state)
{
    lua_pushnumber(state, gMouse->Position().y*-1);
    return 1;
}

static int lua_Mouse_JustPressed(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)Mouse::MOUSE_BUTTON_RIGHT)));
    lua_pushboolean(state, gMouse->IsButtonJustPressed(number));
    return 1;
}

static int lua_Mouse_JustReleased(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)Mouse::MOUSE_BUTTON_RIGHT)));
    lua_pushboolean(state, gMouse->IsButtonJustReleased(number));
    return 1;
}

static int lua_Mouse_Held(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)Mouse::MOUSE_BUTTON_RIGHT)));
    lua_pushboolean(state, gMouse->IsButtonHeld(number));
    return 1;
}

// Either pass a vector in that will be assigned to
// Or a new vector will be created and passed back
static int lua_Mouse_Position(lua_State* state)
{
    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 1);
        if(vector == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        vector->SetXyzw(gMouse->Position());
        vector->y *= -1;
        return 0;
    }

    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    vector->SetXyzw(gMouse->Position());
    vector->y *= -1;
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_Mouse_PrevPosition(lua_State* state)
{
    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 1);
        if(vector == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        vector->SetXyzw(gMouse->PrevPosition());
        return 0;
    }

    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    vector->SetXyzw(gMouse->PrevPosition());
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_Mouse_Difference(lua_State* state)
{
    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 1);
        if(vector == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        vector->SetXyzw(gMouse->Difference());
        return 0;
    }

    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    vector->SetXyzw(gMouse->Difference());
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    return 1;
}

static const struct luaL_reg luaBinding [] =
{
    {"X", lua_Mouse_X},
    {"Y", lua_Mouse_Y},
    {"Position", lua_Mouse_Position},  // Get vector postion - that should expect a vector passed in.
    {"PrevPosition", lua_Mouse_PrevPosition},
    {"Difference", lua_Mouse_Difference},
    {"JustPressed", lua_Mouse_JustPressed},
    {"JustReleased", lua_Mouse_JustReleased},
    {"Held", lua_Mouse_Held},
    {NULL, NULL}  /* sentinel */
};

void Mouse::Bind(LuaState* state)
{
    state->Bind
    (
        Mouse::Meta.Name(),
        luaBinding
    );

    for(int i = 0; i < Mouse::BUTTON_ID_COUNT; i++)
    {
        lua_pushnumber(state->State(), i);
        lua_setglobal(state->State(), Mouse::ButtonIdStr[i]);
    }
}