#include "Dinodeck.h"

#include <assert.h>
#include <cstdio>
#include <cmath>

#include "Asset.h"
#include "DinodeckGL.h"
#include "DDAudio.h"
#include "DDFile.h"
#include "DDLog.h"
#include "Game.h"
#include "GraphicsPipeline.h"
#include "IScreenChangeListener.h"
#include "LuaState.h"
#include "TextureManager.h"
#include "FrameBuffer.h"


class FTTextureFont;
Dinodeck* Dinodeck::Instance = NULL;

Dinodeck::Dinodeck(const std::string& name)
    :   mName(name),
        mManifestAssetStore(),
        mSettings(),
        mSettingsFile(NULL),
        mGame(NULL),
        mTextureManager(NULL),
        mScreenChangeListener(NULL),
        mDDAudio(NULL),
        mFrameBuffer(NULL)
{
    Dinodeck::Instance = this;
    mSettingsFile = new Asset("settings", Asset::Script, "settings.lua", this);
    mTextureManager = new TextureManager();
    mGame = new Game(&mSettings, &mManifestAssetStore, mTextureManager);
    mManifestAssetStore.RegisterAssetOwner("scripts", mGame);
    mDDAudio = new DDAudio();
    mFrameBuffer = new FrameBuffer();
    // You don't require fonts or textures for a game
    mManifestAssetStore.RegisterAssetOwner("textures", mTextureManager, ManifestAssetStore::Optional);
    mManifestAssetStore.RegisterAssetOwner("fonts", &mManifestAssetStore, ManifestAssetStore::Optional);
    mManifestAssetStore.RegisterAssetOwner("sounds", mDDAudio, ManifestAssetStore::Optional);
    mManifestAssetStore.RegisterAssetOwner("soundstreams", mDDAudio, ManifestAssetStore::Optional);
}

Dinodeck::~Dinodeck()
{
    if(mGame)
    {
        delete mGame;
    }

    if(mTextureManager)
    {
        delete mTextureManager;
    }

    if(mDDAudio)
    {
        delete mDDAudio;
    }

    if(mFrameBuffer)
    {
        delete mFrameBuffer;
    }
}

Dinodeck* Dinodeck::GetInstance()
{
    return Instance;
}

bool Dinodeck::ReadInSettingsFile(const char* path)
{
    if(!DDFile::FileExists(path))
    {
        dsprintf("ERROR: Settings file [%s] doesn't exist.\n", path);
        // If the settings file doesn't exist, then the assets
        // should be cleared. As they're no longer in the project.
        mManifestAssetStore.Clear();
        return false;
    }

    //
    // Create a lua state to parse the settings file
    //
    LuaState luaState("Settings");
    bool success = luaState.DoFile(path);

    if(!success)
    {
        dsprintf("ERROR: Lua failed to parse settings [%s].\n", path);
        // For now don't clear the asset store.
        return false;
    }

    // Get values, or assign defaults if not present.
    mSettings.name = luaState.GetString("name", mSettings.name.c_str());
    mSettings.width = luaState.GetInt("width", mSettings.width);
    mSettings.height = luaState.GetInt("height", mSettings.height);
    mSettings.displayWidth = luaState.GetInt("display_width", mSettings.width);
    mSettings.displayHeight = luaState.GetInt("display_height", mSettings.height);
    mSettings.mainScript = luaState.GetString("main_script", "main.lua");
    mSettings.onUpdate = luaState.GetString("on_update", "update()");
    mSettings.manifestPath = luaState.GetString("manifest", "");
    mSettings.webserver = luaState.GetBoolean("webserver", false);
    mSettings.orientation = luaState.GetString("orientation", "portrait");

    // Display Width and Height must be equal or greater
    // than width and height
    if(mSettings.width > mSettings.displayWidth)
    {
        dsprintf("Display width too small.\n\tResizing [%d]->[%d]\n",
                 mSettings.displayWidth,
                 mSettings.width);
        mSettings.displayWidth = mSettings.width;
    }

    if(mSettings.height > mSettings.displayHeight)
    {
        dsprintf("Display height too small.\n\tResizing [%d]->[%d]\n",
                 mSettings.displayHeight,
                 mSettings.height);
        mSettings.displayHeight = mSettings.height;
    }

    dsprintf("Debug: display_width [%d]\tdisplay_height [%d]\n",
             mSettings.displayWidth,
             mSettings.displayHeight);



    SetName(mSettings.name);
    return true;
}

bool Dinodeck::OnAssetReload(Asset& asset)
{
    const char* path = asset.Path().c_str();
    dsprintf("Reloading [%s]\n", path);

    if(!ReadInSettingsFile(path))
    {
        mGame->Break();
        return false;
    }

    if(!DDFile::FileExists(mSettings.manifestPath.c_str()))
    {
        dsprintf("Manifest file doesn't exist [%s]\n", mSettings.manifestPath.c_str());
        dsprintf("Manifest file is specified in settings.lua e.g. manifest=\"manifest.lua\"\n");
        mManifestAssetStore.Clear();
        mGame->Break();
        return false; // You can't do much without assets!
    }

    if(mScreenChangeListener)
    {
        mScreenChangeListener->OnChange(mSettings.width,  mSettings.height);
    }

    ResetRenderWindow(mSettings.width, mSettings.height);

    if(!mManifestAssetStore.Reload(mSettings.manifestPath))
    {
        dsprintf("Breaking as asset store failed to reload.\n");
        mGame->Break();
        return false;
    }

    return true;
}

void Dinodeck::OnAssetDestroyed(Asset& asset)
{
    // Nothing to do.
}

bool Dinodeck::ForceReload()
{
    assert(mSettingsFile);
    mGame->ResetReloadCount();

    bool resetSuccess = true;

    // First check the settings file
    // If it needs reloading it will trigger a reload
    // for the manifest
    if( AssetStore::IsOutOfDate(*mSettingsFile) )
    {
        resetSuccess = mSettingsFile->OnReload();

        if(resetSuccess)
        {
            // Update the timestamp
            time_t lastModified = AssetStore::GetModifiedTimeStamp(*mSettingsFile);
            mSettingsFile->SetTimeLastModified(lastModified);
            mFrameBuffer->Reset(ViewWidth(), ViewHeight());
        }
    }
    else
    {
        dsprintf("Reloading [%s].\n", mSettingsFile->Path().c_str());
        // But that doesn't mean the manifest or other files haven't
        resetSuccess = mManifestAssetStore.Reload();
    }

    if(!resetSuccess)
    {
        dsprintf("Reset failed.\n");
        mGame->Break();
        return false;
    }

    if(mGame->GetReloadCount() > 0 || !mGame->IsRunning())
    {
        mGame->Reset();
    }
    else if(!mGame->IsReady())
    {
        // If previously the game didn't even start running (no settings for instance)
        // Then reload it now.
        mGame->Reset();
    }

    return true;
}

//
// @deltaTime Number of seconds last frame took
//              * Capped to 1/60 on Windows
void Dinodeck::Update(double deltaTime)
{
    mFrameBuffer->Enable(); // draw scene to texture

    glClearColor(mSettings.clearRed,
                 mSettings.clearGreen,
                 mSettings.clearBlue, 0);
    SetModelViewMatrix(ViewWidth(), ViewHeight());

    mGame->Update(deltaTime);
    mFrameBuffer->Disable(); // back to drawing to main window

    glClearColor(0,  0,  0, 0);
    SetModelViewMatrix(DisplayWidth(), DisplayHeight());

    {
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mFrameBuffer->TextureId());

        // This doesn't need doing each frame...
        CreateDisplayQuad();

        const unsigned int POSITION_SIZE = 3; // no w
        const unsigned int COLOUR_SIZE = 4;
        const unsigned int TEXCOORD_SIZE = 2;

        glVertexPointer(POSITION_SIZE, GL_FLOAT, sizeof(Vertex), mVertexBuffer);
        glEnableClientState(GL_VERTEX_ARRAY);

        glColorPointer(COLOUR_SIZE, GL_FLOAT, sizeof(Vertex), &mVertexBuffer[0].r);
        glEnableClientState(GL_COLOR_ARRAY);

        glTexCoordPointer(TEXCOORD_SIZE, GL_FLOAT, sizeof(Vertex), &mVertexBuffer[0].u);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);


        glPushMatrix();
        {
            glDrawArrays(GL_TRIANGLES, 0, DISPLAY_QUAD_VERTS);
        }
        glPopMatrix();

        //
        // Disable the various pointers
        //
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glDisable(GL_TEXTURE_2D);
    }

}

bool Dinodeck::IsRunning() const
{
    return mGame->IsRunning();
}

void Dinodeck::SetModelViewMatrix(float width, float height)
{
    glViewport(0, 0, width, height);

     // Setups an orthographic view, should be handled by renderer.
    float hWidth = width / 2.0f;
    float hHeight = height / 2.0f;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrthof(-hWidth, hWidth, -hHeight, hHeight, 0.0, 0.1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Dinodeck::ResetRenderWindow(unsigned int width, unsigned int height)
{
    dsprintf("Resetting render window %d %d\n", width, height );
    mSettings.width = width;
    mSettings.height = height;

    mFrameBuffer->Reset(ViewWidth(), ViewHeight());
    // A nice slate greyish clear colour
    glClearColor(mSettings.clearRed,
             mSettings.clearGreen,
             mSettings.clearBlue, 0);
    SetModelViewMatrix(ViewWidth(), ViewHeight());

    // Enabled blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void Dinodeck::OpenGLContextReset()
{
    mManifestAssetStore.SetAsNotLoaded(Asset::Texture);
    mManifestAssetStore.SetAsNotLoaded(Asset::Font); // Font also uses textures.
    // Reset the system font too.
    mGame->ResetSystemFont();
    mGame->InvalidateRendererFonts();
    mFrameBuffer->Reset(ViewWidth(),
                        ViewHeight());
}

void Dinodeck::CreateDisplayQuad()
{
    // Fill it up

    Vector colour;
    colour.SetBroadcast(1.0f);
    const float halfWidth = floor(((float)DisplayWidth())/2.0f);
    const float halfHeight = floor(((float)DisplayHeight())/2.0f);

    // TL
    mVertexBuffer[0] = Vertex(
        Vector(-halfWidth, 0 + halfHeight, 0.f, 1),
        colour,
        0, 1);

    // TR
    mVertexBuffer[1] = Vertex(
        Vector(halfWidth, 0 + halfHeight, 0, 1),
        colour,
        1, 1);

    // BL
    mVertexBuffer[2] = Vertex(
        Vector(-halfWidth, 0 - halfHeight, 0, 1),
        colour,
        0, 0);

    // TR
    mVertexBuffer[3] = Vertex(
        Vector(halfWidth, 0 + halfHeight, 0, 1),
        colour,
        1, 1);

    // BR
    mVertexBuffer[4] = Vertex(
        Vector(halfWidth, 0 - halfHeight, 0, 1),
        colour,
        1, 0);

    // BL
    mVertexBuffer[5] = Vertex(
        Vector(-halfWidth, 0 - halfHeight, 0, 1),
        colour,
        0, 0);
}