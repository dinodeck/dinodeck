#ifndef MANIFESTASSETSTORE_H
#define MANIFESTASSETSTORE_H

#include <map>
#include <string>

#include "IAssetOwner.h"
#include "AssetStore.h"


class Asset;
class DDFile;
class FTTextureFont;
struct lua_State;

//
// An asset store controlled by an manifest file.
//
class ManifestAssetStore : public IAssetOwner
{
public:
    enum eOwnerFlags
    {
        Required,
        Optional
    };
private:
    Asset* mManifest;
    AssetStore mAssetStore;

    struct AssetOwner
    {
        const char* id;
        IAssetOwner* owner;
        eOwnerFlags flags;

        AssetOwner() :
            owner(NULL), flags(Required) {}

        AssetOwner(const char* id, IAssetOwner* owner, eOwnerFlags flags) :
            id(id), owner(owner), flags(flags) {}
    };

    struct AssetDef
    {
        Asset::eAssetType type;
        std::string name;
        std::string path;
        std::map<std::string, std::string> flags;

        AssetDef(Asset::eAssetType type, const char* name, const char* path) :
            type(type), name(name), path(path) {}
        AssetDef(Asset::eAssetType type, const char* name, const char* path,
                 std::map<std::string, std::string> flags) :
            type(type), name(name), path(path), flags(flags) {}
    };

    struct FontAsset
    {
        FTTextureFont*  mFont;
        DDFile*         mFontFile; // The file data needs to be kept in memory!
        FontAsset(DDFile* fontFile, FTTextureFont* font) :
            mFont(font), mFontFile(fontFile) {}
    };
    // Who handles what by default
    // [.lua] -> Dindeck etc
    std::map<std::string, AssetOwner> mAssetOwnerMap;
    std::map<std::string, FontAsset> mFontStore;

    bool LoadLuaTableToAssetDefs(lua_State* state,
                                 const char* tableName,
                                 std::map<std::string, ManifestAssetStore::AssetDef>& destination);
    bool LoadAssetSubTable(lua_State* state, const char* tableName, const char* path);
    bool LoadAssetDef(lua_State* state,
                      std::map<std::string, ManifestAssetStore::AssetDef>& destination,
                      Asset::eAssetType assetType);
    void RemoveAssetsNotInManifest();
    static bool AreAssetFlagsEqual(const std::map<std::string, std::string>& a,
                                   const std::map<std::string, std::string>& b);
public:
    ManifestAssetStore();
    ~ManifestAssetStore();

    // Callbacks for Assets
    virtual bool OnAssetReload(Asset& asset);
    virtual void OnAssetDestroyed(Asset& asset);

    bool AssetExists(const char* name)
    {
        return mAssetStore.AssetExists(name);
    }

    Asset* GetAssetByName(const char* name)
    {
        return mAssetStore.GetAssetByName(name);
    }

    bool Reload(std::string manifest); // Copy string is on purpose
    bool Reload();
    void Clear();

    // Used when loading assets from the manifest
    void RegisterAssetOwner(const char* name, IAssetOwner* callback);
    void RegisterAssetOwner(const char* name, IAssetOwner* callback, eOwnerFlags flags);

    FTTextureFont* GetFont(const char* name);
    void    SetAsNotLoaded(Asset::eAssetType type) { mAssetStore.SetAsNotLoaded(type); }
};

#endif