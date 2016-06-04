#ifndef DDLUACALLBACKS_H
#define DDLUACALLBACKS_H
//
// Stores Lua functions which are called via Java
// May also need to be used in windows, but that doesn't handling yet.
//
#include <string>
#include <map>

class LuaState;

class DDLuaCallbacks
{
    public:

    struct LuaCallback
    {
        // These are indices into
        // Lua's REF table, where the callback
        // functions have been stashed.
        int successRef;
        int failureRef;
        bool hasSuccessFired;
        bool hasFailureFired;

        LuaCallback() :
            successRef(-1), failureRef(-1),
            hasSuccessFired(false), hasFailureFired(false) {}

        LuaCallback(int successRef, int failureRef) :
            successRef(successRef), failureRef(failureRef),
            hasSuccessFired(false), hasFailureFired(false) {}
    };

    static std::map<int, LuaCallback> mCallbacks;
    static int StoreCallback(int successRef, int failureRef);
    static void OnSuccess(LuaState* state, int callRef, std::string& response);
    static void OnFailure(LuaState* state, int callRef, std::string& response);
    static void OnFinish(LuaState* state, int callRef);
};

#endif