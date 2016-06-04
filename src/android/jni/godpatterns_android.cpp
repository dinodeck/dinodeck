#include "godpatterns_android.h"

#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <string>


#include "../../Dinodeck.h"
#include "../../DDFile.h"
#include "../../DDLog.h"
#include "../../DDRestful.h"
#include "../../Game.h"
#include "../../input/Touch.h"
#include "../../Settings.h"
#include "AndroidWrapper.h"
#include "DDLuaCallbacks.h"

#define DS_CALLBACK_SUCCESS 1
#define DS_CALLBACK_FAILURE 2
#define DS_CALLBACK_FINISH 3

struct CallbackMsg
{
    int msgType;
    int callbackId;
    std::string response;
    CallbackMsg(int msgType, int callbackId)
        : msgType(msgType), callbackId(callbackId) {}
    CallbackMsg(int msgType, int callbackId, const char* response)
        : msgType(msgType), callbackId(callbackId), response(response) {}
};

static Dinodeck* gDinodeck = NULL;
static JavaVM* gJavaVM;
static std::vector<CallbackMsg> gCallbackMessages;

/* External Java functions are implemented here.
*/

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved)
{
    dsprintf("JNI_OnLoad called.\n");
    gJavaVM = jvm;
    return AndroidWrapper::JniVersion; // Shoud be the latest version of the JNI
}

//
// ACTIVITY
//
JNIEXPORT int JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCreate(
        JNIEnv* env, jobject obj)
{
    dsprintf("Creating Dinodeck Android\n");
    dsprintf("Just a test %d", 108);
    assert(gJavaVM);
    AndroidWrapper::CreateInstance(gJavaVM);
    AssetStore::CleverReloadingFlag(false);
    gDinodeck = new Dinodeck("Dinodeck"); // Never deleted
    gDinodeck->ReadInSettingsFile("settings.lua");

    if(gDinodeck->GetSettings().orientation == "portrait")
    {
        return 0; // 0 for portrait .. not the best scheme!
    }
    return 1; // 1 for landscape.
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeIFStream(
    JNIEnv* env, jobject obj, jbyteArray arr, int iSize)
{
    dsprintf("In native ifstream called back.\n");
    jbyte* c_array = env->GetByteArrayElements(arr, NULL);

    DDFile* pOpenFile = DDFile::GetOpenFile();
    if(pOpenFile)
    {
        dsprintf("Calling SetBuffer on open file.\n");
        pOpenFile->SetBuffer((char*)(c_array), iSize);
    }
    else
    {
        dsprintf("Failed to find OPEN file, this an error!\n");
    }

    // release the memory for java
    env->ReleaseByteArrayElements(arr, c_array, JNI_ABORT);
 //   env->DeleteLocalRef(arr);
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackSuccess(
        JNIEnv* env, jobject obj, int callbackId, jstring response)
{
    dsprintf("nativeOnCallbackSuccess\n");

    if(response)
    {
        const char* s = env->GetStringUTFChars(response, 0);
        gCallbackMessages.push_back(CallbackMsg(DS_CALLBACK_SUCCESS, callbackId, s));
        dsprintf("Response: %s\n", s);
        env->ReleaseStringUTFChars(response, s);
    }
    else
    {
        gCallbackMessages.push_back(CallbackMsg(DS_CALLBACK_SUCCESS, callbackId));
        dsprintf("No response.\n");
    }

    // Potential threading issue, so cache the data and take care of it after update

//    DDRestful::OnSuccess(gDinodeck->GetGame()->GetLuaState(), callbackId, strResponse);
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackFailure(
        JNIEnv* env, jobject obj, int callbackId, jstring response)
{
    dsprintf("nativeOnCallbackFailure\n");
    if(response)
    {
        const char* s = env->GetStringUTFChars(response, 0);
        gCallbackMessages.push_back(CallbackMsg(DS_CALLBACK_FAILURE, callbackId, s));
        dsprintf("Response: %s\n", s);
        env->ReleaseStringUTFChars(response, s);
    }
    else
    {
        dsprintf("No response.\n");
        gCallbackMessages.push_back(CallbackMsg(DS_CALLBACK_FAILURE, callbackId));
    }
    //DSRestful::OnFailure(gDinodeck->GetGame()->GetLuaState(), callbackId, strResponse);
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDActivity_nativeOnCallbackFinish(
        JNIEnv*, jobject obj, int callbackId)
{
    dsprintf("nativeOnHttpFinish\n");
    gCallbackMessages.push_back(CallbackMsg(DS_CALLBACK_FINISH, callbackId));
    //DDRestful::OnFinish(gDinodeck->GetGame()->GetLuaState(), callbackId);
}

//
// RENDERER
//

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeClear(
    JNIEnv* pJNIEnv, jobject obj)
{
    glClearColor(0.0, 0.0, 0.0, 0.0f);
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeUpdate(
    JNIEnv*, jobject obj, float dt)
{
    gDinodeck->Update(dt);

    if(!gDinodeck->IsRunning())
    {
        AndroidWrapper::GetInstance()->Exit();
        return;
    }

    // This vector should be locked.
    for(std::vector<CallbackMsg>::iterator iter = gCallbackMessages.begin(); iter != gCallbackMessages.end(); ++iter)
    {
        CallbackMsg& msg = (*iter);
        switch(msg.msgType)
        {
            case DS_CALLBACK_SUCCESS:
            {
                dsprintf("Callback SUCCESS");
                DDLuaCallbacks::OnSuccess(gDinodeck->GetGame()->GetLuaState(), msg.callbackId, msg.response);
            } break;

            case DS_CALLBACK_FAILURE:
            {
                dsprintf("Callback FAILURE");
                DDLuaCallbacks::OnFailure(gDinodeck->GetGame()->GetLuaState(), msg.callbackId, msg.response);
            } break;

            case DS_CALLBACK_FINISH:
            {
                dsprintf("Callback FINISH");
                DDLuaCallbacks::OnFinish(gDinodeck->GetGame()->GetLuaState(), msg.callbackId);
            } break;

            default:
            {
                assert(false);
            };
        }
    }
    gCallbackMessages.clear();
}

JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDRenderer_nativeResize(
    JNIEnv*, jobject obj, int width, int height, float dpiX, float dpiY)
{
    // At the moment this reloads all textures from disk,
    // might be better to store in ram if it gets slow.
    gDinodeck->ForceReload();
    gDinodeck->ResetRenderWindow(width, height);
}


//
// SURFACE
//
JNIEXPORT void JNICALL Java_com_godpatterns_dinodeck_DDGLSurfaceView_nativeOnTouch(
    JNIEnv*, jobject obj, int event, float x, float y)
{
    assert(event >= 1);
    assert(event <= 3);
    Game* game = gDinodeck->GetGame();
    Settings* settings = game->GetSettings();

    static TouchMessage touchMessage;
    touchMessage.mState = (TouchEvent::Enum)event;
    touchMessage.mX = x - (settings->width/2);
    touchMessage.mY = y - (settings->height/2);

    game->GetTouch()->OnTouchEvent(touchMessage);
}
