#ifndef SOUND_H
#define SOUND_H

#include <string>
#include <map>
#include <vector>

#include "reflect/Reflect.h"
#include "DDAudio.h"

class LuaState;

class Sound
{
public: static Reflect Meta;
private:
    static DDAudio Audio;
public:
    static void Bind(LuaState* state);
};

#endif
