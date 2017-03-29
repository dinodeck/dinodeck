#include "Main.h"
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "Dinodeck.h"
#include "DinodeckGL.h"
#include "DDLog.h"
#include "Game.h"
#include "input/Keyboard.h"
#include "input/Mouse.h"
#include "physfs.h"
#include "SDL/SDL.h"
#include "Settings.h"
#include "Webserver.h"
#include "LuaState.h"

Main::Main() :
  mSurface(0),
  mRunning(true),
  mDinodeck(NULL),
  mWebServer(NULL),
  mDoWebServerReset(false),
  mDoLuaExecute(false),
  mLuaToExecute()
{
    mDinodeck = new Dinodeck("Dinodeck");
    mDinodeck->SetScreenChangeListener(this);
}

Main::~Main()
{
    delete mDinodeck;

    if(mWebServer)
    {
        delete mWebServer;
    }
}

void Main::OnEvent(SDL_Event* event)
{
    switch(event->type)
    {
        case SDL_QUIT:
        {
            mRunning = false;
        } break;

        case SDL_KEYDOWN:
        {
            if(event->key.keysym.sym == SDLK_F2)
            {
                Reset();
            }

            Game* game = mDinodeck->GetGame();
            assert(game);
            Keyboard* keyboard = game->GetKeyboard();
            assert(keyboard);
            keyboard->OnKeyDownEvent(event->key.keysym.sym);
        } break;

        case SDL_KEYUP:
        {
            Game* game = mDinodeck->GetGame();
            assert(game);
            Keyboard* keyboard = game->GetKeyboard();
            assert(keyboard);
            keyboard->OnKeyUpEvent(event->key.keysym.sym);
        } break;

    }
}

bool Main::ResetRenderWindow()
{
    const char* name = mDinodeck->Name().c_str();
    SDL_WM_SetCaption(name, name);

    // SDL handles this surface memory, so it can be called multiple times without issue.
    mSurface = SDL_SetVideoMode(
        mDinodeck->DisplayWidth(),
        mDinodeck->DisplayHeight(),
        32,
        SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);

    if(mSurface == NULL)
    {
        printf("Error initializing graphics: %s\n", SDL_GetError());
        return false;
    }
    // OpenGL context has been reset, therefore all texture data has been lost.
    // Textures may need reloading, mark them as not loaded.
    mDinodeck->OpenGLContextReset();

    SDL_WarpMouse(mDinodeck->DisplayWidth()/2, mDinodeck->DisplayHeight()/2);
    return true;
}

void Main::OnOpenGLContextCreated()
{
    if(!Reset())
    {
        // Settings file reading has failed, so create window with defaults.
        // It would still be nice to display error messages.
        ResetRenderWindow();
    }

    // Check for webserver activate
    if(!mWebServer)
    {
        bool useWebserver = mDinodeck->GetSettings().webserver;
        if(useWebserver)
        {
            mWebServer = new WebServer(8080, this);
        }
    }
}

void Main::HandleInput()
{
    int mouseX;
    int mouseY;
    int mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    mouseX -= mDinodeck->DisplayWidth() / 2;
    mouseY -= mDinodeck->DisplayHeight() / 2;

    bool leftDown   = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
    bool middleDown = (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE));
    bool rightDown  = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));

    Game* game = mDinodeck->GetGame();
    assert(game);
    Mouse* mouse = game->GetMouse();
    assert(mouse);
    mouse->OnMouseEvent(mouseX, mouseY, leftDown, middleDown, rightDown);
}

bool Main::Reset()
{
    return mDinodeck->ForceReload();
}

void Main::Execute()
{
    unsigned int initParams = SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK;
    if(SDL_Init(initParams) < 0)
    {
    	printf("SDL Failed to init");
        return;
    }

    // Allow the game pads to be polled.
    SDL_JoystickEventState(SDL_IGNORE);
    SDL_EnableUNICODE(1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,      8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,     32);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,  	8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 	8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

    OnOpenGLContextCreated();

    unsigned int thisTime = 0;
    unsigned int lastTime = 0;
    unsigned int framesPerSecond = 60;
    unsigned int millisecondsPerFrame = 1000 / framesPerSecond;
    unsigned int fpsTicks = 0;

    SDL_Event event;

    while(mRunning)
    {
        // Calculate delta time
        fpsTicks = SDL_GetTicks();
        thisTime = SDL_GetTicks(); // returns in milliseconds
        double deltaTime = static_cast<double>((thisTime - lastTime)) / 1000.f; // convert to seconds
        lastTime = thisTime;

        while(SDL_PollEvent(&event))
        {
            OnEvent(&event);
        }


        // This could be an actoin queue of types.
        if(mDoWebServerReset)
        {
            mDoWebServerReset = false;
            Reset();
        }

        if(mDoLuaExecute)
        {
            // oh no law of demeter :(
            mDinodeck->GetGame()->GetLuaState()->DoString(mLuaToExecute.c_str());
            mDoLuaExecute = false;
            mLuaToExecute = "";
        }

        HandleInput();
        mDinodeck->Update(deltaTime);

        fpsTicks = SDL_GetTicks() - fpsTicks;
        if (fpsTicks < millisecondsPerFrame)
        {
            SDL_Delay(millisecondsPerFrame - fpsTicks);
        }
        SDL_GL_SwapBuffers();

        if(mRunning)
        {
            mRunning = mDinodeck->IsRunning();
        }
    }

    SDL_Quit();

	return;
}

void Main::OnChange(int width, int height)
{
    dsprintf("Screen res changed occured.\n");
    ResetRenderWindow();
}

std::string Main::OnWebRequest(const std::string& uri, const std::string& postdata)
{
    // THIS IS BROKEN!
    // This all on a seperate thread and therefore dangerous
    // Everything should be locked correctly - which it currently isn't!
    // (Needs a nice cross platform threading library.)

    if(uri == "/reset/")
    {
        mDoWebServerReset = true;
    }
    else if(uri == "/lasterror/")
    {
       return mDinodeck->GetGame()->GetLastError();
    }
    else if(uri == "/execute/")
    {
        // probably need to queue up and execute later
        mDoLuaExecute = true;
        mLuaToExecute = postdata;
    }

    return std::string("");
}

int main(int argc, char *argv[])
{
    PHYSFS_init(argv[0]);
    PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
    PHYSFS_addToSearchPath("data.7z", 1);

    // 'mainInstance', to avoid a #define clash from SDL
    // under mac for 'main'
	Main mainInstance;
	mainInstance.Execute();
	return 0;
}