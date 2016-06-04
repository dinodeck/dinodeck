#include "../../DDAudio.h"

#include <assert.h>

#include "../../Asset.h"
#include "../../DDLog.h"
#include "AndroidWrapper.h"

DDAudio::DDAudio()
{

}

DDAudio::~DDAudio()
{

}

void DDAudio::Reset()
{
    // Does nothing at the moment
}

int DDAudio::Play(const char* name, bool loop)
{
    dsprintf("Being asked to play [%s] Loop: [%s]", name, loop? "true" : "false");
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();

    // Use name to get id
    int soundId = GetSound(name);

    if(soundId == -1)
    {
        return -1;
    }

    int output = wrapper->PlaySound(soundId, loop);
    return output;
}

void DDAudio::Stop(int id)
{
    dsprintf("Stop sound id [%d]", id);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->StopSound(id);
    return;
}

void DDAudio::Pause(int id)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->PauseSound(id);
}

void DDAudio::Resume(int id)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ResumeSound(id);
}

void DDAudio::SetVolume(int id, float volume)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->SetSoundVolume(id, volume);
}

int DDAudio::PlayStream(const char* name, bool loop)
{
    dsprintf("Being asked to play [%s] Loop: [%s]", name, loop? "true" : "false");
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();

    Asset* asset = GetStream(name);
    assert(asset); // Should have been verified by now.
    const char* path = asset->Path().c_str();

    int streamId = wrapper->PlayStream(path, loop);
    return streamId;
}

void DDAudio::StopStream(int id)
{
    dsprintf("Being asked to stop stream [%d]", id);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->StopStream(id);
}

void DDAudio::PauseStream(int id)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->PauseStream(id);
}

void DDAudio::ResumeStream(int id)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ResumeStream(id);
}

void DDAudio::SetStreamVolume(int id, float volume)
{
    dsprintf("Stream volume set to: %d", volume);
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->SetStreamVolume(id, volume);
}

bool DDAudio::OnAssetReload(Asset& asset)
{
    //
    // On Android absoluely notning is done to cleverly reload
    // sound files. They'll just keep getting shoved in memory!
    //
    if(asset.Type() == Asset::Sound)
    {
        AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
        int soundId = wrapper->LoadSound(asset.Path().c_str());

        mSoundNameId.insert
        (
            std::pair<std::string, int>
            (
                std::string(asset.Name()),
                soundId
            )
        );

        dsprintf("Audio loaded: %d", soundId);
    }
    else if(asset.Type() == Asset::Stream)
    {
        mStreamNameAsset.insert
        (
            std::pair<std::string, Asset*>
            (
                std::string(asset.Name()),
                &asset
            )
        );
        return true;
    }
    else
    {
        assert(false);
    }
}

void DDAudio::OnAssetDestroyed(Asset& asset)
{
    if(asset.Type() == Asset::Sound)
    {
    }
    else if(asset.Type() == Asset::Stream)
    {
        std::map<std::string, Asset*>::iterator iter =
            mStreamNameAsset.find(asset.Name());
        mStreamNameAsset.erase(iter);
    }
    else
    {
        assert(false);
    }
}

int DDAudio::GetSound(const char* name)
{
    if(mSoundNameId.find(name) == mSoundNameId.end())
    {
        dsprintf("ERROR: Couldn't find sound [%s]", name);
        return -1;
    }
    return mSoundNameId.find(name)->second;
}

Asset* DDAudio::GetStream(const char* name)
{
    if(mStreamNameAsset.find(name) == mStreamNameAsset.end())
    {
        dsprintf("ERROR: Couldn't find stream [%s]", name);
        return NULL;
    }
    return mStreamNameAsset.find(name)->second;
}