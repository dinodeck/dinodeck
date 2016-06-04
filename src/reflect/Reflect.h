#ifndef REFLECT_H
#define REFLECT_H

#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "Field.h"

class LuaState;

/*
    Some simple reflection abilities that can be added to a class.
    This used to be quite pure reflection but I struggled with extending
    is that way (typed static member functions and typed member functions)
    So now it knows Lua binding which is fine for dancing squid.
*/
class Reflect
{
private:
    std::string mName;
    std::vector< Field* > mFields;
    void (*mStaticLuaBindFunc) (LuaState*);
public:

    static std::map<std::string, Reflect*>& Repo();

    Reflect(std::string name, void (*) (LuaState*));

    static bool Bind(const std::string& name, LuaState* state);

    void CallBind(LuaState* state);

    template<typename T>
    void Print(T* instance)
    {
        typename std::vector< Field* >::iterator iter;
        for (iter = mFields.begin();
             iter != mFields.end(); ++iter)
        {
            printf("%s\n", (*iter)->ToString((void*)instance).c_str());
        }
    }

    template <typename CLASS, typename TYPE>
    void AddField(const char* name, TYPE CLASS::* fieldPtr)
    {
        mFields.push_back(new FieldImpl<CLASS, TYPE>(name, fieldPtr));
    }

    const std::string& Name() const { return mName; }

    Field* GetField(const char* name) const
    {
        for (std::vector<Field*>::const_iterator iter = mFields.begin(); iter != mFields.end(); ++iter)
        {
            if((*iter)->Name() == name)
            {
                return (*iter);
            }
        }
        return NULL;
    }
};


#endif