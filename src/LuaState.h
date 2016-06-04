#ifndef LUASTATE_H
#define LUASTATE_H

#include <string>
#include <vector>

#include "DinodeckLua.h"

class LuaState
{
private:
	const char* mName;
	lua_State* mLuaState;
	std::vector<std::pair<std::string, void*> > mInjections;
    std::string mLastError;
    std::string mLastErrorCallstack;

	// Lua static C style callback to be routed to LuaState's instance OnError function
	static int LuaError(lua_State* luaState);

	void OnError();
	// Doesn't try and cache the injection
	void _InjectIntoRegistry(const char* key, void* value);
public:
	// Get lua_State's wrapper instance of class LuaState
	static LuaState* GetWrapper(lua_State* luaState);
	static void* GetFromRegistry(lua_State* state, const char* key);
	template <class T>
	static T* GetFuncParam(lua_State* state, int argNumber);
	template <class T>
	static T** GetFuncParamPtr(lua_State* state, int argNumber);
	static const char* GetParam(lua_State* state, int argNumber);
    static bool IsType(lua_State* state, int arg, const char* tname);


	LuaState(const char* name);
	~LuaState();

	// Execute lua string, on error will print error and stacktrace if possible.
	bool DoString(const char* str);
	bool DoFile(const char* filepath);
	bool DoBuffer(const char* name, const char* buffer, unsigned int size);

	// Not sure about this approach.
	int GetInt(const char* key, int defaultInt);
	lua_State* State() const;
	std::string GetString(const char* key, const char* defaultStr);
    bool GetBoolean(const char* key, bool defaultBool);
	unsigned int ItemsInStack();
	void Bind(const std::string& name, const luaL_Reg* luaBinding);
	void CollectGarbage();
	void InjectIntoRegistry(const char* key, void* value);
    void CallRegisteredFunction(int index);
    void CallRegisteredFunction(int index, std::string& data);

	void* GetFromRegistry(const char* key)
	{
		return GetFromRegistry(mLuaState, key);
	}
	void Reset();

    const std::string& GetLastError() const { return mLastError; }
    const std::string& GetLastErrorCallstack() const { return mLastErrorCallstack; }
};



template <class T>
T* LuaState::GetFuncParam(lua_State* state, int argNumber)
{
    if(!lua_isuserdata(state, argNumber) || !luaL_checkudata (state, argNumber,  T::Meta.Name().c_str()))
    {
        luaL_typerror(state, argNumber, T::Meta.Name().c_str());
        return NULL;
    }
    T* t = reinterpret_cast<T*>(lua_touserdata(state, argNumber));
    return t;
}

template <class T>
T** LuaState::GetFuncParamPtr(lua_State* state, int argNumber)
{
    if(!lua_isuserdata(state, argNumber) || !luaL_checkudata (state, argNumber,  T::Meta.Name().c_str()))
    {
        luaL_typerror(state, argNumber, T::Meta.Name().c_str());
        return NULL;
    }
    T** t = reinterpret_cast<T**>(lua_touserdata(state, argNumber));
    return t;
}

#endif
