#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <string>
#include <map>

#include "IAssetOwner.h"
#include "Texture.h"


class Asset;

class TextureManager : public IAssetOwner
{
private:
    std::map<std::string, Texture> LoadedTextures;
public:
    Texture* GetTexture(const char* name);
    bool AddTexture(const char* name, const char* path,
                    std::map<std::string, std::string> flags);
    void ClearTextures();

    // IAssetOwner stuff
    virtual bool OnAssetReload(Asset& asset);
    virtual void OnAssetDestroyed(Asset& asset);
};
#endif
