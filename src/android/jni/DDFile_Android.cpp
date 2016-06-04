#include "../../DDFile.h"

#include <fstream>
#include <jni.h>
#include <unistd.h>

#include "AndroidWrapper.h"
#include "../../DDLog.h"

DDFile* DDFile::OpenFile = NULL;


void DDFile::WriteSaveData(const char* name, const char* data)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->WriteSaveData(name, data);
}

void DDFile::ReadSaveData(const char* name, std::string& data)
{
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    wrapper->ReadSaveData(name, data);
}

DDFile::DDFile(const char* filename)
    : mName(filename), mBuffer(NULL), mSize(0)
{

}

DDFile::~DDFile()
{
    ClearBuffer();
}

bool DDFile::FileExists(const char* path)
{
    return true;
}

//
// Loads a file from the APK.
// Calls OpenAsset which talks to Java
// Java calls nativeIFStream which calls
// SetBuffer
//
bool DDFile::LoadFileIntoBuffer()
{
    OpenFile = this;
    AndroidWrapper* wrapper = AndroidWrapper::GetInstance();
    dsprintf("Opening asset: %s", mName.c_str());
    bool result = wrapper->OpenAsset(mName.c_str());

    if(!result)
    {
        dsprintf("Called to Java to open asset failed: %s", mName.c_str());
    }

    OpenFile = NULL;
    return result;
}

void DDFile::SetBuffer(char* pData, int iSize)
{
    dsprintf("Set buffer called size:%d", iSize);
    ClearBuffer();
    mBuffer = new char[iSize];
    memcpy( mBuffer, pData, sizeof(char)*iSize );
    mSize = iSize;
}

void DDFile::ClearBuffer()
{
    if(mBuffer)
    {
        delete[] mBuffer;
        mBuffer = NULL;
        mSize = 0;
    }
}