#ifndef DDTIME_H
#define DDTIME_H

#include "reflect/Reflect.h"
#include <string>

class LuaState;

class DDTime
{
    public: static Reflect Meta;
    public:
        static void Bind(LuaState* state);
};
#endif