#include "ScoreLoop.h"

#include <map>
#include <string>

#include "../DinodeckLua.h"
#include "../LuaState.h"
#include "../reflect/Reflect.h"
#include "../DDLog.h"

#if ANDROID
#include "AndroidWrapper.h"
#include "DSLuaCallbacks.h"
#endif

Reflect ScoreLoop::Meta("ScoreLoop", ScoreLoop::Bind);

ScoreLoop::ScoreLoop(const char* secret)
{
    // Call Java to do
    // Client.init(this, SECRET, null);
    dsprintf("ScoreLoop created with key: %s \n", secret);
#if ANDROID
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ScoreLoopInit(secret);
#endif
}

ScoreLoop::~ScoreLoop()
{
}

bool ScoreLoop::IsInitialized()
{
  #if ANDROID
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    return wrapper->ScoreLoopIsInitialized();
#endif
    return false;
}

void ScoreLoop::PostScore(double primary, double secondary,
               int onSuccessId, int onFailureId)
{
    // Needs going up to Java and some release mechanism for the
    // success, failure ids (hopefully be able to reuse the http ones.)
    dsprintf("Calling PostScore(%f, %f, %d, %d)\n", primary, secondary,
             onSuccessId, onFailureId);
#if ANDROID
    int callbackId = DSLuaCallbacks::StoreCallback(onSuccessId, onFailureId);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ScoreLoopPushScore
    (
        primary,
        secondary,
        callbackId
    );
#endif
}

void ScoreLoop::GetLeaderboard(int type, int onSuccessId, int onFailureId)
{
    dsprintf("Calling GetLeaderboard(%d, %d, %d)\n", type, onSuccessId,
             onFailureId);
#if ANDROID
    int callbackId = DDLuaCallbacks::StoreCallback(onSuccessId, onFailureId);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ScoreLoopGetLeaderboard
    (
        type,
        callbackId
    );
#endif
}

std::string ScoreLoop::GetTOSState()
{
#if ANDROID
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    return wrapper->ScoreLoopTOSState();
#endif
    return "NOT IMPLEMENTED";
}


//
// NOTE: If you've already enabled this, it won't show
// Clean the saved data with:
// >Grrr@Grrr-PC ~/Documents/mycode/dancing-squid/src/android
// >$ adb shell pm clear com.godpatterns.dinodeck
// >Success
//
//
void ScoreLoop::ShowTOS()
{
    dsprintf("ShowTOS();\n");
#if ANDROID
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    return wrapper->ScoreLoopShowTOS();
#endif

}

static int lua_ScoreLoop_Create(lua_State* state)
{
    if(!lua_isstring(state, 1))
    {
        return luaL_typerror(state, 1, "string");
    }
    const char* secret = lua_tostring(state, 1);

    new (lua_newuserdata(state, sizeof(ScoreLoop))) ScoreLoop(secret);
    luaL_getmetatable(state, "ScoreLoop");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_ScoreLoop_IsInitialized(lua_State* state)
{
     ScoreLoop* scoreLoop = LuaState::GetFuncParam<ScoreLoop>(state, 1);
    if(NULL == scoreLoop)
    {
        return 0;
    }
    lua_pushboolean(state, scoreLoop->IsInitialized());
    return 1;
}

/*
    ACCEPTED,
    OUTDATED,
    PENDING,
    REJECTED
*/
static int lua_ScoreLoop_GetTOSState(lua_State* state)
{
    ScoreLoop* scoreLoop = LuaState::GetFuncParam<ScoreLoop>(state, 1);
    if(NULL == scoreLoop)
    {
        return 0;
    }

    // Do some Java call and return TOS state as string.
    std::string tosState = scoreLoop->GetTOSState();

    if(tosState == "NOT_INITED")
    {
        return luaL_error(state, "ScoreLoop not initalised! Use IsInitialized function to check.");
    }

    lua_pushstring(state, tosState.c_str());
    return 1;
}

static int lua_ScoreLoop_ShowTOS(lua_State* state)
{
    ScoreLoop* scoreLoop = LuaState::GetFuncParam<ScoreLoop>(state, 1);
    if(NULL == scoreLoop)
    {
        return 0;
    }

    scoreLoop->ShowTOS();

    return 0;
}

    // static void stackDump (lua_State *L) {
    //   int i;
    //   int top = lua_gettop(L);
    //   for (i = 1; i <= top; i++) {  /* repeat for each level */
    //     int t = lua_type(L, i);
    //     switch (t) {

    //       case LUA_TSTRING:  /* strings */
    //         dsprintf("%d: `%s'", i, lua_tostring(L, i));
    //         break;

    //       case LUA_TBOOLEAN:  /* booleans */
    //         dsprintf("%d:%s", i, lua_toboolean(L, i) ? "true" : "false");
    //         break;

    //       case LUA_TNUMBER:  /* numbers */
    //         dsprintf("%d: %g", i, lua_tonumber(L, i));
    //         break;

    //       default:  /* other values */
    //         dsprintf("%d: %s", i, lua_typename(L, t));
    //         break;

    //     }
    //     dsprintf("  ");  /* put a separator */
    //   }
    //   dsprintf("\n");  /* end the listing */
    // }

static int lua_ScoreLoop_SendScore(lua_State* state)
{
    // score
    double primaryScore = 0;
    double secondaryScore = 0;

    ScoreLoop* scoreLoop = LuaState::GetFuncParam<ScoreLoop>(state, 1);
    if(NULL == scoreLoop)
    {
        return 0;
    }

    // May allow this to be a table?
    if(lua_isnumber(state, 2))         // score
    {
        primaryScore = lua_tonumber(state, 2);
    }
    else if(lua_istable(state, 2))
    {
        lua_pushvalue(state, 2); //  [table]

        // Does it have a key called primary
        lua_pushstring(state, "primary");  // first key [key[table]]

        lua_gettable(state, -2); // [value[table]]

        if(lua_isnumber(state, -1))
        {
            primaryScore = lua_tonumber(state, -1);
            dsprintf("Primary score is: %g\n", primaryScore);
        }
        // Otherwise ignore it.
        lua_pop(state, 1);

        lua_pushstring(state, "secondary");  // first key [key[table]]
        lua_gettable(state, -2); // [value[table]]

        if(lua_isnumber(state, -1))
        {
            secondaryScore = lua_tonumber(state, -1);
            dsprintf("Secondary score is: %g\n", secondaryScore);
        }
        // Otherwise ignore it.
        lua_pop(state, 1);

        lua_pop(state, 1); // pop off the table

        //dsprintf("Is function at correct place: %s\n", lua_isfunction(state, 3)?"true":"false");
    }
    else
    {
        return luaL_typerror(state, 2, "number or table");
    }

    if(!lua_isfunction(state, 3))       // success callback
    {
        return luaL_typerror(state, 3, "function");
    }
    if(!lua_isfunction(state, 4))     // failure callback
    {
        return luaL_typerror(state, 4, "function");
    }



    // Push success and failure calls into the lua registry and save
    // the callback ids
    lua_pushvalue(state, 3);
    int onSuccessRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]
    lua_pushvalue(state, 4);
    int onFailureRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]

    scoreLoop->PostScore
    (
        primaryScore,
        secondaryScore,
        onSuccessRef,
        onFailureRef
    );

    return 0;
}

static int lua_ScoreLoop_GetLeaderboard(lua_State* state)
{
    ScoreLoop* scoreLoop = LuaState::GetFuncParam<ScoreLoop>(state, 1);
    if(NULL == scoreLoop)
    {
        return 0;
    }

    if(!lua_isnumber(state, 2)) // Leaderboard type flag
    {
        return luaL_typerror(state, 2, "number");
    }

        if(!lua_isfunction(state, 3))       // success callback
    {
        return luaL_typerror(state, 3, "function");
    }
    if(!lua_isfunction(state, 4))     // failure callback
    {
        return luaL_typerror(state, 4, "function");
    }

    int leaderboardType = (int) lua_tonumber(state, 2);
    // Push success and failure calls into the lua registry and save
    // the callback ids
    lua_pushvalue(state, 3);
    int onSuccessRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]
    lua_pushvalue(state, 4);
    int onFailureRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]

    scoreLoop->GetLeaderboard
    (
        leaderboardType,
        onSuccessRef,
        onFailureRef
    );

    return 0;
}

static const struct luaL_reg luaBinding [] =
{
    {"Create", lua_ScoreLoop_Create},
    {"GetTOSState", lua_ScoreLoop_GetTOSState},
    {"ShowTOS", lua_ScoreLoop_ShowTOS},
    {"SendScore", lua_ScoreLoop_SendScore},
    {"IsInitialized", lua_ScoreLoop_IsInitialized},
    {"GetLeaderboard", lua_ScoreLoop_GetLeaderboard},
    {NULL, NULL}  /* sentinel */
};


void ScoreLoop::Bind(LuaState* state)
{
    state->Bind
    (
        ScoreLoop::Meta.Name(),
        luaBinding
    );
}