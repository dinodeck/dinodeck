#ifndef VECTOR_H
#define VECTOR_H

#include "reflect/Reflect.h"
#include <string>

class LuaState;
struct lua_State;

class Vector
{
    public: static Reflect Meta;
    public:
        static int lua_Vector_Create(lua_State * state);
        static void Bind(LuaState* state);
        static void Add(Vector& destination, const Vector& left, const Vector& right);
        static void Multiply(Vector& destination, const Vector& left, const Vector& right);
        static void Multiply(Vector& destination, const Vector& left, double right);
        static void Subtract(Vector& destination, const Vector& left, const Vector& right);
        static void Divide(Vector& destination, const Vector& left, const Vector& right);
        static void MultiplyAdd(Vector& destination, const Vector& left, const Vector& right, const Vector& add);
        static void CrossProduct(Vector& destination, const Vector& left, const Vector& right);
        static bool Equal(const Vector& left, const Vector& right, float precision);

        Vector()
            : x(0), y(0), z(0), w(0) {}
        Vector(double x, double y, double z, double w)
            : x(x), y(y), z(z), w(w) {}
        Vector(const Vector& v)
            : x(v.x), y(v.y), z(v.z), w(v.w) {}
        double x;
        double y;
        double z;
        double w;
        void SetXyzw(const Vector& vector);
        void SetXyzw(double x, double y, double z, double w);
        void SetBroadcast(double number) { SetXyzw(number, number, number, number);}
        double Length2() const;
        double Length3() const;
        double Length4() const;
        double Dot2(const Vector& v) const;
        double Dot3(const Vector& v) const;
        double Dot4(const Vector& v) const;
        void Normalize2();
        void Normalize3();
        void Normalize4();
        void SetX(double x) { this->x = x; }
        void SetY(double y) { this->y = y; }
        void SetZ(double z) { this->z = z; }
        void SetW(double w) { this->w = w; }
        std::string ToString() const;
        Vector operator *(const Vector& vec) const;
        Vector operator *(double value) const;

        // Does a normal calculation to check
        bool IsNormal3() const;

    public:
        static Vector AxisX;
        static Vector AxisY;
        static Vector AxisZ;
        static Vector AxisW;
};

#endif