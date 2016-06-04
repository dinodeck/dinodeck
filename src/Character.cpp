#include "Character.h"

#include <assert.h>

#include "DDLog.h"
#include "Game.h"
#include "GraphicsPipeline.h"
#include "LuaState.h"
#include "reflect/Reflect.h"
#include "Renderer.h"

#ifdef ANDROID
#include <FTGL/ftgles.h>
#include "FTGL/../FTGlyph/FTTextureGlyphImpl.h"
#else
#include <FTGL/ftgl.h>
#include "FTGL/../FTGlyph/FTTextureGlyphImpl.h"
#include "FTGL/../FTFont/FTTextureFontImpl.h"
#include "FTGL/../FTGlyphContainer.h"
#include "FTGL/../FTCharmap.h"
#include "FTGL/../FTVector.h"
#endif

// Move mFont->Render code to character.

Reflect Character::Meta("Character", Character::Bind);

Character::Character()
    : mX(0), mY(0), mGlyph('\0')
{
}

Character::~Character()
{
}

static int lua_Character_Create(lua_State* state)
{
        // If there's inheritance I think this may causes problems
    if(lua_isuserdata(state, 1) && luaL_checkudata (state, 1, "Character"))
    {
        Character* character = (Character*)lua_touserdata(state, 1);
        Character* pi = new (lua_newuserdata(state, sizeof(Character))) Character();
        if(character == NULL)
        {
            return luaL_argerror(state, 1, "Expected Character or string");
        }
        pi->SetGlyph(character->GetGlyph());
    }
    else
    {
        const char* str = lua_tostring(state, 1);
        if(NULL == str)
        {
            return luaL_argerror(state, 1, "string");
        }

        Character* pi = new (lua_newuserdata(state, sizeof(Character))) Character();
        pi->SetGlyph(str[0]);

    }
    luaL_getmetatable(state, "Character");
    lua_setmetatable(state, -2);
    return 1;
}

static int lua_Character_FaceMaxHeight(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 1, "Renderer");
    }

    GraphicsPipeline* gp = renderer->Graphics();

    lua_pushnumber(state, Character::GetMaxHeight(gp));
    return 1;
}

float Character::GetKern(GraphicsPipeline* gp, unsigned int current, unsigned int next)
{
    FTTextureFont* font = gp->GetFont();
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
        // Not sure if necessary.
    if(!impl->CheckGlyph(current) || !impl->CheckGlyph(next))
    {
        return 0;
    }
    return glyphList->Advance(current, next);
}

static int lua_Character_GetKern(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 1, "Renderer");
    }

    const char* firstStr = lua_tostring(state, 2);
    const char* secondStr = lua_tostring(state, 3);

    unsigned int first = 0;
    unsigned int second = 0;

    if(firstStr)
    {
        first = firstStr[0];
    }

    if(secondStr)
    {
        second = secondStr[0];
    }

    GraphicsPipeline* gp = renderer->Graphics();
    lua_pushnumber(state, Character::GetKern(gp, first, second));
    return 1;
}

static int lua_Character_PixelWidth(lua_State* state)
{
    Character* character = (Character*)lua_touserdata(state, 1);
    if(character == NULL)
    {
        return luaL_typerror(state, 1, "Character");
    }

    Renderer* renderer = (Renderer*)lua_touserdata(state, 2);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 2, "Renderer");
    }

    GraphicsPipeline* gp = renderer->Graphics();

    lua_pushnumber(state, character->GetPixelWidth(gp));
    return 1;
}

static int lua_Character_GetCorner(lua_State* state)
{
    Character* character = (Character*)lua_touserdata(state, 1);
    if(character == NULL)
    {
        return luaL_typerror(state, 1, "Character");
    }

    Renderer* renderer = (Renderer*)lua_touserdata(state, 2);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 2, "Renderer");
    }

    GraphicsPipeline* gp = renderer->Graphics();

    FTPoint point = character->GetCorner(gp);
    lua_pushnumber(state, point.X());
    lua_pushnumber(state, point.Y());
    lua_pushnumber(state, point.Z());
    return 3;
}

static int lua_Character_PixelHeight(lua_State* state)
{
    Character* character = (Character*)lua_touserdata(state, 1);
    if(character == NULL)
    {
        return luaL_typerror(state, 1, "Character");
    }

    Renderer* renderer = (Renderer*)lua_touserdata(state, 2);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 2, "Renderer");
    }

    GraphicsPipeline* gp = renderer->Graphics();

    lua_pushnumber(state, character->GetPixelHeight(gp));
    return 1;
}

int Character::GetPixelWidth(GraphicsPipeline* gp)
{
    FTTextureFont* font = gp->GetFont();
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    unsigned int charCode = (unsigned int) mGlyph;

    // This isn't just a check! It's lazy loader
    if(!impl->CheckGlyph(charCode))
    {
        return 0;
    }

    unsigned int index = charMap->GlyphListIndex(charCode);
    FTTextureGlyphImpl* glyph = (FTTextureGlyphImpl*) (*glyphVector)[index]->GetImpl();

    return glyph->GetWidth();
}

int Character::GetPixelHeight(GraphicsPipeline* gp)
{
    FTTextureFont* font = gp->GetFont();
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    unsigned int charCode = (unsigned int) mGlyph;

    // This isn't just a check! It's lazy loader
    if(!impl->CheckGlyph(charCode))
    {
        return 0;
    }

    unsigned int index = charMap->GlyphListIndex(charCode);
    FTTextureGlyphImpl* glyph = (FTTextureGlyphImpl*) (*glyphVector)[index]->GetImpl();

    return glyph->GetHeight();
}

FTPoint Character::GetCorner(GraphicsPipeline* gp)
{
    FTTextureFont* font = gp->GetFont();
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    unsigned int charCode = (unsigned int) mGlyph;

    // This isn't just a check! It's lazy loader
    if(!impl->CheckGlyph(charCode))
    {
        return FTPoint();
    }

    unsigned int index = charMap->GlyphListIndex(charCode);
    FTTextureGlyphImpl* glyph = (FTTextureGlyphImpl*) (*glyphVector)[index]->GetImpl();

    FTPoint point = glyph->GetCorner();
    return point;
}


int Character::GetMaxHeight(GraphicsPipeline* gp)
{
    FTTextureFont* font = gp->GetFont();
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    return impl->GetMaxHeight();
}

void Character::DrawGlyph(GraphicsPipeline* gp)
{
    FTTextureFont* font = gp->GetFont();

    // FTTextureFont->Render is a virtual function from FTFont
    //
    //     virtual FTPoint Render(const char* string, const int len = -1,
    //                            FTPoint position = FTPoint(),
    //                            FTPoint spacing = FTPoint(),
    //                            int renderMode = FTGL::RENDER_ALL);
    //
    // This is implemented in FTTextureFontImpl::Render
    //
    //     FTPoint FTTextureFontImpl::Render(const char * string, const int len,
    //                                       FTPoint position, FTPoint spacing,
    //                                       int renderMode)
    //     {
    //         return RenderI(string, len, position, spacing, renderMode);
    //     }
    //
    // This is implemented as
    //        template <typename T>
    //        inline FTPoint FTTextureFontImpl::RenderI(const T* string, const int len,
    //                                                  FTPoint position, FTPoint spacing,
    //                                                  int renderMode)
    //       {
    //            // Protect GL_TEXTURE_2D, GL_BLEND and blending functions
    //            glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    //
    //            glEnable(GL_BLEND);
    //            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE
    //
    //            glEnable(GL_TEXTURE_2D);
    //
    //            FTTextureGlyphImpl::ResetActiveTexture();
    //
    //            FTPoint tmp = FTFontImpl::Render(string, len,
    //                                             position, spacing, renderMode);
    //
    //            glPopAttrib();
    //
    //            return tmp;
    //        }
    //
    //mFont->Render(text);

    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

    //FTTextureGlyphImpl::ResetActiveTexture();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    unsigned int charCode = (unsigned int) mGlyph;

    // This isn't just a check! It's lazy loader
    if(!impl->CheckGlyph(charCode))
    {
        return;
    }

    unsigned int index = charMap->GlyphListIndex(charCode);
    (*glyphVector)[index]->Render(FTPoint(mX, mY), FTGL::RENDER_ALL);

    glPopAttrib();
}

static int lua_Character_Render(lua_State* state)
{
    Character* character = (Character*)lua_touserdata(state, 1);
    if(character == NULL)
    {
        return luaL_typerror(state, 1, "Character");
    }

    Renderer* renderer = (Renderer*)lua_touserdata(state, 2);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 2, "Renderer");
    }

    GraphicsPipeline* gp = renderer->Graphics();

    character->DrawGlyph(gp);
    // gp->PushText
    // (
    //     character->mX,
    //     character->mY,
    //     std::string(1, character->mGlyph).c_str(),
    //     Vector(1,1,1,1),
    //     99999
    // );

    return 0;
}

static int lua_Character_SetPosition(lua_State* state)
{
    Character* character = (Character*)lua_touserdata(state, 1);
    if(character == NULL)
    {
        return luaL_typerror(state, 1, "Character");
    }

    if(lua_isnumber(state, 2) == false)
    {
        return luaL_typerror(state, 2, "number");
    }
    character->mX = lua_tonumber(state, 2);

    if(lua_isnumber(state, 3) == false)
    {
        return 0;
    }
    character->mY = lua_tonumber(state, 3);

    return 0;
}

static const struct luaL_reg luaBinding [] =
{
    {"Create", lua_Character_Create},
    {"FaceMaxHeight", lua_Character_FaceMaxHeight},
    {"GetKern", lua_Character_GetKern},
    {"PixelWidth", lua_Character_PixelWidth},
    {"PixelHeight", lua_Character_PixelHeight},
    {"Render", lua_Character_Render},
    {"SetPosition", lua_Character_SetPosition},
    {"GetCorner", lua_Character_GetCorner},
    {NULL, NULL}  /* sentinel */
};

void Character::Bind(LuaState* state)
{
    state->Bind
    (
        Character::Meta.Name(),
        luaBinding
    );
}
