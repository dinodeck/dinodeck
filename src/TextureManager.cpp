#include "TextureManager.h"

#include "Asset.h"
#include "DDLog.h"


void TextureManager::ClearTextures()
{
    // Clear out the textures from OpenGL
    LoadedTextures.clear();
}

bool TextureManager::AddTexture
(
    const char* name,
    const char* path,
    std::map<std::string, std::string> flags
)
{
    {
        Texture texture;
        LoadedTextures.insert
        (
            std::pair<std::string, Texture>
            (
                std::string(name),
                texture
            )
        );
    }
    bool pixelArt = false;

    //
    // Check for pixel art flag
    //
    std::map<std::string, std::string>::iterator
       iter = flags.find("scale");
    if(iter != flags.end())
    {
        dsprintf("Scale flag found for [%s] value [%s].\n", name, iter->second.c_str());
        pixelArt = (iter->second == std::string("pixelart"));
    }

    bool isLoaded = LoadedTextures[name].LoadDDSTexture(path, pixelArt);

    if(isLoaded == false)
    {
        LoadedTextures.erase(LoadedTextures.find(name));
        return false;
    }

    return true;
}

Texture* TextureManager::GetTexture(const char* name)
{
    if(LoadedTextures.find(name) == LoadedTextures.end())
    {
        dsprintf("ERROR: Couldn't find texture [%s]", name);
        return NULL;
    }
    return &LoadedTextures.find(name)->second;
}

// IAssetOwner stuff
bool TextureManager::OnAssetReload(Asset& asset)
{
    return AddTexture(asset.Name().c_str(), asset.Path().c_str(), asset.Flags());
}

void TextureManager::OnAssetDestroyed(Asset& asset)
{
    std::map<std::string, Texture>::iterator iter = LoadedTextures.find(asset.Name());
    if(iter == LoadedTextures.end())
    {
        if(asset.IsLoaded())
        {
            dsprintf("Asset destroyed [%s]. Asset reported to be loaded, but texture not in LoadedTextures.\n", asset.Name().c_str());
            dsprintf("Something has gone wrong.\n");
        }
    }
    else
    {
        LoadedTextures.erase(iter);
    }
}
