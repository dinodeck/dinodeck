#include "SaveGame.h"

#include "DinodeckLua.h"
#include "LuaState.h"
#include "DDLog.h"
#include "DDFile.h"

Reflect SaveGame::Meta("SaveGame", SaveGame::Bind);

static int lua_SaveGame_Read(lua_State* state)
{
    SaveGame* saveGame = LuaState::GetFuncParam<SaveGame>(state, 1);

    if(NULL == saveGame)
    {
        return 0;
    }
    std::string out("");
    DDFile::ReadSaveData(saveGame->Name().c_str(), out);
    lua_pushstring(state, out.c_str());
    return 1;
}

static int lua_SaveGame_Write(lua_State* state)
{
    SaveGame* saveGame = LuaState::GetFuncParam<SaveGame>(state, 1);

    if(NULL == saveGame)
    {
        return 0;
    }

    const char* data = luaL_checklstring(state, 2, NULL);

    if(NULL == data)
    {
        return 0;
    }

    DDFile::WriteSaveData(saveGame->Name().c_str(), data);

    return 0;
}

static int lua_LoadData_Create(lua_State* state)
{
    const char* name = luaL_checklstring(state, 1, NULL);

    if(NULL == name)
    {
        return 0;
    }

    /*SaveGame* pSaveGame =*/
    new (lua_newuserdata(state, sizeof(SaveGame))) SaveGame(name);
    luaL_getmetatable(state, "SaveGame");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_SaveGame_tostring(lua_State* state)
{
    SaveGame* saveGame = LuaState::GetFuncParam<SaveGame>(state, 1);
    if(NULL == saveGame)
    {
        // Just give out the metatable name.
        lua_pushstring(state, SaveGame::Meta.Name().c_str());
    }
    else
    {
        lua_pushstring(state, saveGame->ToString().c_str());
    }

    return 1;
}

static int lua_gc(lua_State* state)
{
    // It does nothing now but potentially this might close a file handle.
    SaveGame* saveGame = (SaveGame*)lua_touserdata(state, 1);
    if(NULL != saveGame)
    {
        dsprintf("%s collected.\n", saveGame->ToString().c_str());
    }

    return 0;
}

static const struct luaL_reg luaBinding [] =
{
    {"__gc", lua_gc},
    {"__tostring", lua_SaveGame_tostring},
    // Reads in all data as string.
    {"Read", lua_SaveGame_Read},
    // Writes out all data
    {"Write", lua_SaveGame_Write},
    // Create with a filename. Filename uses file if it exists or makes it.
    {"Create", lua_LoadData_Create},
    {NULL, NULL}  /* sentinel */
};


void SaveGame::Bind(LuaState* state)
{
    state->Bind
    (
        SaveGame::Meta.Name(),
        luaBinding
    );
}

std::string SaveGame::ToString() const
{
    std::stringstream ss;
    ss << "SaveGame [" << mName << "]";
    return ss.str();
}