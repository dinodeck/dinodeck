#include "AndroidWrapper.h"

#include <assert.h>
#include <stdint.h>
#include "../../DDLog.h"
#include "../../HttpPostData.h"

int AndroidWrapper::JniVersion = JNI_VERSION_1_6;
AndroidWrapper* AndroidWrapper::Instance = NULL;
const char* AndroidWrapper::DDActivityPath = "com/godpatterns/dinodeck/DDActivity";
const char* AndroidWrapper::DDAudioPath = "com/godpatterns/dinodeck/DDAudio";
const char* AndroidWrapper::DDScoreLoopPath = "com/godpatterns/dinodeck/DDScoreLoop";

AndroidWrapper::AndroidWrapper(JavaVM* jVM) :
    mJavaVM(jVM)
{}

void AndroidWrapper::Exit()
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddActivityClass = env->FindClass(DDActivityPath);
    jmethodID java_Exit = env->GetStaticMethodID
    (
        ddActivityClass,
        "exit",
        "()V"
    );

    env->CallStaticVoidMethod
    (
        ddActivityClass,
        java_Exit
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

bool AndroidWrapper::OpenAsset(const char* name)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return false;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddActivityClass = env->FindClass(DDActivityPath);
    jstring jStrPath = env->NewStringUTF(name);

    jmethodID java_OpenAsset = env->GetStaticMethodID
    (
        ddActivityClass,
        "open_asset",
        "(Ljava/lang/String;)Z"
    );

    bool result = env->CallStaticBooleanMethod
    (
        ddActivityClass,
        java_OpenAsset,
        jStrPath
    );
    env->DeleteLocalRef(jStrPath);


    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }

    return result;
}

bool AndroidWrapper::DoesAssetExist(const char* name)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return false;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddActivityClass = env->FindClass(DDActivityPath);
    jstring jStrPath = env->NewStringUTF(name);

    jmethodID java_DoesAssetExist = env->GetStaticMethodID
    (
        ddActivityClass,
        "does_asset_exist",
        "(Ljava/lang/String;)Z"
    );

    bool result = env->CallStaticBooleanMethod
    (
        ddActivityClass,
        java_DoesAssetExist,
        jStrPath
    );
    env->DeleteLocalRef(jStrPath);

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }

    return result;
}

void AndroidWrapper::WriteSaveData(const char* name, const char* data)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddActivityClass = env->FindClass(DDActivityPath);

    jmethodID java_WriteSaveData = env->GetStaticMethodID
    (
        ddActivityClass,
        "write_save_data",
        "(Ljava/lang/String;Ljava/lang/String;)V"
    );

    jstring jStrPath = env->NewStringUTF(name);
    jstring jStrData = env->NewStringUTF(data);
    env->CallStaticVoidMethod
    (
        ddActivityClass,
        java_WriteSaveData,
        jStrPath,
        jStrData
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::ReadSaveData(const char* name, std::string& data)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddActivityClass = env->FindClass(DDActivityPath);

    jmethodID java_ReadSaveData = env->GetStaticMethodID
    (
        ddActivityClass,
        "read_save_data",
        "(Ljava/lang/String;)Ljava/lang/String;"
    );

    jstring jStrPath = env->NewStringUTF(name);
    jstring jStrData = (jstring) env->CallStaticObjectMethod
    (
        ddActivityClass,
        java_ReadSaveData,
        jStrPath
    );

    const char* result = env->GetStringUTFChars(jStrData, 0);
    data.assign(result);

    dsprintf("Data: [%s]", result);

    env->ReleaseStringUTFChars(jStrData, result);

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

int AndroidWrapper::LoadSound(const char* path)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return -1;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_LoadSound = env->GetStaticMethodID
    (
        ddAudioClass,
        "load_sound",
        "(Ljava/lang/String;)I"
    );

    jstring jStrPath = env->NewStringUTF(path);
    int result = env->CallStaticIntMethod
    (
        ddAudioClass,
        java_LoadSound,
        jStrPath
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
    return result;
}

int AndroidWrapper::PlaySound(int id, bool loop)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return -1;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_PlaySound = env->GetStaticMethodID
    (
        ddAudioClass,
        "play_sound",
        "(IZ)I"
    );

    int result = env->CallStaticIntMethod
    (
        ddAudioClass,
        java_PlaySound,
        id,
        loop
    );


    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
    return result;
}

void AndroidWrapper::StopSound(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_StopSound = env->GetStaticMethodID
    (
        ddAudioClass,
        "stop_sound",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_StopSound,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

int AndroidWrapper::PlayStream(const char* path, bool loop)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return false;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_PlayStream = env->GetStaticMethodID
    (
        ddAudioClass,
        "play_stream",
        "(Ljava/lang/String;Z)I"
    );

    jstring jStrPath = env->NewStringUTF(path);
    int result = env->CallStaticIntMethod
    (
        ddAudioClass,
        java_PlayStream,
        jStrPath,
        loop
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
    return result;
}

void AndroidWrapper::StopStream(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_StopStream = env->GetStaticMethodID
    (
        ddAudioClass,
        "stop_stream",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_StopStream,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::SetStreamVolume(int id, float volume)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_SetStreamVolume = env->GetStaticMethodID
    (
        ddAudioClass,
        "set_stream_volume",
        "(IF)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_SetStreamVolume,
        id,
        volume
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::PauseStream(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_PauseStream = env->GetStaticMethodID
    (
        ddAudioClass,
        "pause_stream",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_PauseStream,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::ResumeStream(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_ResumeStream = env->GetStaticMethodID
    (
        ddAudioClass,
        "resume_stream",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_ResumeStream,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::SetSoundVolume(int id, float volume)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_SetSoundVolume = env->GetStaticMethodID
    (
        ddAudioClass,
        "set_sound_volume",
        "(IF)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_SetSoundVolume,
        id,
        volume
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::PauseSound(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_SoundPause = env->GetStaticMethodID
    (
        ddAudioClass,
        "sound_pause",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_SoundPause,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::ResumeSound(int id)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddAudioClass = env->FindClass(DDAudioPath);

    jmethodID java_SoundResume = env->GetStaticMethodID
    (
        ddAudioClass,
        "sound_resume",
        "(I)V"
    );

    env->CallStaticVoidMethod
    (
        ddAudioClass,
        java_SoundResume,
        id
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::HttpPost(const char* uri,
                              HttpPostData* postData,
                              int callbackId)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return;
        }
        isAttached = true;
    }
    assert(env);

    dsprintf("Finding http post java functions.\n");

    jclass ddActivityClass = env->FindClass(DDActivityPath);
    jmethodID java_StartHttpPost = env->GetStaticMethodID
    (
        ddActivityClass,
        "start_http_post",
        "(Ljava/lang/String;I)V"
    );

    jmethodID java_FinishHttpPost = env->GetStaticMethodID
    (
        ddActivityClass,
        "finish_http_post",
        "()V"
    );

    jmethodID java_AddHttpPostData = env->GetStaticMethodID
    (
        ddActivityClass,
        "add_http_post_data",
        "(Ljava/lang/String;Ljava/lang/String;)V"
    );

    dsprintf("Found the functions [%s] [%s] [%s].\n",
             java_StartHttpPost != NULL? "true":"false",
             java_FinishHttpPost != NULL? "true":"false",
             java_AddHttpPostData != NULL? "true":"false");


    dsprintf("Before call start. Callback id: %d\n", callbackId);
    // Start post (uri, success, failure)
    jstring jStrURI = env->NewStringUTF(uri);
    env->CallStaticVoidMethod
    (
        ddActivityClass,
        java_StartHttpPost,
        jStrURI,
        callbackId
    );
    dsprintf("After call start.\n");
    env->DeleteLocalRef(jStrURI);
    dsprintf("After call start - string deallcated.\n");

    dsprintf("Before post data.\n");
    // Send all data
    if(NULL != postData)
    {
        for(std::map<std::string, std::string>::iterator
            iter = postData->mValueMap.begin();
            iter != postData->mValueMap.end();
            ++iter)
        {
            jstring jStrKey = env->NewStringUTF(iter->first.c_str());
            jstring jStrValue = env->NewStringUTF(iter->second.c_str());

            env->CallStaticVoidMethod
            (
                ddActivityClass,
                java_AddHttpPostData,
                jStrKey,
                jStrValue
            );

            env->DeleteLocalRef(jStrKey);
            env->DeleteLocalRef(jStrValue);
        }
    }
    dsprintf("After post data.\n");

    // Finish post -> triggers the send
    dsprintf("Before finish.\n");
    env->CallStaticVoidMethod
    (
        ddActivityClass,
        java_FinishHttpPost
    );
    dsprintf("After finish.\n");

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::ScoreLoopInit(const char* key)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);

    jmethodID java_Init = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "init",
        "(Ljava/lang/String;)V"
    );

    jstring jStrKey = env->NewStringUTF(key);

    env->CallStaticVoidMethod
    (
        ddScoreLoopClass,
        java_Init,
        jStrKey
    );

    env->DeleteLocalRef(jStrKey);

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

std::string AndroidWrapper::ScoreLoopTOSState()
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return "";
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);

    jmethodID java_get_tos_state = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "get_tos_state",
        "()Ljava/lang/String;"
    );

    dsprintf("Before calling function\n");

    jstring jStrData = (jstring) env->CallStaticObjectMethod
    (
        ddScoreLoopClass,
        java_get_tos_state
    );

    dsprintf("Before getting result\n");


    const char* result = env->GetStringUTFChars(jStrData, 0);
    //data.assign(result);
    std::string output = std::string(result);

    dsprintf("TOS State:[%s]\n", result);

    env->ReleaseStringUTFChars(jStrData, result);

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }

    return output;
}

bool AndroidWrapper::ScoreLoopIsInitialized()
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return false;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);
    jmethodID java_is_initialized = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "is_initialized",
        "()Z"
    );

    bool result = env->CallStaticBooleanMethod
    (
        ddScoreLoopClass,
        java_is_initialized
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }

    return result;
}

void AndroidWrapper::ScoreLoopShowTOS()
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);
    jmethodID java_show_tos = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "show_tos",
        "()V"
    );

    env->CallStaticVoidMethod
    (
        ddScoreLoopClass,
        java_show_tos
    );


    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }

}

void AndroidWrapper::ScoreLoopPushScore(double primary, double secondary,
                                        int callbackId)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);
    jmethodID java_push_score = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "push_score",
        "(DDI)V"
    );

    env->CallStaticVoidMethod
    (
        ddScoreLoopClass,
        java_push_score,
        primary,
        secondary,
        callbackId
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}

void AndroidWrapper::ScoreLoopGetLeaderboard(int type, int callbackId)
{
    JNIEnv *env;
    bool isAttached = false;

    if(mJavaVM->GetEnv((void**)&env, JniVersion) < 0)
    {
        if(mJavaVM->AttachCurrentThread(&env, NULL) < 0)
        {
            return /*false*/;
        }
        isAttached = true;
    }
    assert(env);

    jclass ddScoreLoopClass = env->FindClass(DDScoreLoopPath);
    jmethodID java_get_leaderboard = env->GetStaticMethodID
    (
        ddScoreLoopClass,
        "get_leaderboard",
        "(II)V"
    );

    env->CallStaticVoidMethod
    (
        ddScoreLoopClass,
        java_get_leaderboard,
        type,
        callbackId
    );

    if(isAttached)
    {
        mJavaVM->DetachCurrentThread();
    }
}