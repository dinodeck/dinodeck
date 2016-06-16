#include "Game.h"

#include <ctime>
#include <assert.h>

#include "../bin/default_font.h"
#include "Asset.h"
#include "Dinodeck.h"
#include "DinodeckLua.h"
#include "DDAudio.h"
#include "DDLog.h"
#include "GraphicsPipeline.h"
#include "IAssetOwner.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
#include "input/Touch.h"
#include "LuaState.h"
#include "ManifestAssetStore.h"
#include "Renderer.h"
#include "Settings.h"
//#include "System.h"
#include "TextureManager.h"
#include "Vector.h"


// Would rather unify this
#ifdef ANDROID
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif



Game::Game(Settings* settings,
           ManifestAssetStore* assetStore,
           TextureManager* textureManager) :
    mReloadCount(0),
    mLuaState(NULL),
    mReady(false),
    mSettings(settings),
    mAssetStore(assetStore),
    mDebugGraphics(NULL),
    mTextureManager(textureManager),
    mDeltaTime(0),
    mExit(false),
    mSystemFont(NULL),
    mTouch(NULL),
    mMouse(NULL),
    mKeyboard(NULL)
{
    mLuaState = new LuaState("Game");
    mLuaState->InjectIntoRegistry("Game", (void*) this);
    Game::Bind(mLuaState);

    mTouch = new Touch();
    mMouse = new Mouse();
    mKeyboard = new Keyboard();

    // The system font is created on by being reset when OpenGL context is made.
}

Game::~Game()
{
    if(mLuaState)
    {
        delete mLuaState;
        mLuaState = NULL;
    }

    if(mDebugGraphics)
    {
        delete mDebugGraphics;
        mDebugGraphics = NULL;
    }

    if(mTouch)
    {
        delete mTouch;
        mTouch = NULL;
    }

    if(mMouse)
    {
        delete mMouse;
        mMouse = NULL;
    }

    if(mKeyboard)
    {
        delete mKeyboard;
        mKeyboard = NULL;
    }

    if(mSystemFont)
    {
        delete mSystemFont;
        mSystemFont = NULL;
    }
}

bool Game::OnAssetReload(Asset& asset)
{
    mReloadCount++;
    return true;
}

int Game::RunScriptAsset(const char* name)
{
    Asset* scriptAsset = mAssetStore->GetAssetByName(name);
    lua_State* state = mLuaState->State();

    if(scriptAsset == NULL)
    {
        lua_Debug ar;
        lua_getstack(state, 1, &ar);
        lua_getinfo(state, "nSl", &ar);
        lua_pushfstring
        (
            state,
            "%s:%d: Asset.Run asset [%s] does not exist.\n",
            ar.short_src,
            ar.currentline,
            name
        );
        Break();
        return lua_error(state);
    }

    // This will call lua_error if things go wrong.
    bool success = mLuaState->DoFile(scriptAsset->Path().c_str());

    if(false == success)
    {
        // Need to halt the game
        Break();
        return 0;
    }

    return 0;
}

void Game::OnAssetDestroyed(Asset& asset)
{
    // If you remove a script, that means we'll need to reload.
    mReloadCount++;
}


void Game::Reset()
{
    dsprintf("Reloading project.\n\n");
    mReady = true;
    if(mDebugGraphics)
    {
        delete mDebugGraphics;
        mDebugGraphics = NULL;
    }
    mLuaState->Reset();
    Game::Bind(mLuaState);
    //mGraphicsPipeline->Reset();
    Dinodeck::GetInstance()->GetAudio()->Reset();

    // Main Script Id, needs to be taken from the asset store.
    // Settings->mainScript
    const char* mainScriptName = mSettings->mainScript.c_str();
    if(!mAssetStore->AssetExists(mainScriptName))
    {
        dsprintf("Error: Main script [%s], defined in settings.lua, does not exist in the asset store.\n"
                 "- Has it been defined in the manifest file [%s]?\n",
               mainScriptName, mSettings->manifestPath.c_str());
        Break();
        return;
    }

    Asset* mainScript = mAssetStore->GetAssetByName(mainScriptName);
    printf("Calling main_script %s at %s\n", mainScriptName, mainScript->Path().c_str());

    bool mainFileParsed = mLuaState->DoFile(mainScript->Path().c_str());

    if(false == mainFileParsed)
    {
        dsprintf("Failed parsing main_script.\n");
        Break();
        return;
    }

    if(mReady)
    {
        dsprintf("Reload success:\n");
    }
    else
    {
        dsprintf("Reload failed:");
        dsprintf("\tPress F2 to reload.\n");
        Break();
    }
}

void Game::RenderError()
{
    // Only take hit of an extra pipeline if necessary.
    if(NULL == mDebugGraphics)
    {
        mDebugGraphics = new GraphicsPipeline();
    }
    mDebugGraphics->OnNewFrame();
    //
    // Prints error and reset command to renderer.
    //
    mDebugGraphics->SetFont(mSystemFont);
    Vector lightGrey = Vector(0.839, 0.839, 0.839, 1);
    mDebugGraphics->SetFont(mSystemFont);
    mDebugGraphics->SetTextAlignX(AlignX::Left);
    mDebugGraphics->SetTextAlignY(AlignY::Top);
    mDebugGraphics->SetFontScale(0.3, 0.3);
    mDebugGraphics->PushText
    (
        -mSettings->width/2 + 5,    // +/- 5 is padding
        mSettings->height/2 - 5,
        "Press F2 to reload.",
        lightGrey,
        mSettings->width
    );

    mDebugGraphics->PushText
    (
        -mSettings->width/2 + 5,    // +/- 5 is padding
        mSettings->height/2 - 40,
        GetLastError().c_str(),
        lightGrey,
        mSettings->width
    );
}

void Game::Update(double deltaTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mDeltaTime = deltaTime;

    if(!mReady)
    {
        RenderError();
        return;
    }

    for(std::vector<Renderer*>::iterator it = Renderer::mRenderers.begin();
        it != Renderer::mRenderers.end(); ++it)
    {
        (*it)->Graphics()->OnNewFrame();
    }

    bool result = mLuaState->DoString("update()");

    // This should be in the render function?
    for(std::vector<Renderer*>::iterator it = Renderer::mRenderers.begin();
        it != Renderer::mRenderers.end(); ++it)
    {
        (*it)->Graphics()->Flush();
    }

    if(!result)
    {
        dsprintf("Press F2 to reload.\n");
        Break();
    }

    // Force a full collect each frame
    mLuaState->CollectGarbage();

    //
    // Update Input
    //
    mTouch->Update();
    mMouse->Update();
    mKeyboard->Update();
}

std::string Game::GetLastError() const
{
    return mLuaState->GetLastError();
}

FTTextureFont* Game::GetFont(const char* name)
{
    return mAssetStore->GetFont(name);
}

void Game::ResetSystemFont()
{
    if(NULL != mSystemFont)
    {
        delete mSystemFont;
        mSystemFont = NULL;
    }
    mSystemFont = new FTTextureFont
    (
        default_font,
        default_font_len
    );
    mSystemFont->FaceSize(72.0f);
}

void Game::InvalidateRendererFonts()
{
    for(std::vector<Renderer*>::iterator it = Renderer::mRenderers.begin();
        it != Renderer::mRenderers.end(); ++it)
    {
        (*it)->Graphics()->ClearCachedFont();
    }
}

int lua_load_library(lua_State* state)
{
    if(1 != lua_gettop(state) || !lua_isstring(state,  -1))
    {
        lua_pushstring(state, "Load Library: library name expected.\n");
        return lua_error(state);
    }

    // Get the library name passed in.
    std::string strLibName = std::string(lua_tostring(state,  -1));
    //dsprintf("Load Library:[%s]\n", strLibName.c_str());
    lua_pop(state, 1); // clear the string.

    LuaState* luaState = LuaState::GetWrapper(state);
    bool bindSuccess = Reflect::Bind(strLibName, luaState);

    if(!bindSuccess)
    {
        return luaL_error(state, "Failed to bind library [%s]", strLibName.c_str());
    }
    return 0;
}

static int lua_get_delta_time(lua_State* state)
{
    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    lua_pushnumber(state, game->GetDeltaTime());
    return 1;
}

static int lua_get_time(lua_State* state)
{
    lua_pushnumber(state, time(0));
    return 1;
}

void Game::Bind(LuaState* state)
{
    // Bind the general global functions.
    lua_State* s = state->State();
    lua_register(s, "LoadLibrary", lua_load_library);
    lua_register(s, "GetDeltaTime", lua_get_delta_time);
    lua_register(s, "GetTime", lua_get_time); // seconds since epoch
}