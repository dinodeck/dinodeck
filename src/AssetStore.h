#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <string>
#include <map>

#include "Asset.h"

class IAssetOwner;
struct lua_State;

class AssetStore
{
private:
    static bool CleverReloading;
	std::map<std::string, Asset> mStore;

public:
    static void CleverReloadingFlag(bool value)
    {
        AssetStore::CleverReloading = value;
    }
	AssetStore();
	~AssetStore();

    static bool IsOutOfDate(Asset& asset);
    static time_t GetModifiedTimeStamp(Asset& asset);

    Asset*  Add(const char* name,
                Asset::eAssetType type,
                const char* path,
                std::map<std::string, std::string>& flags,
                IAssetOwner* callback);
    Asset*  GetAssetByName(const char* name);
    bool    AssetExists(const char* name);
    bool    Reload();
    void    Clear();
    void    Remove(const char* name);
    void    RemoveUntouchedAssets();
    void    ResetTouchFlag();
    void    SetAsNotLoaded(Asset::eAssetType type);
};

#endif