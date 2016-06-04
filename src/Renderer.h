#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "Vector.h"

#include "reflect/Reflect.h"
#include "LuaState.h"
#include "DDTextAlign.h"


struct lua_State;
class Sprite;
class GraphicsPipeline;




class Renderer
{
    public: static Reflect Meta;
    public:
        static void Bind(LuaState* state);
        static const char* AlignXStr[AlignX::Count];
        static const char* AlignYStr[AlignY::Count];
        static const float TextScale;
        static std::vector<Renderer*> mRenderers;

        void DrawSprite(const Sprite&);
        void DrawRect2d(const Vector& bottomLeft,
                        const Vector& topRight,
                        const Vector& colour);
        void DrawCircle2d(double x, double y, double radius, int segments,
                          const Vector& rgba);
        void DrawLine2d(const Vector& start, const Vector& end,
                        const Vector& colour);
        Renderer();
        ~Renderer();
        GraphicsPipeline* Graphics() { return mGraphics; }
    private:

        Vector size;
        GraphicsPipeline* mGraphics;
};

#endif