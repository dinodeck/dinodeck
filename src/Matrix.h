
#ifndef MATRIX_H
#define MATRIX_H

#include "reflect/Reflect.h"
#include "Vector.h"

class LuaState;

class Matrix
{
 public: static Reflect Meta;
    Vector mCol0;
    Vector mCol1;
    Vector mCol2;
    Vector mCol3;

 public:
    static Matrix Identity();
    static Matrix Translation(const Vector& translateVec);
    static void Multiply(Matrix& destination, const Matrix& left, const Matrix& right);
    static void Multiply(Vector& destination, const Matrix& left, const Vector& right);
    static void Multiply(Matrix& destination, const Matrix& left, double right);
    static void Bind(LuaState* state);

    Matrix();
    Matrix(const Matrix& matrix);
    Matrix(const Vector& col0,
           const Vector& col1,
           const Vector& col2,
           const Vector& col3);




    Vector operator *(const Vector& vec) const;
    Matrix operator *(const Matrix& mat) const;

//
    Matrix& SetTranslation(const Vector& translateVec);
    // Set this matrix to be roated around the axis by the angle (degrees)
    Matrix& SetRotation(const Vector& axis, float angle);
    Matrix& SetScale(float x, float y, float z);
    Matrix& SetScale(const Vector& s) { return SetScale(s.x, s.y, s.z); }
    Matrix& SetOrtho(float left, float right, float bottom, float top, float near, float far);

    void SetColumns(
            const Vector& col0,
            const Vector& col1,
            const Vector& col2,
            const Vector& col3);
    const Vector& GetCol0() const;
    const Vector& GetCol1() const;
    const Vector& GetCol2() const;
    const Vector& GetCol3() const;
    void SetColumn(int index, float x, float y, float z, float w);
    void SetColumn(int index, const Vector& column);
    std::string ToString() const;
};

#endif