#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>
#include "reflect/Reflect.h"

class LuaState;

class SaveGame
{
    public: static Reflect Meta;
    private:
        std::string mName;
    public:
        static void Bind(LuaState* state);
        SaveGame(const char* name)
            : mName(name) {};
    const std::string& Name() const { return mName; }
    std::string ToString() const;
};

#endif