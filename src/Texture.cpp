// Your local header file
#include "Texture.h"

#include <assert.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
//#include <windows.h>

#include "DinodeckGL.h"
#include "DinodeckLua.h"
#include "DDFile.h"
#include "DDLog.h"
#include "Game.h"
#include "LuaState.h"
#include "reflect/Reflect.h"
#include "soil.h"
#include "TextureManager.h"

Reflect Texture::Meta("Texture", Texture::Bind);

Texture::Texture() :
    mTextureId(0), mWidth(0), mHeight(0)
{
}

Texture::~Texture()
{
    //dsprintf("Removing texture %d\n", mTextureId);
    // Tell opengl to get rid of the texture
    // glDeleteTextures silently ignores 0's and names that do not correspond to
    // existing textures.
    glDeleteTextures(1, &mTextureId);
}

static int lua_Texture_tostring(lua_State* state)
{
    lua_pushfstring(state, "Texture");
    return 1;
}

static int lua_Texture_GetHeight(lua_State* state)
{
    Texture** texture = (Texture**)lua_touserdata(state, 1);
    if(texture == NULL)
    {
        return luaL_typerror(state, 1, "Texture");
    }
    lua_pushnumber(state, (*texture)->GetHeight());
    return 1;
}

static int lua_Texture_GetWidth(lua_State* state)
{
    Texture** texture = (Texture**)lua_touserdata(state, 1);
    if(texture == NULL)
    {
        return luaL_typerror(state, 1, "Texture");
    }
    lua_pushnumber(state, (*texture)->GetWidth());
    return 1;
}

static int lua_Texture_Find(lua_State* state)
{
    //
    // Textures are allocated at startup time
    // Therefore Lua doesn't need to worry about their memory management
    // Therefore only the texture pointer is passed to lua
    // Unlike Vector where lua manages the entire memory block
    //

    if(!lua_isstring(state, 1))
    {
        return luaL_typerror(state, 1, "string");
    }
    const char* textureName = lua_tostring(state, 1);

    Game* game = (Game*) LuaState::GetFromRegistry(state, "Game");
    assert(game);

    Texture* foundTexture = game->Textures()->GetTexture(textureName);

    if(!foundTexture)
    {
        return luaL_error(state, "Texture not found [%s]", textureName);
    }

    Texture **pi = (Texture **)lua_newuserdata(state, sizeof(Texture*));
    (*pi) = foundTexture;

    luaL_getmetatable(state, "Texture");
    lua_setmetatable(state, -2);

    return 1;
}

static const struct luaL_reg luaBinding [] = {
  {"__tostring", lua_Texture_tostring},
  {"GetWidth", lua_Texture_GetWidth},
  {"GetHeight", lua_Texture_GetHeight},
  {"Find", lua_Texture_Find},
  {NULL, NULL}  /* sentinel */
};


// void check_for_GL_errors( const char *calling_location )
// {
//     /*  check for errors    */
//     GLenum err_code = glGetError();
//     while( GL_NO_ERROR != err_code )
//     {
//         printf( "OpenGL Error @ %s: %i", calling_location, err_code );
//         err_code = glGetError();
//     }
// }


unsigned int CreateTexture(unsigned char* const img, int width, int height, int channels, bool pixelArt)
{
    /*  variables   */
    unsigned int tex_id = 0;
    unsigned int internal_texture_format = 0;
    unsigned int original_texture_format = 0;
    unsigned int opengl_texture_type = GL_TEXTURE_2D;
    unsigned int opengl_texture_target = GL_TEXTURE_2D;

    if( tex_id == 0 )
    {
        glGenTextures( 1, &tex_id );
    }
    /* Note: sometimes glGenTextures fails (usually no OpenGL context)  */
    if( tex_id )
    {
        /*  and what type am I using as the internal texture format?    */
        switch( channels )
        {
        case 1:
            original_texture_format = GL_LUMINANCE;
            break;
        case 2:
            original_texture_format = GL_LUMINANCE_ALPHA;
            break;
        case 3:
            original_texture_format = GL_RGB;
            break;
        case 4:
            original_texture_format = GL_RGBA;
            break;
        }
        internal_texture_format = original_texture_format;
        /*  bind an OpenGL texture ID   */
        glBindTexture( opengl_texture_type, tex_id );

        /*  user want OpenGL to do all the work!    */
        glTexImage2D(
            opengl_texture_target, 0,
            internal_texture_format, width, height, 0,
            original_texture_format, GL_UNSIGNED_BYTE, img );

        if(pixelArt)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        else
        {
            /*  instruct OpenGL _NOT_ to use the MIPmaps    */
            glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        }
    }
    return tex_id;
}


// I've manually rewritten parts of this to get it to compile on Android.
// This means the DDS stuff doesn't work anymore!
// DDS files should be able to be loaded even on android,
// so it's worth investigating
bool Texture::LoadDDSTexture(const char *filename, bool pixelArt)
{

//    dsprintf("LoadDDSTexture('%s')\n", filename);

    unsigned char* image = NULL;
    int channels = 0;

    // Load texture for disk
    {
        DDFile file(filename);
        file.LoadFileIntoBuffer();

        if(NULL == file.Buffer())
        {
            dsprintf("Failed to load [%s]\n", filename);
            return false;
        }

        image = SOIL_load_image_from_memory
        (
            (const unsigned char*) file.Buffer(),
            file.Size(),
            &mWidth, &mHeight, &channels,
            SOIL_LOAD_AUTO
        );
    }

    if(image == NULL)
    {
        dsprintf("Texture failed to load:[%s]\n", filename);
        return false;
    }
    // else
    // {
    //      dsprintf("Load from memory ok:[%s]\n", filename);
    // }

    // Crashes on android (even with no flags)
    // GLuint tex_2d = SOIL_create_OGL_texture
    // (
    //     image,
    //     mWidth, mHeight, channels,
    //     SOIL_CREATE_NEW_ID,
    //     SOIL_FLAG_MIPMAPS /*| SOIL_FLAG_INVERT_Y*/ | SOIL_FLAG_NTSC_SAFE_RGB //| SOIL_FLAG_COMPRESS_TO_DXT
    // );

    // Android crashes under SOIL_create_OGL_texture
    // Haven't properly invesitgated why but this stripped down function will
    // do for now. It's also more efficient as it does copy the image data.
    GLuint tex_2d = CreateTexture(image, mWidth, mHeight, channels, pixelArt);

    SOIL_free_image_data
    (
        image
    );

    mTextureId = tex_2d;
    return true;
}



void Texture::Bind(LuaState* state)
{
    state->Bind
    (
        Texture::Meta.Name(),
        luaBinding
    );
}

std::string Texture::ToString() const
{
    std::stringstream ss;
    ss << "Texture ["
       << GetWidth() << "x"
       << GetHeight() << "]";
    return ss.str();
}