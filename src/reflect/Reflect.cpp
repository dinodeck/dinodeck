#include "Reflect.h"

#include <map>
#include <string>
#include "../DDLog.h"

//
// This is why we can't have nice things, C++.
// http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.15
//
std::map<std::string, Reflect*>& Reflect::Repo()
{
   static std::map<std::string, Reflect*>* ans = new std::map<std::string, Reflect*>();
   return *ans;
}

Reflect::Reflect(std::string name, void (*luaBindFunc) (LuaState*))
    : mName(name), mFields(), mStaticLuaBindFunc(luaBindFunc)
{

    // Add the class to the repository so it can be looked up by name.
    Reflect::Repo().insert
    (
        std::pair<std::string, Reflect*>(name, this)
    );
}

bool Reflect::Bind(const std::string& name, LuaState* state)
{
    // Get a pointer to the reflection obj
    std::map<std::string, Reflect*>::iterator iter = Reflect::Repo().find(name);

    if(iter == Reflect::Repo().end())
    {
        return false;
    }

    iter->second->CallBind(state);

    return true;
}

void Reflect::CallBind(LuaState* state)
{
    mStaticLuaBindFunc(state);
}