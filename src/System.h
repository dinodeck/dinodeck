#ifndef SYSTEM_H
#define SYSTEM_H

#include "reflect/Reflect.h"

class LuaState;

class System
{
    public: static Reflect Meta;
    public:
        static void Bind(LuaState* state);
};

#endif