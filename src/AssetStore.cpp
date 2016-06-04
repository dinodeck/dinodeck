#include "AssetStore.h"

#include <assert.h>
#include <map>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <list>

#include "Asset.h"
#include "DDLog.h"
#include "IAssetOwner.h"
#include "LuaState.h"


bool AssetStore::CleverReloading = true;

AssetStore::AssetStore()
{
}

AssetStore::~AssetStore()
{
}

Asset* AssetStore::Add
(
    const char* name,
    Asset::eAssetType assetType,
    const char* path,
    std::map<std::string, std::string>& flags,
    IAssetOwner* callback)
{
	std::pair<std::map<std::string, Asset>::iterator, bool> out =
	mStore.insert(std::pair<std::string, Asset>(
					std::string(name),
					Asset(name, assetType, path, flags, callback)));
	// I hate you stl.
	return &(out.first->second);
}

bool AssetStore::AssetExists(const char* name)
{
	return mStore.find(name) != mStore.end();
}

Asset* AssetStore::GetAssetByName(const char* name)
{
	std::map<std::string, Asset>::iterator iter = mStore.find(name);
	if(iter == mStore.end())
	{
		return NULL;
	}
	return &(iter->second);
}

void AssetStore::Clear()
{
	for(std::map<std::string, Asset>::iterator it = mStore.begin(); it != mStore.end(); ++it)
	{
		Asset& asset = it->second;
		asset.OnDestroy();
		mStore.erase( it++ );
	}
}


bool AssetStore::Reload()
{
	// Iterate through files,
	// Load if not loaded
	// Else get last modified date
	// If date is later than store date then reload
	for(std::map<std::string, Asset>::iterator it = mStore.begin(); it != mStore.end(); ++it)
	{
		Asset& asset = it->second;

        if(false == AssetStore::CleverReloading)
        {
            asset.OnReload();
            continue;
        }

		// Be careful when loading from a package.
		struct stat s;
		time_t lastModified = time(NULL);
		if(stat(asset.Path().c_str(), &s) == 0)
		{
			lastModified = s.st_mtime;
		}

		if(asset.IsLoaded() && lastModified <= asset.LastModified())
		{
			dsprintf("[%s] SKIPPED.\n", asset.Name().c_str());
		}
		else
		{
			std::string timeString(ctime(&lastModified));
			// Remove trailing \n from time string
			{
				size_t end = timeString.find_last_of('\n');
				timeString = timeString.substr(0, end);
			}

			dsprintf
			(
			 	"Loading [%s]\nLast Modified [%s]\n\n",
				asset.Name().c_str(),
				timeString.c_str()
			);

			bool success = asset.OnReload();
			if(!success)
			{
				asset.SetIsLoaded(false);
				dsprintf("Fail to load [%s]\n", asset.Name().c_str());
				return false;
			}
			asset.SetTimeLastModified(lastModified);
		}
	}
	return true;
}

time_t AssetStore::GetModifiedTimeStamp(Asset& asset)
{
	struct stat s;
	if(stat(asset.Path().c_str(), &s) == 0)
	{
		return s.st_mtime;
	}
	return -1;
}

bool AssetStore::IsOutOfDate(Asset& asset)
{
	if(!AssetStore::CleverReloading ||
       !asset.IsLoaded())
	{
		return true;
	}

	time_t lastModified = AssetStore::GetModifiedTimeStamp(asset);
	return lastModified > asset.LastModified();
}

void AssetStore::Remove(const char* name)
{
	std::map<std::string, Asset>::iterator iter = mStore.find(name);
	if(iter == mStore.end())
	{
		// Doesn't exist, don't need to do anything.
		return;
	}
	Asset& asset = iter->second;
	asset.OnDestroy();
	mStore.erase(iter);
}

void AssetStore::ResetTouchFlag()
{
    for(std::map<std::string, Asset>::iterator
    iter = mStore.begin();
    iter != mStore.end();
    ++iter)
	{
    	// Reset touch flag
    	iter->second.Touch(false);
	}
}

void AssetStore::SetAsNotLoaded(Asset::eAssetType type)
{
    for(std::map<std::string, Asset>::iterator
    iter = mStore.begin();
    iter != mStore.end();
    ++iter)
    {
        if(iter->second.Type() == type)
        {
            iter->second.SetIsLoaded(false);
        }
    }
}

void AssetStore::RemoveUntouchedAssets()
{
	std::list< std::map<std::string, Asset>::iterator > iteratorList;

    // Gather up elements that need to be deleted
    for(std::map<std::string, Asset>::iterator
        iter = mStore.begin();
        iter != mStore.end();
        ++iter)
    {
        // Are there any lua-asset that weren't touched? Remove them.
        if(!iter->second.IsTouched())
        {
        	const char* path = iter->second.Path().c_str();
            printf("Removing [%s] as it's no longer in the manifest.\n", path);
            iter->second.OnDestroy();
            iteratorList.push_back(iter);
        }
    }

    // Actually delete the elements.
    for(std::list< std::map<std::string, Asset>::iterator >::iterator
        iter = iteratorList.begin();
        iter != iteratorList.end();
        ++iter)
    {
        mStore.erase(*iter);
    }
}