#ifndef MAIN_H
#define MAIN_H

#include <string>

#include "IScreenChangeListener.h"
#include "IWebServerCallback.h"

class	Dinodeck;
struct	SDL_Surface;
union	SDL_Event;
class   WebServer;

class Main : IScreenChangeListener, IWebServerCallback
{
  private:
	SDL_Surface*   mSurface;
	bool           mRunning;
	Dinodeck*      mDinodeck;
	WebServer*     mWebServer;

    // Needs to be abstracted into some action queue.
    bool mDoWebServerReset;
    bool mDoLuaExecute;
    std::string mLuaToExecute;

	bool ResetRenderWindow();
 	void OnOpenGLContextCreated();
    void OnEvent(SDL_Event* event);
    void HandleInput();
 public:
	bool Reset();
	void Execute();
 	void OnChange(int width, int height);

    // Warning, this occurs outside the main thread!
    std::string OnWebRequest(const std::string& uri, const std::string& postdata);
	Main();
	~Main();
};

#endif