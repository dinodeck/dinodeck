#ifndef DINODECK_H
#define DINODECK_H
#include <string>

#include "AssetStore.h"
#include "IAssetOwner.h"
#include "ManifestAssetStore.h"
#include "Settings.h"
#include "Vertex.h"

// Dinodeck is the kernel of the engine and should have limited dependancies.
// It assumes access to OpenGL

class Asset;
class Game;
class TextureManager;
class IScreenChangeListener;
class DDAudio;
class FrameBuffer;

class Dinodeck : IAssetOwner
{
private:
    std::string mName; // name of the game / project
    ManifestAssetStore mManifestAssetStore;
    Settings mSettings;
    Asset* mSettingsFile;
    Game* mGame;
    TextureManager* mTextureManager;
    IScreenChangeListener* mScreenChangeListener;
    DDAudio* mDDAudio;
    FrameBuffer* mFrameBuffer;
    static const int DISPLAY_QUAD_VERTS = 6;
    Vertex mVertexBuffer[DISPLAY_QUAD_VERTS];
    static Dinodeck* Instance;

public:
    // There's only going to be one instance of dancing squid.
    // And it's often required in lua callbacks, so here it is
    // like a singleton.
    static Dinodeck* GetInstance();

    Dinodeck(const std::string& name);
    ~Dinodeck();
    const std::string& Name() const { return mName; }
    unsigned int ViewHeight() const { return mSettings.height; }
    unsigned int ViewWidth() const { return mSettings.width; }
    unsigned int DisplayWidth() const { return mSettings.displayWidth; }
    unsigned int DisplayHeight() const { return mSettings.displayHeight; }
    bool ForceReload();
    void ResetRenderWindow(unsigned int width, unsigned int height);
    void SetName(const std::string& value) { mName = value; }
    void Update(double deltaTime);
    Game* GetGame() { return mGame; }
    const Settings& GetSettings() { return mSettings; }
    DDAudio* GetAudio() { return mDDAudio; }
    bool ReadInSettingsFile(const char* name);

    // Font as specified to be default in the manifest. Can be NULL
    FTTextureFont* GetDefaultFont() { return mManifestAssetStore.GetFont("font"); }

    void SetScreenChangeListener(IScreenChangeListener* scl)
    {
        mScreenChangeListener = scl;
    };

    bool IsRunning() const;

    // Called when the context gets reset.
    void OpenGLContextReset();

    // Callback to reload the manifest
    virtual void OnAssetDestroyed(Asset& asset);
    virtual bool OnAssetReload(Asset& asset);
private:
    void SetModelViewMatrix(float width, float height);
    void CreateDisplayQuad();
};

#endif
