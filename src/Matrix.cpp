#include "Matrix.h"

#include <assert.h>
#include <cmath>

#include "reflect/Reflect.h"
#include "Vector"
#include "DDLog.h"
#include "LuaState.h"
#include "DinodeckGL.h"
#include "DDMath.h"


Reflect Matrix::Meta("Matrix", Matrix::Bind);

Matrix Matrix::Identity()
{
    return Matrix
    (
        Vector::AxisX,
        Vector::AxisY,
        Vector::AxisZ,
        Vector::AxisW
    );
}

Matrix::Matrix()
    :   mCol0(Vector::AxisX),
        mCol1(Vector::AxisY),
        mCol2(Vector::AxisZ),
        mCol3(Vector::AxisW)
{}

Matrix::Matrix(const Matrix& matrix)
    :
        mCol0(matrix.mCol0),
        mCol1(matrix.mCol1),
        mCol2(matrix.mCol2),
        mCol3(matrix.mCol3)
{}

Matrix::Matrix(const Vector& col0,
               const Vector& col1,
               const Vector& col2,
               const Vector& col3)
    :   mCol0(col0),
        mCol1(col1),
        mCol2(col2),
        mCol3(col3)
{}

void Matrix::Multiply(Matrix& destination, const Matrix& left, const Matrix& right)
{
    destination.SetColumns
    (
        left * right.mCol0,
        left * right.mCol1,
        left * right.mCol2,
        left * right.mCol3
    );
}

void Matrix::Multiply(Vector& destination, const Matrix& left, const Vector& right)
{
    // value ares worked out long form because destination may equal right
    destination.SetXyzw
    (
        ( ( ( ( left.mCol0.x * right.x ) + ( left.mCol1.x * right.y ) ) + ( left.mCol2.x * right.z ) ) + ( left.mCol3.x * right.w ) ),
        ( ( ( ( left.mCol0.y * right.x ) + ( left.mCol1.y * right.y ) ) + ( left.mCol2.y * right.z ) ) + ( left.mCol3.y * right.w ) ),
        ( ( ( ( left.mCol0.z * right.x ) + ( left.mCol1.z * right.y ) ) + ( left.mCol2.z * right.z ) ) + ( left.mCol3.z * right.w ) ),
        ( ( ( ( left.mCol0.w * right.x ) + ( left.mCol1.w * right.y ) ) + ( left.mCol2.w * right.z ) ) + ( left.mCol3.w * right.w ) )
    );
}

void Matrix::Multiply(Matrix& destination, const Matrix& left, double right)
{
    destination.SetColumns
    (
        left.mCol0 * right,
        left.mCol1 * right,
        left.mCol2 * right,
        left.mCol3 * right
    );
}


Vector Matrix::operator *(const Vector& vec) const
{
    Vector destination;
    Matrix::Multiply(destination, *this, vec);
    return destination;
}

Matrix Matrix::operator *(const Matrix& mat) const
{
    Matrix destination;
    Matrix::Multiply(destination, *this, mat);
    return destination;
}


Matrix Matrix::Translation(const Vector& translateVec)
{
    return Matrix
    (
        Vector::AxisX,
        Vector::AxisY,
        Vector::AxisZ,
        Vector(translateVec.x, translateVec.y, translateVec.z, 1)
    );
}

Matrix& Matrix::SetTranslation(const Vector& translateVec)
{
    mCol3.SetXyzw(translateVec);
    mCol3.w = 1;
    return *this;
}

Matrix& Matrix::SetRotation(const Vector& axis, float angle)
{
    // Need to normalise it, if the axis isn't normal.
    assert(axis.IsNormal3());
    float c = cos(DegreeToRadian(angle));
    float s = sin(DegreeToRadian(angle));

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    mCol0.SetXyzw((x * x) * (1 - c) + c,        (x * y) * (1 - c) + (z * s),    (x * z) * (1 - c) - (y * s),    mCol0.w);
    mCol1.SetXyzw((x * y) * (1 - c) - (z * s),  ((y * y) * (1 - c)) + c,        ((y * z) * (1 - c)) + (x * s),  mCol1.w);
    mCol2.SetXyzw((x * z) * (1 - c) + (y * s),  (y * z) * (1 - c) - (x * s),    (z * z) * (1 - c) + c,          mCol2.w);

    return *this;
}

Matrix& Matrix::SetScale(float x, float y, float z)
{
    // (
    //      x   0   0   0
    //      0   y   0   0
    //      0   0   z   0
    //      0   0   0   1
    // )
    mCol0.x = x;
    mCol1.y = y;
    mCol2.z = z;
    mCol3.w = 1;
    return *this;
}

Matrix& Matrix::SetOrtho(float left, float right, float bottom, float top, float near_, float far_)
{

    //
    // Why far_ and near_ because C++ is terrible (glee.h's fault).
    //

    // (
    //      2/(right - left)    0   0   tx
    //      0   2/(top - bottom)    0   ty
    //      0   0   ( -2)/(far_ - near_)  tz
    //      0   0   0   1
    // )
    //
    // where
    //
    float tx = - (right + left) / (right - left);
    float ty = - (top + bottom) / (top - bottom);
    float tz = - (far_ + near_) / (far_ - near_);
    //

    mCol0.SetXyzw(2/(right - left) , 0, 0, 0);
    mCol1.SetXyzw(0, 2/(top - bottom), 0, 0);
    mCol2.SetXyzw(0, 0, ( -2)/(far_ - near_), 0);
    mCol3.SetXyzw(tx, ty, tz, 1);

    return *this;
}

const Vector& Matrix::GetCol0() const
{
    return mCol0;
}

const Vector& Matrix::GetCol1() const
{
    return mCol1;
}

const Vector& Matrix::GetCol2() const
{
    return mCol2;
}

const Vector& Matrix::GetCol3() const
{
    return mCol3;
}

void Matrix::SetColumns(
        const Vector& col0,
        const Vector& col1,
        const Vector& col2,
        const Vector& col3)
{
    mCol0.SetXyzw(col0);
    mCol1.SetXyzw(col1);
    mCol2.SetXyzw(col2);
    mCol3.SetXyzw(col3);
}

std::string Matrix::ToString() const
{
    std::stringstream ss;
    ss <<
        mCol0.x << "\t" << mCol1.x  << "\t" << mCol2.x  << "\t" << mCol3.x  << "\n" <<
        mCol0.y << "\t" << mCol1.y  << "\t" << mCol2.y  << "\t" << mCol3.y  << "\n" <<
        mCol0.z << "\t" << mCol1.z  << "\t" << mCol2.z  << "\t" << mCol3.z  << "\n" <<
        mCol0.w << "\t" << mCol1.w  << "\t" << mCol2.w  << "\t" << mCol3.w  << "\n";
    return ss.str();
}

void Matrix::SetColumn(int index, float x, float y, float z, float w)
{
    assert(index >= 1);
    assert(index <= 4);

    if(index == 1)
    {
        mCol0.SetXyzw(x, y, z, w);
    }
    else if(index == 2)
    {
        mCol1.SetXyzw(x, y, z, w);
    }
    else if(index == 3)
    {
        mCol2.SetXyzw(x, y, z, w);
    }
    else
    {
        mCol3.SetXyzw(x, y, z, w);
    }
}

void Matrix::SetColumn(int index, const Vector& column)
{
    assert(index >= 1);
    assert(index <= 4);

    if(index == 1)
    {
        mCol0.SetXyzw(column);
    }
    else if(index == 2)
    {
        mCol1.SetXyzw(column);
    }
    else if(index == 3)
    {
        mCol2.SetXyzw(column);
    }
    else
    {
        mCol3.SetXyzw(column);
    }
}

int lua_Matrix_Create(lua_State * state)
{
    // If there's inheritance I think this may causes problems
    if(lua_isuserdata(state, 1) && luaL_checkudata(state, 1, "Matrix"))
    {
        Matrix* matrix = (Matrix*)lua_touserdata(state, 1);
        if(matrix == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        new (lua_newuserdata(state, sizeof(Matrix))) Matrix(*matrix);
    }
    else if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* row0 = LuaState::GetFuncParam<Vector>(state, 1);
        Vector* row1 = LuaState::GetFuncParam<Vector>(state, 2);
        Vector* row2 = LuaState::GetFuncParam<Vector>(state, 3);
        Vector* row3 = LuaState::GetFuncParam<Vector>(state, 4);

        if (!(row0 && row1 && row2 && row3))
        {
            // There's been an error.
            return 0;
        }
        new (lua_newuserdata(state, sizeof(Matrix))) Matrix
        (
            (*row0),
            (*row1),
            (*row2),
            (*row3)
        );
    }
    else
    {
        // tonumber returns 0 if it fails.
        new (lua_newuserdata(state, sizeof(Matrix))) Matrix();
    }
    luaL_getmetatable(state, "Matrix");
    lua_setmetatable(state, -2);
    return 1;
}

int lua_Matrix_tostring(lua_State* state)
{
    Matrix* matrix = (Matrix*)lua_touserdata(state, 1);
    if(matrix == NULL)
    {
        // to string on the metatable itself
        lua_pushstring(state, "Matrix");
    }
    else
    {
        lua_pushstring(state, matrix->ToString().c_str());
    }

    return 1;
}

static int lua_Matrix_multiply_operator(lua_State* state)
{
    if(!LuaState::IsType(state, 1, "Matrix"))
    {
        return luaL_typerror(state, 1, "Matrix expected.");
    }

    Matrix* lhs = static_cast<Matrix*>(lua_touserdata(state, 1));

    if(LuaState::IsType(state, 2, "Vector"))
    {
        Vector* rhs = static_cast<Vector*>(lua_touserdata(state, 2));
        Vector* destination = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        luaL_getmetatable(state, "Vector");
        lua_setmetatable(state, -2);
        Matrix::Multiply(*destination, *lhs, *rhs);
    }
    else if(LuaState::IsType(state, 2, "Matrix"))
    {
        Matrix* rhs = static_cast<Matrix*>(lua_touserdata(state, 2));
        Matrix* destination = new (lua_newuserdata(state, sizeof(Matrix))) Matrix();
        luaL_getmetatable(state, "Matrix");
        lua_setmetatable(state, -2);
        Matrix::Multiply(*destination, *lhs, *rhs);
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or Matrix expected.");
    }

    return 1;
}

static int lua_Matrix_SetColumn(lua_State* state)
{
    Matrix* matrix = LuaState::GetFuncParam<Matrix>(state, 1);
    if(matrix == NULL)
    {
        return 0;
    }
    // Row index
    if(!lua_isnumber(state, 2))
    {
        return luaL_typerror(state, 2, "number expected.");
    }
    int columnIndex = (int)lua_tonumber(state, 2);

    // Indices the 4 columns
    if(columnIndex < 1 or columnIndex > 4)
    {
        return luaL_typerror(state, 2, "column index should be 1-4");
    }

    if(lua_isnumber(state, 3))
    {
        float x = lua_tonumber(state, 3);
        float y = lua_tonumber(state, 4);
        float z = lua_tonumber(state, 5);
        float w = lua_tonumber(state, 6);
        matrix->SetColumn(columnIndex, x, y, z, w);
        return 0;
    }
    else if(lua_isuserdata(state, 3) && LuaState::IsType(state, 3, "Vector"))
    {
        Vector* vec = LuaState::GetFuncParam<Vector>(state, 3);
        matrix->SetColumn(columnIndex, *vec);
        return 0;
    }
    else
    {
        return luaL_typerror(state, 3, "Vector or Number expected.");
    }
}

static int lua_Matrix_SetRotation(lua_State* state)
{
    Matrix* matrix = LuaState::GetFuncParam<Matrix>(state, 1);
    if(matrix == NULL)
    {
        return 0;
    }

    Vector* axis = NULL;
    if(lua_isuserdata(state, 2) && luaL_checkudata(state, 2, "Vector"))
    {
       axis = LuaState::GetFuncParam<Vector>(state, 2);
       assert(axis);
    }
    else
    {
        return luaL_typerror(state, 2, "Vector");
    }

    if(!lua_isnumber(state, 3))
    {
        return luaL_typerror(state, 3, "number");
    }

    float angle = (float) lua_tonumber(state, 3);
    matrix->SetRotation(*axis, angle);
    return 0;
}

static int lua_Matrix_SetScale(lua_State* state)
{
    Matrix* matrix = LuaState::GetFuncParam<Matrix>(state, 1);
    if(matrix == NULL)
    {
        return 0;
    }

    if(lua_isuserdata(state, 2) && luaL_checkudata(state, 2, "Vector"))
    {
       Vector* vector = LuaState::GetFuncParam<Vector>(state, 2);
       assert(vector);
       matrix->SetScale
       (
           vector->x,
           vector->y,
           vector->z
       );
    }
    else
    {
        return luaL_typerror(state, 2, "Vector");
    }
    return 0;
}

static int lua_Matrix_SetTranslate(lua_State* state)
{
    Matrix* matrix = LuaState::GetFuncParam<Matrix>(state, 1);
    if(matrix == NULL)
    {
        return 0;
    }

    if(lua_isuserdata(state, 2) && luaL_checkudata (state, 2, "Vector"))
    {
       Vector* vector = LuaState::GetFuncParam<Vector>(state, 2);
       assert(vector);
       matrix->SetTranslation(*vector);
    }
    else
    {
        return luaL_typerror(state, 2, "Vector");
    }
    return 0;
}

static int lua_Matrix_Multiply(lua_State* state)
{
    Matrix* matrix = LuaState::GetFuncParam<Matrix>(state, 1);
    if(matrix == NULL)
    {
        return 0;
    }

    // Not support for M * V here, because it's returns a Vector.

    // This is a logical XOR, is one of the params a number but not both.
    // 1 != 1 = false
    // 0 != 1 = true
    // etc
    if(lua_isnumber(state, 2) != lua_isnumber(state, 3))
    {
        // scalar matrix multiplication is commutative, so order doesn't matter
        // here - and we can just use on function.
        if(LuaState::IsType(state, 2, Matrix::Meta.Name().c_str()))
        {
            // m * s
            Matrix::Multiply
            (
                (*matrix),
                (*LuaState::GetFuncParam<Matrix>(state, 2)),
                static_cast<double>(lua_tonumber(state, 3))
            );
        }
        else if(LuaState::IsType(state, 3, Matrix::Meta.Name().c_str()))
        {
            // s * m
            Matrix::Multiply
            (
                (*matrix),
                (*LuaState::GetFuncParam<Matrix>(state, 3)), // NOTE: arg numbers reversed.
                static_cast<double>(lua_tonumber(state, 2))
            );
        }
        else
        {
            return luaL_error(state, "Argument 2 or 3 must be of type Matrix.");
        }

    }
    else if(LuaState::IsType(state, 2, Matrix::Meta.Name().c_str()) &&
            LuaState::IsType(state, 3, Matrix::Meta.Name().c_str()))
    {
        // Assume both matrices
        Matrix::Multiply
        (
            (*matrix),
            (*LuaState::GetFuncParam<Matrix>(state, 2)),
            (*LuaState::GetFuncParam<Matrix>(state, 3))
        );
    }
    else
    {
        // Return correct parameter error.
        // First param can be a number of Matrix but not a Vector.
        if(!lua_isnumber(state, 2) || !LuaState::IsType(state, 2, Matrix::Meta.Name().c_str()))
        {
            return luaL_typerror(state, 2, "number or Matrix");
        }
        else
        {
            return luaL_typerror(state, 3, "number or Matrix");
        }
    }
    return 0;
}

static const struct luaL_reg luaBinding [] = {
    {"Create", lua_Matrix_Create},
    {"__tostring", lua_Matrix_tostring},
    {"__mul", lua_Matrix_multiply_operator},
    {"Multiply", lua_Matrix_Multiply},
    {"SetColumn", lua_Matrix_SetColumn},
    {"SetRotation", lua_Matrix_SetRotation},
    {"SetScale", lua_Matrix_SetScale},
    {"SetTranslate", lua_Matrix_SetTranslate},
    {NULL, NULL}  /* sentinel */
};

void Matrix::Bind(LuaState* state)
{
    state->Bind
    (
        Matrix::Meta.Name(),
        luaBinding
    );
}


