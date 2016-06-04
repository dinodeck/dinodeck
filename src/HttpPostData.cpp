#include "HttpPostData.h"

#include <map>
#include <string>

#include "DinodeckLua.h"
#include "LuaState.h"
#include "reflect/Reflect.h"
#include "DDLog.h"

Reflect HttpPostData::Meta("HttpPostData", HttpPostData::Bind);

int lua_HttpPostData_Create(lua_State* state)
{
    if(!lua_isstring(state, 1))
    {
        return luaL_typerror(state, 1, "string");
    }

    std::string contentTypeStr = std::string(lua_tostring(state, 1));
    if(contentTypeStr != "application/x-www-form-urlencoded" && contentTypeStr != "multipart/form-data")
    {
        return luaL_argerror(state, 1, "contentType must be \"application/x-www-form-urlencoded\" or \"multipart/form-data\".\n");
    }

    HttpPostData::ContentType contentType = HttpPostData::Application;
    if(contentTypeStr == "multipart/form-data")
    {
        contentType = HttpPostData::Multipart;
    }

    new (lua_newuserdata(state, sizeof(HttpPostData))) HttpPostData(contentType);
    luaL_getmetatable(state, "HttpPostData");
    lua_setmetatable(state, -2);
    return 1;
}

int lua_HttpPostData_gc(lua_State* state)
{
    dsprintf("GC> post data collected.\n");
    HttpPostData* httpPostData = LuaState::GetFuncParam<HttpPostData>(state, 1);
    httpPostData->~HttpPostData();
    return 0;
}

int lua_HttpPostData_AddValue(lua_State* state)
{
    HttpPostData* httpPostData = LuaState::GetFuncParam<HttpPostData>(state, 1);
    if(NULL == httpPostData)
    {
        return 0;
    }

    // Key name
    if(!lua_isstring(state, 2))
    {
        return luaL_typerror(state, 2, "string");
    }
    const char* key = lua_tostring(state, 2);

    // Key value
    if(lua_isstring(state, 3))
    {
        bool result = httpPostData->AddValue(key, lua_tostring(state, 3));
        lua_pushboolean(state, result);
        return 1;
    }
    else
    {
        return luaL_typerror(state, 3, "string");
    }
}


static const struct luaL_reg luaBinding [] = {
  {"Create", lua_HttpPostData_Create},
  {"__gc", lua_HttpPostData_gc},
  {"AddValue", lua_HttpPostData_AddValue},
  {NULL, NULL}  /* sentinel */
};

HttpPostData::HttpPostData(ContentType contentType)
{
}

HttpPostData::HttpPostData(const HttpPostData& httpPostData)
{
    for(std::map<std::string, std::string>::const_iterator it = httpPostData.mValueMap.begin(); it != httpPostData.mValueMap.end(); ++it)
    {
        mValueMap.insert
        (
            std::pair<std::string, std::string>
            (
                it->first,
                it->second
            )
        );
    }
}

HttpPostData::HttpPostData()
{
}

HttpPostData::~HttpPostData()
{
}

bool HttpPostData::AddValue(const char* key, const char* value)
{
    mValueMap.insert
    (
        std::pair<std::string, std::string>
        (
            std::string(key),
            std::string(value)
        )
    );
    return true;
}

void HttpPostData::Bind(LuaState* state)
{
    state->Bind
    (
        HttpPostData::Meta.Name(),
        luaBinding
    );
}