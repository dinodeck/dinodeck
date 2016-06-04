#include "Wave.h"

#include <stdio.h>
#if __APPLE__
#include <stdint.h>
#else
//typedef unsigned long uint32_t;
#endif

#include "../DDLog.h"
#include "../DDFile.h"



// Source: http://www.dunsanyinteractive.com/blogs/oliver/?p=72

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header
{
  char chunkID[4];
  uint32_t chunkSize;//size not including chunkSize or chunkID
  char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format
{
  char subChunkID[4];
  uint32_t subChunkSize;
  short audioFormat;
  short numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  short blockAlign;
  short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data
{
  char subChunkID[4]; //should contain the word data
  uint32_t subChunk2Size; //Stores the size of the data block
};

/*
 * Load wave file function. No need for ALUT with this
 */
bool Wave::LoadToOpenALBuffer(const char* filename,
                              ALuint* buffer,
                              ALsizei* size,
                              ALsizei* frequency,
                              ALenum* format)
{
    //Local Declarations
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;
    unsigned char* data;

    // Need to implement



    //FILE* soundFile = fopen(filename, "rb");
    DDFile soundFile(filename);
    if (!DDFile::FileExists(filename))
    {
        dsprintf("ERROR: Failed to load. Couldn't find file [%s].\n", filename);
        return false;
    }

    soundFile.LoadFileIntoBuffer();
    void* readPtr = (void*)soundFile.Buffer();

    // Read in the first chunk into the struct
    //size_t readCount = fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);
    // dsprintf("ReadPtr1 %p.\n", readPtr);
    soundFile.Read(&riff_header, sizeof(RIFF_Header), readPtr);
    // dsprintf("ReadPtr2 %p.\n", readPtr);

    //check for RIFF and WAVE tag in memeory
    if ((riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F') &&
        (riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E'))
    {
        dsprintf("ERROR: Failed to load. Wave header tag missing.\n");
        return false;
    }

    //Read in the 2nd chunk for the wave info
    //readCount = fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
    soundFile.Read(&wave_format, sizeof(WAVE_Format), readPtr);

    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ')
    {
        dsprintf("ERROR: Failed to load. Wave fmt tag missing.\n");
        return false;
    }

    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
    {
        dsprintf("Warning: Wave.cpp untested code running\n");
        //fseek(soundFile, sizeof(short), SEEK_CUR);
        soundFile.Seek((int)sizeof(short), readPtr);
    }

    //Read in the the last byte of data before the sound file
    //fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
    soundFile.Read(&wave_data, sizeof(WAVE_Data), readPtr);
    //check for data tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
    {
        dsprintf("ERROR: Failed to load. Data tag missing.\n");
        return false;
    }

    //Allocate memory for data
    data = new unsigned char[wave_data.subChunk2Size];

    // Read in the sound data into the soundData variable
    if (!soundFile.Read(data, wave_data.subChunk2Size, /*1,*/ readPtr))
    {
        dsprintf("ERROR: Failed to read header into struct.\n");
        delete [] data;
        data = NULL;
        return false;
    }


    //Now we set the variables that we passed in with the
    //data from the structs
    *size = wave_data.subChunk2Size;
    *frequency = wave_format.sampleRate;
    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if(wave_format.numChannels == 1)
    {
        if(wave_format.bitsPerSample == 8)
        {
            *format = AL_FORMAT_MONO8;
        }
        else if(wave_format.bitsPerSample == 16)
        {
            *format = AL_FORMAT_MONO16;
        }

    }
    else if(wave_format.numChannels == 2)
    {
        if(wave_format.bitsPerSample == 8 )
        {
            *format = AL_FORMAT_STEREO8;
        }
        else if(wave_format.bitsPerSample == 16)
        {
            *format = AL_FORMAT_STEREO16;
        }
    }

    //create our openAL buffer and check for success
    alGenBuffers(1, buffer);

    //errorCheck();

    //now we put our data into the openAL buffer and
    //check for success
    alBufferData(*buffer, *format, (void*) data, *size, *frequency);

    delete [] data;
    // errorCheck();

    //clean up and return true if successful
    //fclose(soundFile);

    return true;
}
