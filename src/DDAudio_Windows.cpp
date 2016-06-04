#include "DDAudio.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <assert.h>

#include "audio/Wave.h"
#include "DDLog.h"

static const unsigned int MAX_SOUND_CHANNELS = 256;
std::vector<unsigned int> gChannels;


ALCdevice* gDevice = NULL;
ALCcontext* gContext = NULL;

bool IsSoundPlaying(unsigned int channel)
{
    int value;
    alGetSourcei(channel, AL_SOURCE_STATE, &value);
    return value == AL_PLAYING;
}

int FindNextFreeChannel()
{
    for (std::vector<unsigned int>::iterator i = gChannels.begin(); i != gChannels.end(); ++i)
    {
        if(!IsSoundPlaying(*i))  // potential issue, if sound just paused!
        {
            return *i;
        }
    }
    return -1;
}


DDAudio::DDAudio()
{
    gDevice = alcOpenDevice(NULL);
    gContext = alcCreateContext(gDevice, NULL);
    alcMakeContextCurrent(gContext);

    int error = alGetError();
    if(error != AL_NO_ERROR)
    {
        dsprintf("OpenAL setup error : %d", error);
        return;
    }

    // // Defaults for 2d sound
    // alListener3f(AL_POSITION, 0, 0, 0);
    // alListener3f(AL_VELOCITY, 0, 0, 0);
    // alListener3f(AL_ORIENTATION, 0, 0, -1);


    //
    // Reserve sound channels.
    //

    gChannels.clear();

    while(gChannels.size() < MAX_SOUND_CHANNELS)
    {
        unsigned int src;
        alGenSources(1, &src);
        error = alGetError();
        if(error == AL_NO_ERROR)
        {
            gChannels.push_back(src);
        }
        else
        {
            dsprintf("OpenAL sound channel error : %d\n", error);
            break;
        }
    }

    dsprintf("%d sound channels found.\n", (int)gChannels.size());
}

DDAudio::~DDAudio()
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(gContext);
    alcCloseDevice(gDevice);
    gContext = NULL;
    gDevice = NULL;
}


void DDAudio::Reset()
{
    // Stop the sounds overlapping and going mental
    for (std::vector<unsigned int>::iterator i = gChannels.begin(); i != gChannels.end(); ++i)
    {
        alSourceStop(*i);
    }
}

bool DDAudio::OnAssetReload(Asset& asset)
{
    dsprintf("Being asked to load [%s]\n", asset.Name().c_str());
    if(asset.Type() == Asset::Sound)
    {
        ALuint buffer;
        ALsizei size;
        ALsizei frequency;
        ALenum format;

        bool success = Wave::LoadToOpenALBuffer(asset.Path().c_str(),
                                 &buffer, &size, &frequency, &format);

        if(!success)
        {
            return false;
        }

        mSoundNameId.insert
        (
            std::pair<std::string, int>
            (
                std::string(asset.Name()),
                buffer
            )
        );

        return true;
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

    return false;
}

void DDAudio::OnAssetDestroyed(Asset& asset)
{
    if(asset.Type() == Asset::Sound)
    {
        std::map<std::string, int>::iterator iter =
            mSoundNameId.find(asset.Name());
        ALuint buffer = iter->second;
        alDeleteBuffers(1, &buffer);
        mSoundNameId.erase(iter);
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

int DDAudio::Play(const char* name, bool loop)
{
    dsprintf("Being asked to play [%s] Loop: [%s]\n", name, loop? "true" : "false");

    int buffer = GetSound(name);


    int channel = FindNextFreeChannel();
    alSourceStop(channel);
    if(channel == -1)
    {
        printf("Couldn't find free channel.\n");
        return -1;
    }


    alSourcei(channel, AL_BUFFER, buffer);

    alSourcef(channel, AL_PITCH, 1);
    alSourcef(channel, AL_GAIN, 1);
    alSource3f(channel, AL_POSITION, 0, 0, 0);
    alSource3f(channel, AL_VELOCITY, 0, 0, 0);
    alSourcei(channel, AL_LOOPING, loop);

    alSourcePlay(channel);

    int error = alGetError();
    if(error != AL_NO_ERROR)
    {
        return -1;
    }

    return channel;
}

void DDAudio::Stop(int soundId)
{
    alSourceStop(soundId);
}

void DDAudio::Pause(int soundId)
{
    alSourcePause(soundId);
}

void DDAudio::Resume(int soundId)
{
    alSourcePlay(soundId);
}

void DDAudio::SetVolume(int soundId, float volume)
{
    alSourcef(soundId, AL_GAIN, volume);
}


int DDAudio::PlayStream(const char* path, bool loop)
{
    dsprintf("[%s] not loaded. Streaming not currently supported on windows.\n",
             path);
    return -1;
}

void DDAudio::StopStream(int soundId)
{
    // Not currently implemented on windows.
}

void DDAudio::PauseStream(int soundId)
{

}

void DDAudio::ResumeStream(int soundId)
{

}

void DDAudio::SetStreamVolume(int soundId, float volume)
{

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