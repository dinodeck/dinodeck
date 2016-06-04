#ifndef ANDROIDWRAPPER_H
#define ANDROIDWRAPPER_H

#include <jni.h>
#include <string>
// There are AssetManager classes but they're
// only available in SDK > 9
// We're looking to support 2.2 as minimum
// So that option is not open

class HttpPostData;

class AndroidWrapper
{
public:
    static int JniVersion;
private:
    static AndroidWrapper* Instance;
    static const char* DDActivityPath;
    static const char* DDAudioPath;
    static const char* DDScoreLoopPath;
    JavaVM* mJavaVM;
public:
    static void CreateInstance(JavaVM* jVM)
    {
        // Never deleted.
        AndroidWrapper::Instance = new AndroidWrapper(jVM);
    };
    static AndroidWrapper* GetInstance() { return AndroidWrapper::Instance; }

    AndroidWrapper(JavaVM* jVM);



    // Functions on the Java side
    void Exit();
    bool OpenAsset(const char* name);
    bool DoesAssetExist(const char* name);
    void WriteSaveData(const char* name, const char* data);
    void ReadSaveData(const char* name, std::string& data);
    int LoadSound(const char* path);

    // Sound handling on Java side
    int PlaySound(int id, bool loop);
    void StopSound(int id);
    void SetSoundVolume(int id, float volume);
    void PauseSound(int id);
    void ResumeSound(int id);

    int PlayStream(const char* path, bool loop);
    void StopStream(int id);
    void SetStreamVolume(int id, float volume);
    void PauseStream(int id);
    void ResumeStream(int id);

    // Internet type functios on Java side
    void HttpPost(const char* uri,
                  HttpPostData* postData,
                  int callbackId);

    // ScoreLoop
    void ScoreLoopInit(const char* key);
    std::string ScoreLoopTOSState();
    bool ScoreLoopIsInitialized();
    void ScoreLoopShowTOS();
    void ScoreLoopPushScore(double primary, double secondary, int callbackId);
    void ScoreLoopGetLeaderboard(int type, int callbackId);
};

#endif
