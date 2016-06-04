#include "Vector.h"

#include <assert.h>
#include <cmath>
#include <sstream>
#include <string>

#include "DinodeckLua.h"
#include "LuaState.h"
#include "reflect/Reflect.h"

using namespace std;

Reflect Vector::Meta("Vector", Vector::Bind);

Vector Vector::AxisX = Vector(1.0f, 0.0f, 0.0f, 0.0f);
Vector Vector::AxisY = Vector(0.0f, 1.0f, 0.0f, 0.0f);
Vector Vector::AxisZ = Vector(0.0f, 0.0f, 1.0f, 0.0f);
Vector Vector::AxisW = Vector(0.0f, 0.0f, 0.0f, 1.0f);

int Vector::lua_Vector_Create(lua_State * state)
{
    // If there's inheritance I think this may causes problems
    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 1);
        Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        if(vector == NULL)
        {
            return luaL_argerror(state, 1, "Expected Vector, number or nil");
        }
        pi->SetXyzw(*vector);
    }
    else
    {
        // tonumber returns 0 if it fails.
        Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        pi->x = lua_tonumber(state, 1);
        pi->y = lua_tonumber(state, 2);
        pi->z = lua_tonumber(state, 3);
        pi->w = lua_tonumber(state, 4);
    }
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    return 1;
}

int lua_Vector_tostring(lua_State * state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if(vector == NULL)
    {
        // to string on the metatable itself
        lua_pushstring(state, "Vector");
    }
    else
    {
        lua_pushstring(state, vector->ToString().c_str());
    }

    return 1;
}

// These are all very similar.
// There must be some template magic that can sort them.

int lua_Vector_X(lua_State* state)
{
    // First arg is self
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    lua_pushnumber(state, vector->x);
    return 1;
}

int lua_Vector_Y(lua_State* state)
{
    // First arg is self
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        luaL_typerror(state, 1, "Vector");
        return 0;
    }
    lua_pushnumber(state, vector->y);
    return 1;
}

int lua_Vector_Z(lua_State* state)
{
    // First arg is self
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        luaL_typerror(state, 1, "Vector");
        return 0;
    }
    lua_pushnumber(state, vector->z);
    return 1;
}

int lua_Vector_W(lua_State* state)
{
    // First arg is self
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        luaL_typerror(state, 1, "Vector");
        return 0;
    }
    lua_pushnumber(state, vector->w);
    return 1;
}

int lua_Vector_SetX(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }
    lua_Number number = lua_tonumber(state, 2);
    vector->x = number;
    return 0;
}

int lua_Vector_SetY(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }
    lua_Number number = lua_tonumber(state, 2);
    vector->y = number;
    return 0;
}

int lua_Vector_SetZ(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }
    lua_Number number = lua_tonumber(state, 2);
    vector->z = number;
    return 0;
}

int lua_Vector_SetW(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }
    lua_Number number = lua_tonumber(state, 2);
    vector->w = number;
    return 0;
}

int lua_Vector_SetXyzw(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }

    vector->x = lua_tonumber(state, 2);
    if(lua_isnumber(state, 3) == false)
    {
        return 0;
    }
    vector->y = lua_tonumber(state, 3);
    if(lua_isnumber(state, 4) == false)
    {
        return 0;
    }
    vector->z = lua_tonumber(state, 4);
    if(lua_isnumber(state, 5) == false)
    {
        return 0;
    }
    vector->w = lua_tonumber(state, 5);
    return 0;
}

//
// This data is used by add, multiply, subtract so I broke it out into it's own function.
// destination = left [op] right
// I'm happy to replace this function with anything better.
//

int lua_get_infix_operation_data(lua_State* state, Vector*& leftVector, Vector*& rightVector)
{
    static Vector defaultLeftVector;
    static Vector defaultRightVector;

    leftVector = NULL;
    if(lua_isuserdata(state, 1))
    {
        if(!luaL_checkudata (state, 1, "Vector"))
        {
            return luaL_typerror(state, 1, "Vector");
        }
        leftVector = (Vector*)lua_touserdata(state, 1);
        assert(leftVector);
    }
    else if(lua_isnumber(state, 1))
    {
        double number = lua_tonumber(state, 1);
        defaultLeftVector.SetBroadcast(number);
        leftVector = &defaultLeftVector;
    }
    else
    {
        return luaL_typerror(state, 1, "Vector or number");
    }

    rightVector = NULL;
    if(lua_isuserdata(state, 2))
    {
        if(!luaL_checkudata (state, 2, "Vector"))
        {
            return luaL_typerror(state, 2, "Vector");
        }
        rightVector = (Vector*)lua_touserdata(state, 2);
        assert(rightVector);
    }
    else if(lua_isnumber(state, 2))
    {
        double number = lua_tonumber(state, 2);
        defaultRightVector.SetBroadcast(number);
        rightVector = &defaultRightVector;
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or number");
    }
    return 1;
}

int lua_get_binary_operation_data(lua_State* state, Vector*& destination, Vector*& leftVector, Vector*& rightVector)
{
    // There are no threads but if there were this would of course, blow up.
    static Vector defaultLeftVector;
    static Vector defaultRightVector;
    if (!lua_isuserdata(state, 1) || !luaL_checkudata (state, 1, "Vector"))
    {
        return luaL_typerror(state, 1, "Vector");
    }
    destination = (Vector*)lua_touserdata(state, 1);
    assert(destination);

    // This can be a single number or vector

    if(lua_isuserdata(state, 2))
    {
        if(!luaL_checkudata (state, 2, "Vector"))
        {
            return luaL_typerror(state, 2, "Vector");
        }
        leftVector = (Vector*)lua_touserdata(state, 2);
        assert(leftVector);
    }
    else if(lua_isnumber(state, 2))
    {
        double number = lua_tonumber(state, 2);
        defaultLeftVector.SetBroadcast(number);
        leftVector = &defaultLeftVector;
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or number");
    }

    if(lua_isuserdata(state, 3))
    {
        if(!luaL_checkudata (state, 3, "Vector"))
        {
            return luaL_typerror(state, 3, "Vector");
        }
        rightVector = (Vector*)lua_touserdata(state, 3);
        assert(rightVector);
    }
    else if(lua_isnumber(state, 3))
    {
        double number = lua_tonumber(state, 3);
        defaultRightVector.SetBroadcast(number);
        rightVector = &defaultRightVector;
    }
    else
    {
        return luaL_typerror(state, 3, "Vector or number");
    }
    assert(destination);
    assert(leftVector);
    assert(rightVector);
    return 1;
}

int lua_Vector_Add(lua_State* state)
{

    Vector* destination = NULL;
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;

    int result = lua_get_binary_operation_data
    (
        state,
        destination,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }

    assert(destination);
    assert(leftVector);
    assert(rightVector);
    Vector::Add(*destination, *leftVector, *rightVector);

    return 0;
}

int lua_Vector_Subtract(lua_State* state)
{

    Vector* destination = NULL;
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;

    int result = lua_get_binary_operation_data
    (
        state,
        destination,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }

    Vector::Subtract(*destination, *leftVector, *rightVector);

    return 0;
}

int lua_Vector_Multiply(lua_State* state)
{
    Vector* destination = NULL;
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;

    int result = lua_get_binary_operation_data
    (
        state,
        destination,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }

    Vector::Multiply(*destination, *leftVector, *rightVector);

    return 0;
}

int lua_Vector_Divide(lua_State* state)
{
    Vector* destination = NULL;
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;

    int result = lua_get_binary_operation_data
    (
        state,
        destination,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }


    if(rightVector->x == 0
    || rightVector->y == 0
    || rightVector->z == 0
    || rightVector->w == 0)
    {
        return luaL_error(state, "Divide by zero.");
    }
    Vector::Divide(*destination, *leftVector, *rightVector);

    return 0;
}

int lua_Vector_MultiplyAdd(lua_State* state)
{

    Vector* destination = NULL;
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;

    int result = lua_get_binary_operation_data
    (
        state,
        destination,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }

    Vector defaultAddVector;
    Vector* addVector = NULL;
    if(lua_isuserdata(state, 4))
    {
        if(!luaL_checkudata (state, 4, "Vector"))
        {
            return luaL_typerror(state, 4, "Vector");
        }
        addVector = (Vector*)lua_touserdata(state, 4);
        assert(addVector);
    }
    else if(lua_isnumber(state, 4))
    {
        double number = lua_tonumber(state, 4);
        defaultAddVector.SetBroadcast(number);
        addVector = &defaultAddVector;
    }
    else
    {
        return luaL_typerror(state, 4, "Vector or number");
    }

    Vector::MultiplyAdd(*destination, *leftVector, *rightVector, *addVector);

    return 0;
}

int lua_Vector_SetBroadcast(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    if(!lua_isnumber(state, 2))
    {
        return luaL_typerror(state, 2, "number");
    }
    double number = lua_tonumber(state, 2);
    vector->SetBroadcast(number);
    return 0;
}

int lua_Vector_unary_operator(lua_State* state)
{
    Vector* vector = (Vector*)lua_touserdata(state, 1);
    if (vector == NULL)
    {
        return luaL_typerror(state, 1, "Vector");
    }
    Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);

    Vector negate;
    negate.SetBroadcast(-1);
    Vector::Multiply(*pi, *vector, negate);
    return 1;
}


int lua_infix_op(lua_State* state, void (*opFunctionPointer)(Vector& destination, const Vector& left, const Vector& right))
{
    Vector* leftVector = NULL;
    Vector* rightVector = NULL;
    int result = lua_get_infix_operation_data
    (
        state,
        leftVector,
        rightVector
    );

    if(!result)
    {
        return 0;
    }

    Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    (opFunctionPointer)(*pi, *leftVector, *rightVector);
    return 1;
}

static int lua_Vector_add_operator(lua_State* state)
{
    return lua_infix_op(state, Vector::Add);
}

static int lua_Vector_multiply_operator(lua_State* state)
{
    return lua_infix_op(state, Vector::Multiply);
}

static int lua_Vector_divide_operator(lua_State* state)
{
    return lua_infix_op(state, Vector::Divide);
}

static int lua_Vector_subtract_operator(lua_State* state)
{
    return lua_infix_op(state, Vector::Subtract);
}

static int lua_Vector_Copy(lua_State* state)
{
    Vector* toVector = LuaState::GetFuncParam<Vector>(state, 1);
    if(toVector == NULL)
    {
        return 0;
    }
    Vector* fromVector = LuaState::GetFuncParam<Vector>(state, 2);
    if(fromVector == NULL)
    {
        return 0;
    }

    toVector->SetXyzw(*fromVector);
    return 0;
}

static int lua_Vector_Length2(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, vector->Length2());
    return 1;
}

static int lua_Vector_Length3(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, vector->Length3());
    return 1;
}

static int lua_Vector_Length4(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, vector->Length4());
    return 1;
}

static int lua_Vector_Normalize2(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    vector->Normalize2();
    return 0;
}

static int lua_Vector_Normalize3(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    vector->Normalize3();
    return 0;
}

static int lua_Vector_Normalize4(lua_State* state)
{
    Vector* vector = LuaState::GetFuncParam<Vector>(state, 1);
    if(vector == NULL)
    {
        return 0;
    }
    vector->Normalize4();
    return 0;
}

static int lua_Vector_Cross(lua_State* state)
{
    Vector* destination = LuaState::GetFuncParam<Vector>(state, 1);
    if(destination == NULL)
    {
        return 0;
    }
    Vector* left = LuaState::GetFuncParam<Vector>(state, 2);
    if(left == NULL)
    {
        return 0;
    }
    Vector* right = LuaState::GetFuncParam<Vector>(state, 3);
    if(right == NULL)
    {
        return 0;
    }
    Vector::CrossProduct(*destination, *left, *right);
    return 0;
}

static int lua_Vector_Dot2(lua_State* state)
{
    Vector* left = LuaState::GetFuncParam<Vector>(state, 1);
    if(left == NULL)
    {
        return 0;
    }
    Vector* right = LuaState::GetFuncParam<Vector>(state, 2);
    if(right == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, left->Dot2(*right));
    return 1;
}

static int lua_Vector_Dot3(lua_State* state)
{
    Vector* left = LuaState::GetFuncParam<Vector>(state, 1);
    if(left == NULL)
    {
        return 0;
    }
    Vector* right = LuaState::GetFuncParam<Vector>(state, 2);
    if(right == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, left->Dot3(*right));
    return 1;
}

static int lua_Vector_Dot4(lua_State* state)
{
    Vector* left = LuaState::GetFuncParam<Vector>(state, 1);
    if(left == NULL)
    {
        return 0;
    }
    Vector* right = LuaState::GetFuncParam<Vector>(state, 2);
    if(right == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, left->Dot4(*right));
    return 1;
}


static const struct luaL_reg luaBinding [] =
{
    {"Create", Vector::lua_Vector_Create},
    {"__tostring", lua_Vector_tostring},
    {"__unm", lua_Vector_unary_operator},
    {"__add", lua_Vector_add_operator},
    {"__mul", lua_Vector_multiply_operator},
    {"__div", lua_Vector_divide_operator},
    {"__sub", lua_Vector_subtract_operator},
    {"X", lua_Vector_X},
    {"Y", lua_Vector_Y},
    {"Z", lua_Vector_Z},
    {"W", lua_Vector_W},
    {"SetX", lua_Vector_SetX},
    {"SetY", lua_Vector_SetY},
    {"SetZ", lua_Vector_SetZ},
    {"SetW", lua_Vector_SetW},
    {"SetXyzw", lua_Vector_SetXyzw},
    {"Add", lua_Vector_Add},
    {"Multiply", lua_Vector_Multiply},
    {"MultiplyAdd", lua_Vector_MultiplyAdd},
    {"Subtract", lua_Vector_Subtract},
    {"Divide", lua_Vector_Divide},
    {"SetBroadcast", lua_Vector_SetBroadcast},
    {"Copy", lua_Vector_Copy},
    {"Length2", lua_Vector_Length2},
    {"Length3", lua_Vector_Length3},
    {"Length4", lua_Vector_Length4},
    {"Normalize2", lua_Vector_Normalize2},
    {"Normalize3", lua_Vector_Normalize3},
    {"Normalize4", lua_Vector_Normalize4},
    {"Cross", lua_Vector_Cross},
    {"Dot2", lua_Vector_Dot2},
    {"Dot3", lua_Vector_Dot3},
    {"Dot4", lua_Vector_Dot4},
    {NULL, NULL}  /* sentinel */
};

void Vector::Add(Vector& destination, const Vector& left, const Vector& right)
{
    destination.x = left.x + right.x;
    destination.y = left.y + right.y;
    destination.z = left.z + right.z;
    destination.w = left.w + right.w;
}

void Vector::Subtract(Vector& destination, const Vector& left, const Vector& right)
{
    destination.x = left.x - right.x;
    destination.y = left.y - right.y;
    destination.z = left.z - right.z;
    destination.w = left.w - right.w;
}

void Vector::Multiply(Vector& destination, const Vector& left, const Vector& right)
{
    destination.x = left.x * right.x;
    destination.y = left.y * right.y;
    destination.z = left.z * right.z;
    destination.w = left.w * right.w;
}

void Vector::Multiply(Vector& destination, const Vector& left, double right)
{
    destination.x = left.x * right;
    destination.y = left.y * right;
    destination.z = left.z * right;
    destination.w = left.w * right;
}

void Vector::Divide(Vector& destination, const Vector& left, const Vector& right)
{
    destination.x = left.x / right.x;
    destination.y = left.y / right.y;
    destination.z = left.z / right.z;
    destination.w = left.w / right.w;
}

void Vector::MultiplyAdd(Vector& destination, const Vector& left, const Vector& right, const Vector& add)
{
    destination.x = (left.x * right.x) + add.x;
    destination.y = (left.y * right.y) + add.y;
    destination.z = (left.z * right.z) + add.z;
    destination.w = (left.w * right.w) + add.w;
}

void Vector::CrossProduct(Vector& destination, const Vector& left, const Vector& right)
{
    destination.x = left.y * right.z - left.z * right.y;
    destination.y = left.z * right.x - left.x * right.z;
    destination.z = left.x * right.y - left.y * right.x;
}

bool Vector::Equal(const Vector& left, const Vector& right, float precision)
{
    return std::abs(left.x - right.x) <= precision &&
           std::abs(left.y - right.y) <= precision &&
           std::abs(left.z - right.z) <= precision &&
           std::abs(left.w - right.w) <= precision;
}

void Vector::SetXyzw(const Vector& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
    w = vector.w;
}

void Vector::SetXyzw(double x, double y, double z, double w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

double Vector::Length2() const
{
    return sqrt((x * x) + (y * y));
}

double Vector::Length3() const
{
    return sqrt((x * x) + (y * y) + (z * z));
}

double Vector::Length4() const
{
    return sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

double Vector::Dot2(const Vector& v) const
{
    return x * v.x + y * v.y;
}
double Vector::Dot3(const Vector& v) const
{
    return x * v.x + y * v.y + z * v.z;
}
double Vector::Dot4(const Vector& v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

void Vector::Normalize2()
{
    double length = Length2();
    x = x / length;
    y = y / length;
}

void Vector::Normalize3()
{
    double length = Length3();
    x = x / length;
    y = y / length;
    z = z / length;
}

void Vector::Normalize4()
{
    double length = Length4();
    x = x / length;
    y = y / length;
    z = z / length;
    w = w / length;
}

bool Vector::IsNormal3() const
{
    Vector test(*this);
    test.Normalize3();
    return Vector::Equal(*this, test, 0.001);
}

void Vector::Bind(LuaState* state)
{
    state->Bind
    (
        Vector::Meta.Name(),
        luaBinding
    );
}

std::string Vector::ToString() const
{
    stringstream ss;
    ss << "Vector.Create("
       << x << ", "
       << y << ", "
       << z << ", "
       << w << ")";
    return ss.str();
}

Vector Vector::operator *(const Vector& vec) const
{
    Vector destination;
    Vector::Multiply(destination, *this, vec);
    return destination;
}

Vector Vector::operator *(double value) const
{
    Vector destination;
    Vector::Multiply(destination, *this, value);
    return destination;
}