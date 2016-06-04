#include "Sound.h"

#include <assert.h>

#include "DinodeckLua.h"
#include "Dinodeck.h"
#include "LuaState.h"
#include "DDLog.h"

Reflect Sound::Meta("Sound", Sound::Bind);

//
// number f(string name, bool loop = false)
// Returns an id for the sound being played.
//
static int lua_Sound_Play(lua_State* state)
{
    if(!lua_isstring(state, 1))
    {
        return luaL_typerror(state, 1, "string");
    }
    const char* soundName = lua_tostring(state, 1);

    bool looping = false;
    if(lua_isboolean(state, 2))
    {
        looping = lua_toboolean(state, 2);
    }

    Dinodeck* dd = Dinodeck::GetInstance();
    int soundId = dd->GetAudio()->Play(soundName, looping);

    lua_pushnumber(state, soundId);

    return 1;
}

static int lua_Sound_Stop(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int soundId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->Stop(soundId);
    return 0;
}

static int lua_Sound_Pause(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int soundId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->Pause(soundId);
    return 0;
}

static int lua_Sound_Resume(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int soundId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->Resume(soundId);
    return 0;
}

static int lua_Sound_SetVolume(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int soundId = lua_tonumber(state, 1);

    if(!lua_isnumber(state, 2))
    {
        return luaL_typerror(state, 2, "number");
    }
    float volume = lua_tonumber(state, 2);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->SetVolume(soundId, volume);

    return 0;
}


static const struct luaL_reg luaBinding [] =
{
 // {"Create", Vector::lua_Vector_Create},
    {"Play", lua_Sound_Play},
    {"Stop", lua_Sound_Stop},
    {"Pause", lua_Sound_Pause},
    {"Resume", lua_Sound_Resume},
    {"SetVolume", lua_Sound_SetVolume},
    {NULL, NULL}  /* sentinel */
};

void Sound::Bind(LuaState* state)
{
    state->Bind
    (
        Sound::Meta.Name(),
        luaBinding
    );
}