#include "Touch.h"

#include <assert.h>

#include "../DinodeckLua.h"
#include "../DDLog.h"
#include "../Game.h"
#include "../LuaState.h"
#include "../reflect/Reflect.h"
#include "../Vector.h"

Reflect Touch::Meta("Touch", Touch::Bind);

void Touch::Update()
{
    if(mTouchMessage)
    {
        mState = mTouchMessage->mState;
        mX = mTouchMessage->mX;
        mY = mTouchMessage->mY;
        mTouchMessage = NULL;
    }
    else
    {
        mState = TouchEvent::None;
    }
}


static int lua_Touch_X(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    Touch* touch = game->GetTouch();
    lua_pushnumber(state, touch->X());
    return 1;
}

static int lua_Touch_Y(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    Touch* touch = game->GetTouch();
    lua_pushnumber(state, touch->Y() * -1);
    return 1;
}

static int lua_Touch_Position(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    Touch* touch = game->GetTouch();

    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 1);
        if(vector == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        vector->x = touch->X();
        vector->y = touch->Y() * -1;
        return 0;
    }

    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    vector->x = touch->X();
    vector->y = touch->Y() * -1;
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_Touch_JustPressed(lua_State* state)
{
    //dsprintf("In touch just pressed.\n");
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    //dsprintf("Just got game pointer [%s].\n", game?"non-null":"null");
    assert(game);
    Touch* touch = game->GetTouch();
    //dsprintf("Just got touch pointer [%s].\n", touch?"non-null":"null");
    assert(touch);
    bool justPressed = touch->IsPressed();
    //dsprintf("Called just pressed [%s].\n", justPressed?"true":"false");
    lua_pushboolean(state, justPressed);
    return 1;
}

static int lua_Touch_JustReleased(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    Touch* touch = game->GetTouch();
    lua_pushboolean(state, touch->IsReleased());
    return 1;
}

static int lua_Touch_Held(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    Touch* touch = game->GetTouch();
    lua_pushboolean(state, touch->IsHeld());
    return 1;
}

static const struct luaL_reg luaBinding [] =
{
    {"X", lua_Touch_X},
    {"Y", lua_Touch_Y},
    {"Position", lua_Touch_Position},
    {"JustPressed", lua_Touch_JustPressed},
    {"JustReleased", lua_Touch_JustReleased},
    {"Held", lua_Touch_Held},
    {NULL, NULL}  /* sentinel */
};

void Touch::Bind(LuaState* state)
{
    state->Bind
    (
        Touch::Meta.Name(),
        luaBinding
    );
}