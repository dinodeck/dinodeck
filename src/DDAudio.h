#ifndef DDAUDIO_ANDROID_H
#define DDAUDIO_ANDROID_H

#include <string>
#include <map>

#include "IAssetOwner.h"
#include "Asset.h"

class DDAudio : public IAssetOwner
{
private:
    std::map<std::string, int> mSoundNameId;
    std::map<std::string, Asset*> mStreamNameAsset;
public:

    DDAudio();
    ~DDAudio();
    void Reset();

    int GetSound(const char* name);
    Asset* GetStream(const char* name);

    int Play(const char* name, bool loop);
    void Stop(int id);
    void Pause(int id);
    void Resume(int id);
    void SetVolume(int id, float volume);

    int PlayStream(const char* name, bool loop);
    void StopStream(int id);
    void PauseStream(int id);
    void ResumeStream(int id);
    void SetStreamVolume(int id, float volume);

    //
    // IAssetOwner implementation.
    //
    bool OnAssetReload(Asset& asset);
    void OnAssetDestroyed(Asset& asset);
};

#endif