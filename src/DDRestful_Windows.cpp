#include "DDRestful.h"

#include <assert.h>
#include <string>

#include "DDlog.h"
#include "LuaState.h"
#include "DinodeckLua.h"

// TEMP - FOR IMMEDIATE FAILURE
#include "Dinodeck.h"
#include "Game.h"

int DDRestful::Post(const char* uri, HttpPostData* postData,
                   int successRef, int failureRef)
{
    dsprintf("Restful currently not supported on windows. %s|%d|%d \n",
             uri, successRef, failureRef);
    // if(postData != NULL)
    // {
    //     dsprintf("Post data attached.\n");
    // }

    // int number = StoreCallback(successRef, failureRef);

    // // Needs curl based implementation.
    // // Windows will need to clear callbacks on reset and cancel async things

    // // NOT IMPLEMENTED, SO FAIL IMMEDIATELY.
    // std::string msg = "";
    // DDRestful::OnFailure(Dinodeck::GetInstance()->GetGame()->GetLuaState(), number, msg);
    // DDRestful::OnFinish(Dinodeck::GetInstance()->GetGame()->GetLuaState(), number);
    return -1;
}

