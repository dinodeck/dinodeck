#include "Asset.h"
#include <assert.h>
#include <stdio.h>

#include "DinodeckLua.h"
#include "Game.h"
#include "LuaState.h"
#include "reflect/Reflect.h"

Reflect Asset::Meta("Asset", Asset::Bind);


const char* Asset::TypeToStr[Asset::Count] =
{
    "",
    "scripts",
    "fonts",
    "textures",
    "sounds",
    "soundstreams"
};

bool Asset::OnReload()
{
    assert(mLoader);
    mIsLoaded = mLoader->OnAssetReload(*this);
    return mIsLoaded;
}

//
// Gives a chance for the owner to clean up the Asset
//
void Asset::OnDestroy()
{
    assert(mLoader);
    mLoader->OnAssetDestroyed(*this);
}

Asset::Asset(const char* name, Asset::eAssetType assetType, const char* path, IAssetOwner* owner)
{
    assert(owner);
    mName = name;
    mPath = path;
    mLoader = owner;
    mIsLoaded = false;
    mAssetType = assetType;
}

Asset::Asset(const char* name,
            eAssetType assetType,
            const char* path,
            std::map<std::string, std::string> flags,
            IAssetOwner* owner)
{
    assert(owner);
    mName = name;
    mPath = path;
    mLoader = owner;
    mFlags = flags;
    mIsLoaded = false;
    mAssetType = assetType;
}


void Asset::SetTimeLastModified(time_t lastModified)
{
    mLastModified = lastModified;
}

Asset::eAssetType Asset::StringToAssetType(const char* assetType)
{
    std::string id = assetType;

    if(id == "scripts")
    {
        return Asset::Script;
    }
    else if(id == "fonts")
    {
        return Asset::Font;
    }
    else if(id == "textures")
    {
        return Asset::Texture;
    }
    else if(id == "sounds")
    {
        return Asset::Sound;
    }
    else if(id == "soundstreams")
    {
        return Asset::Stream;
    }
    return Asset::Unknown;
}


int Asset::Run(lua_State* state, const char* name)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);
    return game->RunScriptAsset(name);
}

static int lua_Run(lua_State* state)
{
    // Expecting one args that's a string
    if(1 != lua_gettop(state)
       || !lua_isstring(state, -1))
    {
        return luaL_typerror(state, 1, "string");
    }
    const char* firstArg = lua_tostring(state,  -1);
    return Asset::Run(state, firstArg);
}

static const struct luaL_reg luaBinding [] = {
  {"Run", lua_Run},
  // {"Request", lua_Request},
  // {"__gc", lua_gc},
  // {"IsLoaded", lua_IsLoaded},
  // {"IsLoading", lua_IsLoading},
  {NULL, NULL}  /* sentinel */
};

void Asset::Bind(LuaState* state)
{
    state->Bind
    (
        Asset::Meta.Name(),
        luaBinding
    );
}