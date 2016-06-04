#ifndef WAVE_H
#define WAVE_H

#include <AL/al.h>
#include <AL/alc.h>

class Wave
{
public:
    static bool LoadToOpenALBuffer(const char* filename,
                                   ALuint* buffer,
                                   ALsizei* size,
                                   ALsizei* frequency,
                                   ALenum* format);
};

#endif