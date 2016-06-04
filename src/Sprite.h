#ifndef SPRITE_H
#define SPRITE_H

#include "reflect/Reflect.h"
#include "Vector.h"

class LuaState;
class Texture;

class Sprite
{
    public: static Reflect Meta;
    public:
        Texture* texture;
        Vector colour;
        Vector position;
        Vector scale;

        double topLeftU;
        double topLeftV;
        double bottomRightU;
        double bottomRightV;

        double rotation; // degrees

        static void Bind(LuaState* state);
        Sprite() { Init(); }
        void Init();
        void Init(const Sprite&);
        void SetPosition(const Vector&);
        void SetPosition(double x, double y);
        const Vector& GetPosition() const { return position; };
        void SetTexture(Texture*);
        void SetUVs(double topLeftU, double topLeftV, double bottomRightU, double bottomRightV);
        double GetRotation() const { return rotation; }
        void SetRotation(double value) { rotation = value; }
};

#endif