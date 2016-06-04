#include "ManifestAssetStore.h"

#include <assert.h>
#include <list>
#include <map>
#include <string>

#include "Asset.h"
#include "DinodeckLua.h"
#include "DDFile.h"
#include "DDLog.h"
#include "LuaState.h"


// TEMP
#ifdef ANDROID
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif

ManifestAssetStore::ManifestAssetStore()
    : mManifest(NULL), mAssetStore()
{}

ManifestAssetStore::~ManifestAssetStore()
{
    if(mManifest)
    {
        delete mManifest;
        mManifest = NULL;
    }
}

void ManifestAssetStore::Clear()
{
    if(mManifest)
    {
        delete mManifest;
        mManifest = NULL;
    }
    mAssetStore.Clear();

    for(std::map<std::string, FontAsset>::iterator iter = mFontStore.begin(); iter != mFontStore.end(); ++iter)
    {
        FontAsset& asset = iter->second;
        if(asset.mFontFile)
        {
            delete asset.mFontFile;
            asset.mFontFile = NULL;
        }

        if(asset.mFont)
        {
            delete asset.mFont;
            asset.mFont = NULL;
        }
    }
    mFontStore.clear();
}


bool ManifestAssetStore::AreAssetFlagsEqual(
    const std::map<std::string, std::string>& a,
    const std::map<std::string, std::string>& b)
{
    // Need to do a comparison.
    return true;
}

//
// From a manifest in a LuaState load asset definitions into a map
//
bool ManifestAssetStore::LoadAssetDef(lua_State* state,
                                      std::map<std::string, AssetDef>& destination,
                                      Asset::eAssetType assetType)
{
    assert(state);
    // Table coming into this function:
    // -1: asset def table
    // -2: asset def name

    // Error check - is the id a string?
    const char* assetName = luaL_checkstring(state, -2);
    if(!assetName)
    {
        return false;
    }
    std::string assetNameStr = std::string(assetName);

    //
    // ITERATE THROUGH THE ASSET DEFINITION
    //
    std::map<std::string, std::string> flags;
    lua_pushnil(state);
    // -1: nil, the first key
    // -2: asset def table
    // -3: asset def name
    while (lua_next(state, -2) != 0)
    {
        // -1: value
        // -2: key
        //dsprintf("%s = %s\n", lua_tostring(state, -2), lua_tostring(state, -1));
        flags.insert(std::pair<std::string, std::string>
        (
            lua_tostring(state, -2),
            lua_tostring(state, -1)
        ));
        lua_pop(state, 1);  // remove value, keep key for next iter
    }

    // Table after loop.
    // -1: asset def table
    // -2: asset def name

    if(flags.find( "path" ) == flags.end())
    {
        dsprintf("No path entry for asset def [%s] in manifest.\n", assetName);
        return false;
    }

    std::string path = flags["path"];

    destination.insert(std::pair<std::string, AssetDef>
    (
        assetNameStr,
        AssetDef(assetType, assetNameStr.c_str(), path.c_str(), flags)
    ));
    //printf("    AssetDef added [%s:%s]\n", name.c_str(), path.c_str());
    return true;
}


bool ManifestAssetStore::LoadLuaTableToAssetDefs(lua_State* state,
    const char* tableName,
    std::map<std::string, AssetDef>& destination)
{
    Asset::eAssetType assetType = Asset::StringToAssetType(tableName);

    if(assetType == Asset::Unknown)
    {
        dsprintf("Asset type [%s] unrecognised.\n", tableName);
        return false;
    }

    lua_pushstring(state, tableName); // -1 in stack
    lua_rawget(state, -2);
    if(lua_istable(state, -1))
    {
        // printf("Parsing %s.\n", tableName);
        // Iterate through k, v and create resource
        // Entries
        lua_pushnil(state);  // first key
        while (lua_next(state, -2) != 0)
        {
            bool success = true;
            success = LoadAssetDef(state, destination, assetType);
            if(!success)
            {
                return false;
            }
            lua_pop(state, 1);  // remove value, keep key for next iter
        }
    }
    lua_pop(state, 1);
    return true;
}

// Takes tables like this
// scripts =
// {
//     ['main.lua'] =
//     {
//          path = "/test/main.lua"
//     }
//     ... etc
// }
// And updates the in memory asset store intelligently.
bool ManifestAssetStore::LoadAssetSubTable(lua_State* state, const char* tableName, const char* path)
{
    // No longer an assert, I don't want the user to be able to crash Dancing Squid
    if(mAssetOwnerMap.find( std::string(tableName) ) == mAssetOwnerMap.end())
    {
        dsprintf("No handler for types of [%s] in manifest.\n", tableName);
        return false;
    }
    IAssetOwner* assetOwner = mAssetOwnerMap.find( std::string(tableName) )->second.owner;

    std::map<std::string, ManifestAssetStore::AssetDef> scriptAssets;
    bool isLoaded = false;
    isLoaded = LoadLuaTableToAssetDefs(state, tableName, scriptAssets);
    if(!isLoaded)
    {
        dsprintf("Failed to parse [%s].\n", path);
        dsprintf("[%s]\n", lua_tostring(state, -1));
        return false;
    }

    // Go through - has the path changed? Unload, update path
    // Are there any additional resources add them
    for(std::map<std::string, ManifestAssetStore::AssetDef>::iterator
        iter = scriptAssets.begin();
        iter != scriptAssets.end();
        ++iter)
    {
        ManifestAssetStore::AssetDef& assetDef = iter->second;
        const char* assetName = iter->first.c_str();

        Asset* asset = mAssetStore.GetAssetByName(assetName);
        if(!asset)
        {
            // If the key doesn't exist in the asset store, we should add it.
            Asset* asset = mAssetStore.Add(assetDef.name.c_str(), assetDef.type, assetDef.path.c_str(), assetDef.flags, assetOwner);
            asset->Touch(true);
            continue;
        }

        // It already exists
        // a. Check the path
        // Once [AreassetFlagsEqual] implemented can remove path check
        // As path is a flag
        if(asset->Path() != assetDef.path || !AreAssetFlagsEqual(assetDef.flags, asset->Flags()))
        {
            mAssetStore.Remove(assetName);
            Asset* newAsset = mAssetStore.Add(assetDef.name.c_str(), assetDef.type, assetDef.path.c_str(), assetDef.flags, assetOwner);
            newAsset->Touch(true);
        }
        else
        {
            // It's added, the name is the same, mark it as touched.
            asset->Touch(true);
        }
    }
    return true;
}



bool ManifestAssetStore::Reload()
{
    assert(mManifest);
    return Reload(mManifest->Path());
}

// Copy the string, because it could be pointer
// into the manifest object that's being destroyed and
// reloaded
bool ManifestAssetStore::Reload(std::string manifestPath)
{
    const char* path = manifestPath.c_str();

    if(mManifest &&                             // Do we already have a manifest?
       mManifest->Path() == manifestPath &&     // Is the new manifest, a different file?
       !AssetStore::IsOutOfDate(*mManifest))    // Has the file timestamp changed?
    {
        // Manifest file hasn't changed. Reload manifest children.
        dsprintf("[%s] SKIPPED.\n", manifestPath.c_str());
        return mAssetStore.Reload();
    }
    else
    {
        // Deleting and recreating the manifest asset keeps the Asset Class
        // simple. Even through I don't really like messing with memory outside
        // of constructors / destructors
        if(mManifest)
        {
            delete mManifest;
            mManifest = NULL;
        }

        mManifest = new Asset("manifest", Asset::Script, path, this);
        mManifest->SetTimeLastModified
        (
            AssetStore::GetModifiedTimeStamp(*mManifest)
        );

        // OK the manifest has changed, this effects ALL the assets
        // some might no longer be refernced and need removing
        // Some might be new and need adding, so first thing is to
        // read in the manifest
        LuaState luaState("Manifest");
        bool success = luaState.DoFile(path);
        if(!success)
        {
            // Should just do a clear?
            // But this way don't unload everything if there's a minor mistake
            // in the manifest file.
            delete mManifest;
            mManifest = NULL;
            return false;
        }

        // Load in the manifest lua file
        // Manifest is expected to have global table 'manifest'.
        lua_State* state = luaState.State();
        lua_getglobal(state, "manifest");
        if(!lua_istable(state, -1))
        {
            dsprintf("Error: [manifest] table missing in [%s]. Expected:\n",  path);
            dsprintf("manifest =\n");
            dsprintf("{\n");
            dsprintf("    scripts =\n");
            dsprintf("    {\n");
            dsprintf("        ...\n");
            dsprintf("    },\n");
            dsprintf("}\n");
            lua_close(state);
            return false;
        }

        {
            //
            // Modify the store of assets
            // 1. Removing ones that the manifest on longer specifies
            // 2. Add ones that are brand new
            // 3. Telling ones to be reloading if the path has changed.
            //
            mAssetStore.ResetTouchFlag();

            // Iterate through asset owners map
            for(std::map<std::string, AssetOwner>::iterator
                iter = mAssetOwnerMap.begin();
                iter != mAssetOwnerMap.end();
                ++iter)
            {
                AssetOwner& assetOwner = iter->second;

                bool success = LoadAssetSubTable(state, assetOwner.id, path);
                if(assetOwner.flags == ManifestAssetStore::Required
                   && !success)
                {
                    dsprintf("Failed to reload [%s]\n", iter->first.c_str());
                    lua_close(state);
                    return false;
                }
            }

            RemoveAssetsNotInManifest();

            return mAssetStore.Reload();
        }
        mManifest->SetIsLoaded(true);
        return true;
    }
}

// Assets the have a touch flag equaling false are removed.
void ManifestAssetStore::RemoveAssetsNotInManifest()
{
    mAssetStore.RemoveUntouchedAssets();
}

bool ManifestAssetStore::OnAssetReload(Asset& asset)
{
    if(asset.Type() == Asset::Font)
    {
        // No error checking!! Fix this.

        DDFile* fontFile = new DDFile(asset.Path().c_str());
        fontFile->LoadFileIntoBuffer();
        FTTextureFont* font = new FTTextureFont((const unsigned char*)fontFile->Buffer(), (size_t) fontFile->Size());
        font->FaceSize(72.0f);

        dsprintf("Adding font [%s]->[%s]\n",
                asset.Name().c_str(), asset.Path().c_str());
        mFontStore.insert(std::pair<std::string, FontAsset>
        (
            asset.Name(),
            FontAsset(fontFile, font)
        ));
        return true;
    }
    return false;
}

void ManifestAssetStore::OnAssetDestroyed(Asset& asset)
{
    if(asset.Type() == Asset::Font)
    {
        std::map<std::string, FontAsset>::iterator iter = mFontStore.find(std::string(asset.Name()));
        if(iter != mFontStore.end())
        {
            delete iter->second.mFont;
            iter->second.mFont = NULL;
            delete iter->second.mFontFile;
            iter->second.mFontFile = NULL;
            mFontStore.erase(iter);
        }
    }
}

// Used when loading assets from the manifest
void ManifestAssetStore::RegisterAssetOwner(const char* name, IAssetOwner* callback)
{
    RegisterAssetOwner(name, callback, ManifestAssetStore::Required);
}

void ManifestAssetStore::RegisterAssetOwner(const char* name,
                                            IAssetOwner* callback,
                                            ManifestAssetStore::eOwnerFlags flags)
{
    mAssetOwnerMap[std::string(name)] = AssetOwner
                                        (
                                            name,
                                            callback,
                                            flags
                                        );
}

FTTextureFont* ManifestAssetStore::GetFont(const char* name)
{
    std::map<std::string, FontAsset>::iterator iter = mFontStore.find(std::string(name));
    if(iter == mFontStore.end())
    {
        return NULL;
    }
    return (iter->second).mFont;
}