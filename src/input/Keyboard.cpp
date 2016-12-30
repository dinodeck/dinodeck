#include "Keyboard.h"

#include <assert.h>

#include "../DinodeckLua.h"
#include "../LuaState.h"
#include "Button.h"

Reflect Keyboard::Meta("Keyboard", Keyboard::Bind);
static Keyboard* gKeyboard = NULL;

Keyboard::Keyboard()
{
    assert(gKeyboard == NULL);
    gKeyboard = this;
};

// Should be done once frame
void Keyboard::Update()
{
    for(int i = 0; i < KEY_LAST; i++ )
    {
        mButton[i].mJustPressed = false;
        mButton[i].mJustReleased = false;
    }
}

void Keyboard::OnKeyDownEvent(int key)
{
    assert(key < KEY_LAST);
    mButton[key].Update(true);
}

void Keyboard::OnKeyUpEvent(int key)
{
    assert(key < KEY_LAST);
    mButton[key].Update(false);
}

bool Keyboard::IsButtonHeld(int key) const
{
    assert(key < (int) KEY_LAST);
    return mButton[key].mIsDown;
}

bool Keyboard::IsButtonJustPressed(int key) const
{
    assert(key < (int) KEY_LAST);
    return mButton[key].mJustPressed;
}

bool Keyboard::IsButtonJustReleased(int key) const
{
    assert(key < (int) KEY_LAST);
    return mButton[key].mJustReleased;
}

static int lua_Keyboard_JustPressed(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)KEY_LAST)));
    lua_pushboolean(state, gKeyboard->IsButtonJustPressed(number));
    return 1;
}

static int lua_Keyboard_JustReleased(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)KEY_LAST)));
    lua_pushboolean(state, gKeyboard->IsButtonJustReleased(number));
    return 1;
}

static int lua_Keyboard_Held(lua_State* state)
{
    int number = (int)luaL_checknumber(state, 1);
    number = std::max(0, std::min(number, ((int)KEY_LAST)));
    lua_pushboolean(state, gKeyboard->IsButtonHeld(number));
    return 1;
}

static const struct luaL_reg luaBinding [] =
{
    {"JustPressed",     lua_Keyboard_JustPressed},
    {"JustReleased",    lua_Keyboard_JustReleased},
    {"Held",            lua_Keyboard_Held},
    {NULL, NULL}  /* sentinel */
};

void AddKeyToLuaGlobal(lua_State* state, const char* id, int value)
{
    lua_pushnumber(state, value);
    lua_setglobal(state, id);
}

void Keyboard::Bind(LuaState* state)
{
    state->Bind
    (
        Keyboard::Meta.Name(),
        luaBinding
    );

    lua_State* l = state->State();

    AddKeyToLuaGlobal(l, "KEY_BACKSPACE",   KEY_BACKSPACE);
    AddKeyToLuaGlobal(l, "KEY_TAB",         KEY_TAB);
    AddKeyToLuaGlobal(l, "KEY_CLEAR",       KEY_CLEAR);
    AddKeyToLuaGlobal(l, "KEY_RETURN",      KEY_RETURN);
    AddKeyToLuaGlobal(l, "KEY_PAUSE",       KEY_PAUSE);
    AddKeyToLuaGlobal(l, "KEY_ESCAPE",      KEY_ESCAPE);
    AddKeyToLuaGlobal(l, "KEY_SPACE",       KEY_SPACE);
    AddKeyToLuaGlobal(l, "KEY_EXCLAIM",     KEY_EXCLAIM);
    AddKeyToLuaGlobal(l, "KEY_QUOTEDBL",    KEY_QUOTEDBL);
    AddKeyToLuaGlobal(l, "KEY_HASH",        KEY_HASH);
    AddKeyToLuaGlobal(l, "KEY_DOLLAR",      KEY_DOLLAR);
    AddKeyToLuaGlobal(l, "KEY_AMPERSAND",   KEY_AMPERSAND);
    AddKeyToLuaGlobal(l, "KEY_QUOTE",       KEY_QUOTE);
    AddKeyToLuaGlobal(l, "KEY_LEFTPAREN",   KEY_LEFTPAREN);
    AddKeyToLuaGlobal(l, "KEY_RIGHTPAREN",  KEY_RIGHTPAREN);
    AddKeyToLuaGlobal(l, "KEY_ASTERISK",    KEY_ASTERISK);

    AddKeyToLuaGlobal(l, "KEY_A", KEY_A);
    AddKeyToLuaGlobal(l, "KEY_B", KEY_B);
    AddKeyToLuaGlobal(l, "KEY_C", KEY_C);
    AddKeyToLuaGlobal(l, "KEY_D", KEY_D);
    AddKeyToLuaGlobal(l, "KEY_E", KEY_E);
    AddKeyToLuaGlobal(l, "KEY_F", KEY_F);
    AddKeyToLuaGlobal(l, "KEY_G", KEY_G);
    AddKeyToLuaGlobal(l, "KEY_H", KEY_H);
    AddKeyToLuaGlobal(l, "KEY_I", KEY_I);
    AddKeyToLuaGlobal(l, "KEY_J", KEY_J);
    AddKeyToLuaGlobal(l, "KEY_K", KEY_K);
    AddKeyToLuaGlobal(l, "KEY_L", KEY_L);
    AddKeyToLuaGlobal(l, "KEY_M", KEY_M);
    AddKeyToLuaGlobal(l, "KEY_N", KEY_N);
    AddKeyToLuaGlobal(l, "KEY_O", KEY_O);
    AddKeyToLuaGlobal(l, "KEY_P", KEY_P);
    AddKeyToLuaGlobal(l, "KEY_Q", KEY_Q);
    AddKeyToLuaGlobal(l, "KEY_R", KEY_R);
    AddKeyToLuaGlobal(l, "KEY_S", KEY_S);
    AddKeyToLuaGlobal(l, "KEY_T", KEY_T);
    AddKeyToLuaGlobal(l, "KEY_U", KEY_U);
    AddKeyToLuaGlobal(l, "KEY_V", KEY_V);
    AddKeyToLuaGlobal(l, "KEY_W", KEY_W);
    AddKeyToLuaGlobal(l, "KEY_X", KEY_X);
    AddKeyToLuaGlobal(l, "KEY_Y", KEY_Y);
    AddKeyToLuaGlobal(l, "KEY_Z", KEY_Z);

    AddKeyToLuaGlobal(l, "KEY_0", KEY_0);
    AddKeyToLuaGlobal(l, "KEY_1", KEY_1);
    AddKeyToLuaGlobal(l, "KEY_2", KEY_2);
    AddKeyToLuaGlobal(l, "KEY_3", KEY_3);
    AddKeyToLuaGlobal(l, "KEY_4", KEY_4);
    AddKeyToLuaGlobal(l, "KEY_5", KEY_5);
    AddKeyToLuaGlobal(l, "KEY_6", KEY_6);
    AddKeyToLuaGlobal(l, "KEY_7", KEY_7);
    AddKeyToLuaGlobal(l, "KEY_8", KEY_8);
    AddKeyToLuaGlobal(l, "KEY_9", KEY_9);


    AddKeyToLuaGlobal(l, "KEY_UP",          KEY_UP);
    AddKeyToLuaGlobal(l, "KEY_DOWN",        KEY_DOWN);
    AddKeyToLuaGlobal(l, "KEY_RIGHT",       KEY_RIGHT);
    AddKeyToLuaGlobal(l, "KEY_LEFT",        KEY_LEFT);
    AddKeyToLuaGlobal(l, "KEY_INSERT",      KEY_INSERT);
    AddKeyToLuaGlobal(l, "KEY_HOME",        KEY_HOME);
    AddKeyToLuaGlobal(l, "KEY_END",         KEY_END);
    AddKeyToLuaGlobal(l, "KEY_PAGEUP",      KEY_PAGEUP);
    AddKeyToLuaGlobal(l, "KEY_PAGEDOWN",    KEY_PAGEDOWN);

    AddKeyToLuaGlobal(l, "KEY_RSHIFT",      KEY_RSHIFT);
    AddKeyToLuaGlobal(l, "KEY_LSHIFT",      KEY_LSHIFT);
    AddKeyToLuaGlobal(l, "KEY_RCTRL",       KEY_RCTRL);
    AddKeyToLuaGlobal(l, "KEY_LCTRL",       KEY_LCTRL);
    AddKeyToLuaGlobal(l, "KEY_RALT",        KEY_RALT);
    AddKeyToLuaGlobal(l, "KEY_LALT",        KEY_LALT);
}