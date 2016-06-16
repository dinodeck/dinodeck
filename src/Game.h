#ifndef GAME_H
#define GAME_H

#include <string>

#include "IAssetOwner.h"
class Asset;
class LuaState;
struct Settings;
class ManifestAssetStore;
class GraphicsPipeline;
class TextureManager;
class FTTextureFont;

// Can these be generalised?
class Touch;
class Mouse;
class Keyboard;

//
// Responsible for calling the Lua update script.
//
class Game : public IAssetOwner
{
    // used as counter to determine if the lua state should be reloaded.
    unsigned int        mReloadCount;
    LuaState*           mLuaState;
    bool                mReady;
    Settings*           mSettings;
    ManifestAssetStore* mAssetStore;
    GraphicsPipeline*   mDebugGraphics;
    TextureManager*     mTextureManager;
    double              mDeltaTime;
    bool                mExit;
    FTTextureFont*      mSystemFont; // Better in graphics pipeline?

    Touch*              mTouch; // This feels like somekind of module system would be better.
    Mouse*              mMouse;
    Keyboard*           mKeyboard;

    void RenderError();
public:

    Game(Settings* settings,
         ManifestAssetStore* assetStore,
         TextureManager* mTextureManager);
    ~Game();

    static void Bind(LuaState* state);
    // Callbacks for Assets
    virtual bool OnAssetReload(Asset& asset);
    virtual void OnAssetDestroyed(Asset& asset);

    void ResetReloadCount() { mReloadCount = 0; }
    unsigned int GetReloadCount() { return mReloadCount; }
    int RunScriptAsset(const char* name);
    double GetDeltaTime() const { return mDeltaTime; }

    // Reloads the lua state.
    void Reset();
    // Reset the system font
    void ResetSystemFont();
    void InvalidateRendererFonts();
    void Update(double deltaTime);
    void Break() { mReady = false; }

    TextureManager* Textures() { return mTextureManager; }
    Settings* GetSettings() { return mSettings; }

    Touch* GetTouch() { return mTouch; }
    Mouse* GetMouse() { return mMouse; }
    Keyboard* GetKeyboard() { return mKeyboard; }
    LuaState* GetLuaState() { return mLuaState; }
    std::string GetLastError() const;

    FTTextureFont* GetSystemFont() { return mSystemFont; }
    FTTextureFont* GetFont(const char* name);

    // Functions to allow the game to be exited from the script
    bool IsRunning() const { return (mExit == false); }
    bool IsReady() const { return mReady; }
    void Stop() { mExit = true; }
};

#endif