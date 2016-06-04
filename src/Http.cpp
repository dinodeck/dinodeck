#include "Http.h"

#include "DinodeckLua.h"
#include "DDLog.h"
#include "DDRestful.h"
#include "HttpPostData.h"
#include "LuaState.h"
#include "reflect/Reflect.h"

Reflect Http::Meta("Http", Http::Bind);

static int lua_Http_Post(lua_State* state)
{
    // Do a RESTful type post command

    // The URI we're talking to
    if(!lua_isstring(state, 1))
    {
        return luaL_typerror(state, 1, "number");
    }
    const char* uri = lua_tostring(state, 1);

    // HttpPost Data is optional
    HttpPostData* httpPostData = NULL;
    if(lua_isuserdata(state, 2) && LuaState::IsType(state, 2, "HttpPostData"))
    {
        httpPostData = reinterpret_cast<HttpPostData*>(lua_touserdata(state, 2));
    }
    else if(!lua_isnil(state, 2))
    {
        return luaL_typerror(state, 2, "HttpPostData | nil");
    }

    // The onSuccess
    if(!lua_isfunction(state, 3))
    {
        return luaL_typerror(state, 3, "function");
    }
    // The onFailure
    if(!lua_isfunction(state, 4))
    {
        return luaL_typerror(state, 4, "function");
    }
    //stackDump(state);
    lua_pushvalue(state, 3);
    int onSuccessRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]
    lua_pushvalue(state, 4);
    //dsprintf("Top type: %d", lua_type(state, -1));
    //dsprintf("is everything ok: %s\n", lua_isfunction(state, -1)? "yes":"no");
    int onFailureRef = luaL_ref(state, LUA_REGISTRYINDEX); // [-1, +0, m]

    /* int callbackId = */
    dsprintf("posting callbacks: %d, %d", onSuccessRef, onFailureRef);
    DDRestful::Post(uri, httpPostData, onSuccessRef, onFailureRef);

    // std::string test = "hello";
    // DDRestful::OnSuccess(LuaState::GetWrapper(state), callbackId, test);
    return 0;
}

static const struct luaL_reg luaBinding [] =
{
    {"Post", lua_Http_Post},
    {NULL, NULL}  /* sentinel */
};

void Http::Bind(LuaState* state)
{
    state->Bind
    (
        Http::Meta.Name(),
        luaBinding
    );
}