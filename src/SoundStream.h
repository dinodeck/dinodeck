#ifndef SOUNDSTREAM_H
#define SOUNDSTREAM_H

#include <string>
#include <map>
#include <vector>

#include "reflect/Reflect.h"
#include "DDAudio.h"

class LuaState;

class SoundStream
{
public: static Reflect Meta;
public:
    static void Bind(LuaState* state);
};

#endif