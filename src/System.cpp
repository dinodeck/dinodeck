#include "System.h"
#include "Version.h"

#include <assert.h>
// Needs moving out of here!
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif


#include "DinodeckLua.h"
#include "Game.h"
#include "reflect/Reflect.h"
#include "Vector.h"
#include "LuaState.h"
#include "Settings.h"


Reflect System::Meta("System", System::Bind);


static int lua_IsWideScreen(lua_State* state)
{
    lua_pushboolean(state, true);
    return 1;
}

// These need to get the Game injection.
static int lua_GetScreenBottomRight(lua_State* state)
{
    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();;
    assert(vector != NULL);
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    int width = game->GetSettings()->width;
    int height = game->GetSettings()->height;
    vector->SetXyzw(width/2, -height/2, 0, 0);
    return 1;
}

static int lua_GetScreenTopLeft(lua_State* state)
{
    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    assert(vector != NULL);
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    int width = game->GetSettings()->width;
    int height = game->GetSettings()->height;
    vector->SetXyzw(-width/2, height/2, 0, 0);
    return 1;
}

static int lua_ScreenWidth(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    lua_pushnumber(state, game->GetSettings()->width);
    return 1;
}

static int lua_ScreenHeight(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    lua_pushnumber(state, game->GetSettings()->height);
    return 1;
}

static int lua_OpenURL(lua_State* state)
{
    if(!lua_isstring(state, -1))
    {
        luaL_typerror(state, -1, "string");
        return 0;
    }
    else
    {
#ifdef _WIN32
        const char* url = lua_tostring(state, -1);
        ShellExecute(GetActiveWindow(), "open", url, NULL, NULL, SW_SHOWNORMAL);
#endif
    }
    return 0;
}

static int lua_Version(lua_State* state)
{
    lua_pushstring(state, DINODECK_VERSION);
    return 1;
}

static int lua_Exit(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    game->Stop();
    return 0;
}

static const struct luaL_reg luaBinding [] = {
  {"IsWideScreen", lua_IsWideScreen},
  {"ScreenWidth", lua_ScreenWidth},
  {"ScreenHeight", lua_ScreenHeight},
  {"ScreenTopLeft", lua_GetScreenTopLeft},
  {"ScreenBottomRight", lua_GetScreenBottomRight},
  {"OpenURL", lua_OpenURL},
  {"Version", lua_Version},
  {"Exit", lua_Exit},
  {NULL, NULL}  /* sentinel */
};

void System::Bind(LuaState* state)
{
    state->Bind
    (
        System::Meta.Name(),
        luaBinding
    );
}