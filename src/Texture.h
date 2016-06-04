#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <string>

#include "DinodeckGL.h"
#include "reflect/Reflect.h"

class Asset;
class LuaState;

// Find will return lightuserdata
// All removing / adding handled here

class Texture
{
    public: static Reflect Meta;
    private:
        GLuint mTextureId;
        int mWidth;
        int mHeight;
    public:
        static void Bind(LuaState* state);
        Texture();
        ~Texture();
        bool LoadDDSTexture(const char* filename, bool pixelArt);
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }
        GLuint GetId() const { return mTextureId; }
        std::string ToString() const;
};

#endif