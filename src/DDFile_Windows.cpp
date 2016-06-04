#include "DDFile.h"

#include <stdio.h>
#include <string.h> // for memcpy
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "physfs.h"
#include "DDLog.h"

DDFile* DDFile::OpenFile = NULL;

void DDFile::WriteSaveData(const char* name, const char* data)
{
    std::ofstream outFile;
    outFile.open(name);
    outFile << data;
    outFile.close();
}

void DDFile::ReadSaveData(const char* name, std::string& data)
{
    std::ifstream ifs(name);
    if(ifs.good())
    {
        data.assign
        (
            std::istreambuf_iterator<char>(ifs),
            std::istreambuf_iterator<char>()
        );
    }
    else
    {
        data = "";
    }
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
    //
    // Try to find files in the dir root,
    // then try the packages.
    //
    struct stat attributes;

    if(stat(path, &attributes) == 0
       && S_ISREG(attributes.st_mode))
    {
        return true;
    }

    return PHYSFS_exists(path);

}

bool DDFile::LoadFileIntoBuffer()
{

    OpenFile = this;
    bool result = false;
    const char* path = mName.c_str();
    PHYSFS_file* physFile = PHYSFS_openRead(path);


    if(!physFile)
    {
        OpenFile = NULL;
        printf("Failed to open [%s]\n", path);
        return false;
    }

    ClearBuffer();

    mSize = PHYSFS_fileLength(physFile);
    mBuffer = new char[mSize * sizeof(char)];
    PHYSFS_read(physFile, (void*)mBuffer, 1, mSize);
    PHYSFS_close(physFile);

    OpenFile = NULL;
    return result;
}

// Reads an array of count elements, each one with a size of size bytes,
// from the stream and stores them in the block of memory specified by ptr.
// The position indicator of the stream is advanced by the total amount of bytes read.
unsigned int DDFile::Read(void* target, unsigned int size, /*unsigned int count,*/ void*& source)
{
    char* sourcePtr = (char*)source;
    char* targetPtr = (char*)target;

    // Copy data byte by byte
    int i = 0;
    while(i < size)
    {
        *targetPtr++ = *sourcePtr++;
        i++;
    }

    // Advance source pointer
    source = (void*)sourcePtr;
    return 1;
}

// If successful, the function returns zero.
// Otherwise, it returns non-zero value.
int DDFile::Seek(int offset, void*& origin)
{
    char* originPtr = (char*)origin;
    originPtr += offset;
    origin = (void*)originPtr;
    return 1;
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

void DDFile::SetBuffer(char* pData, int iSize)
{
    ClearBuffer();
    mBuffer = new char[iSize];
    memcpy( mBuffer, pData, sizeof(char)*iSize );
    mSize = iSize;
}