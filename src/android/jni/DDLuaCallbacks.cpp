#include "DDLuaCallbacks.h"

#include <assert.h>
#include <string>

#include "../../DDLog.h"
#include "../../LuaState.h"
#include "../../DinodeckLua.h"

std::map<int, DDLuaCallbacks::LuaCallback> DDLuaCallbacks::mCallbacks;


int DDLuaCallbacks::StoreCallback(int successRef, int failureRef)
{
    // Find the next free number
    std::map<int,  DDLuaCallbacks::LuaCallback>::iterator iter;
    int number = 0;
    iter = mCallbacks.find(number);
    while(iter != mCallbacks.end())
    {
        number++;
        iter = mCallbacks.find(number);
    }

    mCallbacks.insert
    (
        std::pair<int, DDLuaCallbacks::LuaCallback>
        (
            number,
            DDLuaCallbacks::LuaCallback(successRef, failureRef)
        )
    );

    dsprintf("Stored callbacks (%d, %d) at: %d", successRef, failureRef, number);
    return number;
}

void DDLuaCallbacks::OnSuccess(LuaState* state, int callRef, std::string& response)
{
    std::map<int, DDLuaCallbacks::LuaCallback>::iterator iter = mCallbacks.find(callRef);
    assert(iter != mCallbacks.end());
    DDLuaCallbacks::LuaCallback& callback = iter->second;

    callback.hasSuccessFired = true;
    dsprintf("Callbackid: %d CallRegisteredFunction: %d, %s", callRef, callback.successRef, response.c_str());
    state->CallRegisteredFunction(callback.successRef, response);
}

void DDLuaCallbacks::OnFailure(LuaState* state, int callRef, std::string& response)
{
    std::map<int, DDLuaCallbacks::LuaCallback>::iterator iter = mCallbacks.find(callRef);
    assert(iter != mCallbacks.end());
    DDLuaCallbacks::LuaCallback& callback = iter->second;

    callback.hasFailureFired = true;
    dsprintf("Callbackid: %d CallRegisteredFunction: %d, %s", callRef, callback.successRef, response.c_str());
    state->CallRegisteredFunction(callback.failureRef, response);
    dsprintf("After lua callback");
}

void DDLuaCallbacks::OnFinish(LuaState* state, int callRef)
{
    std::map<int, DDLuaCallbacks::LuaCallback>::iterator iter = mCallbacks.find(callRef);
    assert(iter != mCallbacks.end());
    DDLuaCallbacks::LuaCallback& callback = iter->second;

    if(false == callback.hasFailureFired && false == callback.hasSuccessFired)
    {
        // Fire failure.
        dsprintf("Neither callback fired. Firing failure.\n");
        state->CallRegisteredFunction(callback.failureRef);
    }

    // Kill off these references
    luaL_unref(state->State(), LUA_REGISTRYINDEX, callback.successRef);
    luaL_unref(state->State(), LUA_REGISTRYINDEX, callback.failureRef);

    // Remove it
    mCallbacks.erase(iter);

    dsprintf("End of OnFinish");
}