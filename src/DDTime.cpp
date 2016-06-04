#include "DDTime.h"

#include <assert.h>
#include <cmath>
#include <ctime>

#include "DinodeckLua.h"
#include "LuaState.h"
#include "reflect/Reflect.h"

Reflect DDTime::Meta("Time", DDTime::Bind);

static int lua_Difference(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    if(!lua_isnumber(state, 2))
    {
        return luaL_typerror(state, 2, "number expected.");
    }

    time_t firstTime = (time_t) lua_tonumber(state, 1);
    time_t secondTime = (time_t) lua_tonumber(state, 2);
    lua_pushnumber(state, difftime(secondTime, firstTime));
    return 1;
}

static int lua_Get_Year(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );
    if(NULL != structedTime)
    {
        int unix_year_start = 1900;
        lua_pushnumber(state, unix_year_start + structedTime->tm_year);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}

static int lua_Get_Date(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );
    if(NULL != structedTime)
    {
        lua_pushnumber(state, structedTime->tm_mday);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}

static int lua_Get_Day(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );
    if(NULL != structedTime)
    {
        // 1 - 7
        lua_pushnumber(state, structedTime->tm_wday + 1);
        return 1;
    }
    return luaL_error(state, "invalid time value");
}

static int lua_Get_Hour(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );
    if(NULL != structedTime)
    {
        // 0 - 23
        lua_pushnumber(state, structedTime->tm_hour);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}

static int lua_Get_Minute(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );
    if(NULL != structedTime)
    {
        // 0 - 59
        lua_pushnumber(state, structedTime->tm_min);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}

static int lua_Get_Month(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );

    if(NULL != structedTime)
    {
        // 0 to 11 -> 1 - 12
        lua_pushnumber(state, structedTime->tm_mon + 1);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}

static int lua_Get_Second(lua_State* state)
{
    if(!lua_isnumber(state, 1))
    {
        return luaL_typerror(state, 1, "number expected.");
    }

    time_t rawTime = (time_t) lua_tonumber(state, 1);
    struct tm* structedTime = gmtime ( &rawTime );

    if(NULL != structedTime)
    {
        // 0 to 59 (can be up to 61 for leap seconds on cerain systems.)
        lua_pushnumber(state, structedTime->tm_sec);
        return 1;
    }

    return luaL_error(state, "invalid time value");
}


static int lua_Get_Time(lua_State* state)
{
    lua_pushnumber(state, time(0));
    return 1;
}

static const struct luaL_reg luaBinding [] =
{
    {"Difference", lua_Difference},
    {"GetDate", lua_Get_Date},
    {"GetDay", lua_Get_Day},
    {"GetHour", lua_Get_Hour},
    {"GetMinute", lua_Get_Minute},
    {"GetMonth", lua_Get_Month},
    {"GetSecond", lua_Get_Second},
    {"GetTime", lua_Get_Time},
    {"GetYear", lua_Get_Year},
    {NULL, NULL}  /* sentinel */
};

void DDTime::Bind(LuaState* state)
{
    state->Bind
    (
        DDTime::Meta.Name(),
        luaBinding
    );
}