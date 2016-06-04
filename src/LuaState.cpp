#include "LuaState.h"
#include <assert.h>
#include <string.h>

#include "DinodeckLua.h"
#include "DDFile.h"
#include "DDLog.h"



LuaState::LuaState(const char* name) :
    mName(name),
    mLuaState(NULL)
{
    // lua_newstate( MemHandler, NULL ); <- can use this to get some mem stats
    Reset();
}


LuaState::~LuaState()
{
    dsprintf("[LUASTATE|%s] DESTROYED\n", mName);
    lua_close(mLuaState);
}

LuaState* LuaState::GetWrapper(lua_State* luaState)
{
    LuaState* out = static_cast<LuaState*>(LuaState::GetFromRegistry(luaState, "this"));
    assert(luaState);
    return out;
}

int LuaState::LuaError(lua_State* luaState)
{
	LuaState* wrapper = LuaState::GetWrapper(luaState);
	assert(wrapper);
	wrapper->OnError();
	return 0;
}

void LuaState::OnError()
{
 	// Prints out the error and a stack trace
    mLastError = lua_tostring(mLuaState, -1);
    dsprintf("\n[LUASTATE|%s] Error: %s\n", mName, mLastError.c_str());

    // Push the debug library on the stack
    lua_getfield(mLuaState, LUA_GLOBALSINDEX, "debug");
    if(!lua_istable(mLuaState, -1))
    {
        dsprintf("[LUASTATE|%s] Debug library not loaded. Couldn't get stack trace\n", mName);
        lua_pop(mLuaState, 1); // pop the non-table
        return;
    }

    // Debug library is on the stack

    // A traceback is the most basic thing we can do but we can also
    // Get variable information and all that good stuff - it's just
    // a question of how to present it.

    // Push the traceback on to the stack as a field
    lua_getfield(mLuaState, -1, "traceback");

    if(!lua_isfunction(mLuaState, -1))
    {
        // If the traceback field doesn't exist
        dsprintf("[LUASTATE|%s] Tried to get trackback but function doesnt exist.\n", mName);
        dsprintf("[LUASTATE|%s] Have you overidden the default debug table?\n", mName);
        lua_pop(mLuaState, 2); // pop table and field
        return;
    }

    // Call function on the top of the stack, with no arguements, expecting one return.
    lua_call(mLuaState, 0, 1);
    mLastErrorCallstack = lua_tostring(mLuaState, -1);
    dsprintf("[LUASTATE|%s] %s\n", mName, mLastErrorCallstack.c_str());

    return;
}

lua_State* LuaState::State() const
{
	return mLuaState;
}

unsigned int LuaState::ItemsInStack()
{
	return lua_gettop(mLuaState);
}

bool LuaState::DoString(const char* str)
{
	// Does this leave the stack messy in case or error (probaby doesn't matter!)
	lua_pushcfunction(mLuaState, LuaState::LuaError);
	int fail = luaL_loadstring(mLuaState, str);

	if(fail)
	{
        // This is for syntax errors on loading the string.
        // The cfunction pushed above, is used by the pcall below
        // Not by loadstring
        OnError();
		return false;
	}
	else
	{
		// Execute the string on the stack
		// If anything goes wrong call the function under that
		bool result = lua_pcall(mLuaState, 0, LUA_MULTRET, -2) == 0;
		lua_pop(mLuaState, 1); // remove error function
		return result;
	}
}

bool LuaState::DoBuffer(const char* name, const char* buffer, unsigned int size)
{
    if(!buffer)
    {
        dsprintf("\n[LUASTATE|%s] Asset doesnt exist, no buffer: %s\n", mName, name);
        return false;
    }

    // This is pushed for the pcall below. (Doesn't do syntax errors)
    lua_pushcfunction(mLuaState, LuaState::LuaError);
    int fail = luaL_loadbuffer
    (
        mLuaState,
        buffer,
        size,
        name
    );

    if(fail)
    {
        // This is for syntax errors on loading the string.
        // The cfunction pushed above, is used by the pcall below
        // Not by loadstring
        OnError();
        return false;
    }
    else
    {
        // Execute the string on the stack
        // If anything goes wrong call the function under that
        bool result = lua_pcall(mLuaState, 0, LUA_MULTRET, -2) == 0;
        lua_pop(mLuaState, 1); // remove error function
        return result;
    }
}

bool LuaState::DoFile(const char* path)
{
    DDFile file(path);
    file.LoadFileIntoBuffer();
    return DoBuffer(path, file.Buffer(), file.Size());
}

std::string LuaState::GetString(const char* key, const char* defaultStr)
{
	lua_getfield(mLuaState, LUA_GLOBALSINDEX, key);
	const char* out = luaL_optlstring(mLuaState, -1, defaultStr, NULL);
	lua_pop(mLuaState, 1); // Remove key,
	return out;
}

bool LuaState::GetBoolean(const char* key, bool defaultBool)
{
    lua_getfield(mLuaState, LUA_GLOBALSINDEX, key);
    if(lua_isboolean(mLuaState, -1))
    {
        return lua_toboolean(mLuaState, -1);
    }
    return defaultBool;
}

int LuaState::GetInt(const char* key, int defaultInt)
{
	lua_getfield(mLuaState, LUA_GLOBALSINDEX, key);
	int out = luaL_optint (mLuaState, -1, defaultInt);
	lua_pop(mLuaState, 1); // Remove key,
	return out;
}

void LuaState::Reset()
{
    if(mLuaState)
    {
        lua_close(mLuaState);
        mLuaState = NULL;
    }
    mLuaState = lua_open();
    luaL_openlibs(mLuaState);
    // Push object instance pointer in the lua state
    // So for static functions (used by Lua) we can retrieve this object associated with a lua state

    _InjectIntoRegistry("this", this);

    for(std::vector<std::pair<std::string, void*> >::iterator
        iter = mInjections.begin();
        iter < mInjections.end();
        iter++)
    {
        _InjectIntoRegistry((*iter).first.c_str(), (*iter).second);
    }
}

void LuaState::CollectGarbage()
{
    lua_gc(mLuaState, LUA_GCCOLLECT, 0);
}

void LuaState::_InjectIntoRegistry(const char* key, void* object)
{
    lua_pushlstring(mLuaState, key, strlen(key));
    lua_pushlightuserdata(mLuaState, object);       /* push value */
    lua_settable(mLuaState, LUA_REGISTRYINDEX);     /* registry[key] = value */
}

void LuaState::InjectIntoRegistry(const char* key, void* object)
{
    _InjectIntoRegistry(key, object);
    // Injection need to be stored so that they may be reapplied if the lua
    // state is reset
    mInjections.push_back(std::pair<std::string, void*>
    (
        key,
        object
    ));
}

void* LuaState::GetFromRegistry(lua_State* state, const char* key)
{
    lua_pushlstring(state, key, strlen(key));
    // Needs sanity checks.
    lua_gettable(state, LUA_REGISTRYINDEX);  /* retrieve value */
    void* object = lua_touserdata(state, -1);
    lua_pop(state, 1); // pop the user data off the top of the stack
    return object;
}

void LuaState::Bind(const std::string& name, const luaL_Reg* luaBinding)
{
    const char* id = name.c_str();
    lua_newtable(mLuaState);
    luaL_newmetatable(mLuaState, id);
    luaL_register(mLuaState, NULL, luaBinding);
    lua_pushliteral(mLuaState, "__index");
    lua_pushvalue(mLuaState, -2);
    lua_rawset(mLuaState, -3);
    lua_setglobal(mLuaState, id);
}

const char* LuaState::GetParam(lua_State* state, int argNumber)
{
    if(!lua_isstring(state, argNumber))
    {
        luaL_typerror(state, argNumber, "string");
        return NULL;
    }
    return lua_tostring(state, argNumber);
}


// lua_checkudata causes a lua_error if not to checked type
// This function is basically lua_checkudata with out the error.
bool LuaState::IsType(lua_State* state, int arg, const char* type_name)
{
    if(NULL == lua_touserdata(state, arg))
    {
        return false;
    }

    // does it have a metatable?
    if(lua_getmetatable(state, arg))
    {
        // get correct metatable
        lua_getfield(state, LUA_REGISTRYINDEX, type_name);

        // Do metatables match?
        if(lua_rawequal(state, -1, -2))
        {
            // Remove metatables
            lua_pop(state, 2);
            return true;
        }
    }
    return false;
}

void LuaState::CallRegisteredFunction(int index)
{
    lua_pushcfunction(mLuaState, LuaState::LuaError);
    lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, index);
    lua_pcall(mLuaState, 0, 0, -2);
    lua_pop(mLuaState, 1); // remove error function
}

void LuaState::CallRegisteredFunction(int index, std::string& data)
{
    lua_pushcfunction(mLuaState, LuaState::LuaError);
    lua_rawgeti(mLuaState, LUA_REGISTRYINDEX, index);
    lua_pushstring(mLuaState, data.c_str());
    lua_pcall(mLuaState, 1, 0, -3);
    lua_pop(mLuaState, 1); // remove error function
}

