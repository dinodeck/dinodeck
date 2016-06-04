#ifndef ASSET_H
#define ASSET_H

#include <ctime>
#include <string>
#include <map>

#include "IAssetOwner.h"
#include "reflect/Reflect.h"

class LuaState;
struct lua_State;

//
// An asset is something the game uses such as a script file, texture or model
//
class Asset
{
public: static Reflect Meta;
public:
	enum eAssetType
    {
        Unknown,
        Script,
        Font,
        Texture,
        Sound,
        Stream,
        Count
    };
    static const char* TypeToStr[Asset::Count];
private:
	std::string mPath;
	std::string mName;
    std::map<std::string, std::string> mFlags;
	bool mIsLoaded;
	IAssetOwner* mLoader;
	time_t mLastModified;
	eAssetType mAssetType;

	bool mTouch; // used when checking what needs to be reloaded
public:
	static void Bind(LuaState* state);
	static int Run(lua_State* state, const char* name);
	static Asset::eAssetType StringToAssetType(const char* assetType);

	Asset(const char* name, eAssetType assetType, const char* path, IAssetOwner* owner);
    Asset(const char* name,
          eAssetType assetType,
          const char* path,
          std::map<std::string, std::string> flags,
          IAssetOwner* owner);

	// Functions that trigger callbacks
	bool 				OnReload();
	void 				OnDestroy();

	// General functions
	const std::string& 	Path() const { return mPath; }
	const std::string& 	Name() const { return mName; }
    const std::map<std::string, std::string>&  Flags() const { return mFlags; }
	bool 				IsLoaded() const { return mIsLoaded; }
	void				SetIsLoaded(bool value) { mIsLoaded = value; }
	void 				SetTimeLastModified(time_t lastModified);
	time_t 				LastModified() const { return mLastModified; }
	void				Touch(bool value) { mTouch = value; }
	bool				IsTouched() const { return mTouch; }
	eAssetType			Type() const { return mAssetType; }
};


#endif
