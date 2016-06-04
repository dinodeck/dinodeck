#include "Sprite.h"

#include <string>
#include <assert.h>

#include "DinodeckGL.h"
#include "DinodeckLua.h"
#include "LuaState.h"
#include "Texture.h"

Reflect Sprite::Meta("Sprite", Sprite::Bind);

int lua_Sprite_Create(lua_State * state)
{
    Sprite * ps = new (lua_newuserdata(state, sizeof(Sprite))) Sprite();
    ps->Init();
    luaL_getmetatable(state, "Sprite");
    lua_setmetatable(state, -2);
    return 1;
}

int lua_Sprite_tostring(lua_State* state)
{
    lua_pushliteral(state, "Sprite");
    return 1;
}

int lua_Sprite_GetPosition(lua_State* state)
{

    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }

    // This is optional
    Vector* vector = NULL;
    if(lua_isuserdata(state, 2))
    {
        if(luaL_checkudata (state, 2, "Vector"))
        {
            vector = (Vector*)lua_touserdata(state, 2);
        }
        else
        {
            luaL_typerror(state, 2, "Vector");
        }
    }
    if (vector == NULL)
    {
        vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();;
        assert(vector != NULL);
        luaL_getmetatable(state, "Vector");
        lua_setmetatable(state, -2);
        vector->SetXyzw(sprite->position);
        return 1;
    }
    vector->SetXyzw(sprite->position);
    return 0;
}

int lua_Sprite_SetPosition(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }
    if(lua_isnumber(state, 2))
    {
        double x = (double) luaL_optnumber(state, 2, sprite->GetPosition().x);
        double y = (double) luaL_optnumber(state, 3, sprite->GetPosition().y);
        sprite->SetPosition(x, y);
    }
    else
    {
        Vector* vector = (Vector*)lua_touserdata(state, 2);
        if (vector == NULL)
        {
            return luaL_typerror(state, 2, "Vector");
        }
        sprite->SetPosition(*vector);
    }
    return 0;
}

int lua_Sprite_SetTexture(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL && !luaL_checkudata (state, 1, "Sprite"))
    {
        return luaL_typerror(state, 1, "Sprite");
    }
    Texture** texture = (Texture**)lua_touserdata(state, 2);
    if (texture == NULL && !luaL_checkudata (state, 2, "Texture"))
    {
        return luaL_typerror(state, 2, "Texture");
    }
    sprite->SetTexture(*texture);
    return 0;
}

int lua_Sprite_GetTexture(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }
    Texture* texture = sprite->texture;
    if(texture == NULL)
    {
        lua_pushnil(state);
        return 1;
    }
    else
    {
        // Mainly repeated code, should be a call to texture
        Texture **pi = (Texture **)lua_newuserdata(state, sizeof(Texture*));
        (*pi) = texture;
        luaL_getmetatable(state, "Texture");
        lua_setmetatable(state, -2);
        return 1;
    }
}

int lua_Sprite_SetScale(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }

    if(lua_isuserdata(state, 2) && luaL_checkudata (state, 2, "Vector"))
    {
        Vector* vector = (Vector*)lua_touserdata(state, 2);
        assert(vector);
        sprite->scale.SetXyzw(*vector);
    }
    else if(lua_isnumber(state, 2))
    {
        sprite->scale.x = lua_tonumber(state, 2);

        // Will be 0 if no number passed in!
        sprite->scale.y = lua_tonumber(state, 3);
    }
    else
    {
        return luaL_typerror(state, 1, "Vector or number");
    }

    // Scale can either be two numbers of a vector
    return 0;
}

int lua_Sprite_GetScale(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }

    if(lua_isuserdata(state, 2))
    {
        if(!luaL_checkudata (state, 2, "Vector"))
        {
            return luaL_typerror(state, 2, "Vector");
        }
        Vector* vector = (Vector*)lua_touserdata(state, 2);
        assert(vector);
        vector->SetXyzw(sprite->scale);
        return 0;
    }
    else
    {
        // No vector passed in so one needs creating
        Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        luaL_getmetatable(state, "Vector");
        lua_setmetatable(state, -2);
        pi->SetXyzw(sprite->scale);
        return 1;
    }
}

int lua_Sprite_SetColour(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }

    if(!lua_isuserdata(state, 2) || !luaL_checkudata (state, 2, "Vector"))
    {
        return luaL_typerror(state, 1, "Vector");
    }
    Vector* vector = (Vector*)lua_touserdata(state, 2);
    assert(vector);
    sprite->colour.SetXyzw(*vector);
    return 0;
}

int lua_Sprite_GetColour(lua_State* state)
{
    Sprite* sprite = (Sprite*)lua_touserdata(state, 1);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 1, "Sprite");
    }

    if(lua_isuserdata(state, 2))
    {
        if(!luaL_checkudata (state, 2, "Vector"))
        {
            return luaL_typerror(state, 2, "Vector");
        }
        Vector* vector = (Vector*)lua_touserdata(state, 2);
        assert(vector);
        vector->SetXyzw(sprite->colour);
        return 0;
    }
    else
    {
        Vector * pi = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        luaL_getmetatable(state, "Vector");
        lua_setmetatable(state, -2);
        pi->SetXyzw(sprite->colour);
        return 1;
    }
}

int lua_Sprite_SetUvs(lua_State* state)
{
    Sprite* sprite = LuaState::GetFuncParam<Sprite>(state, 1);
    if(sprite == NULL)
    {
        return 0;
    }

    // Sprite set uvs is overloaded in far too many ways
    if(lua_isnumber(state, 2))
    {
        double topLeftU = luaL_checknumber(state, 2);
        double topLeftV = luaL_checknumber(state, 3);
        double bottomRightU = luaL_checknumber(state, 4);
        double bottomRightV = luaL_checknumber(state, 5);
        sprite->SetUVs(topLeftU, topLeftV, bottomRightU, bottomRightV);

    }
    else if(lua_isuserdata(state, 2))
    {
        printf("Sorry vector overloads not supported yet.\n");
        return 0;
    }

    return 0;
}

static int lua_Sprite_SetRotation(lua_State* state)
{
    Sprite* sprite = LuaState::GetFuncParam<Sprite>(state, 1);
    if(sprite == NULL)
    {
        return 0;
    }
    if(!lua_isnumber(state, 2))
    {

        return luaL_typerror(state, 2, "number");
    }

    double rotation = lua_tonumber(state, 2);
    sprite->SetRotation(rotation);
    return 0;
}

static int lua_Sprite_GetRotation(lua_State* state)
{
    Sprite* sprite = LuaState::GetFuncParam<Sprite>(state, 1);
    if(sprite == NULL)
    {
        return 0;
    }
    lua_pushnumber(state, sprite->GetRotation());
    return 1;
}

static const struct luaL_reg luaBinding [] = {
  {"Create", lua_Sprite_Create},
  {"__tostring", lua_Sprite_tostring},
  {"GetPosition", lua_Sprite_GetPosition},
  {"SetPosition", lua_Sprite_SetPosition},
  {"SetTexture", lua_Sprite_SetTexture},
  {"GetTexture", lua_Sprite_GetTexture},
  {"SetScale", lua_Sprite_SetScale},
  {"GetScale", lua_Sprite_GetScale},
  {"SetColor", lua_Sprite_SetColour},
  {"GetColor", lua_Sprite_GetColour},
  {"SetUVs", lua_Sprite_SetUvs},
  {"SetRotation", lua_Sprite_SetRotation},
  {"GetRotation", lua_Sprite_GetRotation},
  {NULL, NULL}  /* sentinel */
};

void Sprite::Bind(LuaState* state)
{
    state->Bind
    (
        Sprite::Meta.Name(),
        luaBinding
    );
}

void Sprite::Init()
{
    texture = NULL;
    scale.x = 1;
    scale.y = 1;
    colour.SetXyzw(1, 1, 1, 1);
    topLeftU = 0;
    topLeftV = 0;
    bottomRightU = 1;
    bottomRightV = 1;
    rotation = 0;
}

void Sprite::Init(const Sprite& sprite)
{
    texture = sprite.texture;
    position.SetXyzw(sprite.position);
    colour.SetXyzw(sprite.colour);
    scale.SetXyzw(sprite.scale);
    topLeftU = sprite.topLeftU;
    topLeftV = sprite.topLeftV;
    bottomRightU = sprite.bottomRightU;
    bottomRightV = sprite.bottomRightV;
    rotation = sprite.rotation;
}

void Sprite::SetPosition(const Vector& value)
{
    position.x = value.x;
    position.y = value.y;
    position.z = value.z;
    position.w = value.w;
}

void Sprite::SetPosition(double x, double y)
{
    position.x = x;
    position.y = y;
}

void Sprite::SetTexture(Texture* texture)
{
    this->texture = texture;
}

void Sprite::SetUVs(double topLeftU, double topLeftV, double bottomRightU, double bottomRightV)
{
    this->topLeftU = topLeftU;
    this->topLeftV = topLeftV;
    this->bottomRightU = bottomRightU;
    this->bottomRightV = bottomRightV;
}
