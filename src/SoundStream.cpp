#include "SoundStream.h"

#include "DinodeckLua.h"
#include "Dinodeck.h"
#include "LuaState.h"
#include "DDLog.h"

Reflect SoundStream::Meta("SoundStream", SoundStream::Bind);

int lua_SoundStream_Play(lua_State* state)
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
    int streamId = dd->GetAudio()->PlayStream(soundName, looping);

    lua_pushnumber(state, streamId);

    return 1;
}

int lua_SoundStream_Stop(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int streamId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->StopStream(streamId);
    return 0;
}

int lua_SoundStream_Pause(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int streamId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->PauseStream(streamId);
    return 0;
}

int lua_SoundStream_Resume(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int streamId = lua_tonumber(state, 1);

    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->ResumeStream(streamId);
    return 0;
}

int lua_SoundStream_SetVolume(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    int streamId = lua_tonumber(state, 1);

    if(!lua_isnumber(state, 2))
    {
        return luaL_typerror(state, 2, "number");
    }
    float volume = lua_tonumber(state, 2);

    dsprintf("lua_SoundStream_SetVolume");
    Dinodeck* dd = Dinodeck::GetInstance();
    dd->GetAudio()->SetStreamVolume(streamId, volume);
    return 0;
}

static const struct luaL_reg luaBinding [] =
{
 // {"Create", Vector::lua_Vector_Create},
    {"Play", lua_SoundStream_Play},
    {"Stop", lua_SoundStream_Stop},
    {"Pause", lua_SoundStream_Pause},
    {"Resume", lua_SoundStream_Resume},
    {"SetVolume", lua_SoundStream_SetVolume},
    {NULL, NULL}  /* sentinel */
};

void SoundStream::Bind(LuaState* state)
{
    state->Bind
    (
        SoundStream::Meta.Name(),
        luaBinding
    );
}