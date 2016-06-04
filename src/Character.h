#ifndef CHARACTER_H
#define CHARACTER_H

#include "DinodeckGL.h"
#include "reflect/Reflect.h"

#ifdef ANDROID
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif

class LuaState;
class GraphicsPipeline;

class Character
{
    public: static Reflect Meta;
        float mX;
        float mY;
        char mGlyph; // a char[3] might be better.
    public:
        static void Bind(LuaState* state);
        Character();
        ~Character();

        char GetGlyph() const { return mGlyph; }
        void SetGlyph(char glyph) { mGlyph = glyph; }

        void DrawGlyph(GraphicsPipeline* gp);
        int GetPixelWidth(GraphicsPipeline* gp);
        int GetPixelHeight(GraphicsPipeline* gp);
        static int GetMaxHeight(GraphicsPipeline* gp);
        FTPoint GetCorner(GraphicsPipeline* gp);
        static float GetKern(GraphicsPipeline* gp, unsigned int current, unsigned int next);
};

#endif