#ifndef DDFILE_ANDROID_H
#define DDFILE_ANDROID_H

#include <string>

class DDFile
{
private:
    // Used to communicate asset loading w/ android
    static DDFile* OpenFile;
    std::string mName;
    char* mBuffer;
    unsigned int mSize;
public:

    // Save data may be handled in a special way
    // Depending on the device
    static void WriteSaveData(const char* name, const char* data);
    static void ReadSaveData(const char* name, std::string& data);


    static bool FileExists(const char* path);
    static DDFile* GetOpenFile() { return DDFile::OpenFile; }
    DDFile(const char* filename);
    ~DDFile();
    bool LoadFileIntoBuffer();
    void SetBuffer(char* pData, int iSize);
    const char* Buffer() { return mBuffer; }
    unsigned int Size() { return mSize; }
    void ClearBuffer();

    // Should these be in here or a seperate util library?
    // These are super non-standard as they don't move a file ptr
    // Just the ptr passed in, they're all sorts of dangerous!
    unsigned int Read(void* target, unsigned int size, /*unsigned int count,*/ void*& source);
    int Seek(/*FILE * stream,*/ int offset, void*& origin);
};

#endif