#ifndef HTTP_H
#define HTTP_H

#include "reflect/Reflect.h"

class LuaState;

// Http a lua class for doing RESTful type communications with the net.
class Http
{
    public: static Reflect Meta;
    public: static void Bind(LuaState* state);
};

#endif