#include "Renderer.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <string>
//#include <FTGL/ftgl.h>

#include "DinodeckGL.h"
#include "DDLog.h"
#include "FormatText.h"
#include "Game.h"
#include "GraphicsPipeline.h"
#include "LuaState.h"
#include "Sprite.h"
#include "Texture.h"
#include "Vector"

std::vector<Renderer*> Renderer::mRenderers;
const float Renderer::TextScale = 4.0;


Reflect Renderer::Meta("Renderer", Renderer::Bind);

const char* Renderer::AlignXStr[AlignX::Count] =
{
    "left",
    "right",
    "center"
};


const char* Renderer::AlignYStr[AlignY::Count] =
{
    "top",
    "center",
    "bottom"
};


static int lua_Create(lua_State* state)
{
    // Instance new lets the constructor be called on a block of data
    Renderer * r = new (lua_newuserdata(state, sizeof(Renderer))) Renderer();

    // Should this really be static?
    Renderer::mRenderers.push_back(r);
    luaL_getmetatable(state, "Renderer");
    lua_setmetatable(state, -2);
    return 1;
}


static int lua_DrawSprite(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL)
    {
        return luaL_typerror(state, 1, "Renderer");
    }
    Sprite* sprite = (Sprite*)lua_touserdata(state, 2);
    if (sprite == NULL)
    {
        return luaL_typerror(state, 2, "Sprite");
    }
    renderer->DrawSprite(*sprite);
    return 0;
}


static int lua_DrawText2d(lua_State* state)
{
    static Vector DefaultColor;
    static Vector DefaultPosition;
    DefaultColor.SetXyzw(1,1,1,1);

    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL or !luaL_checkudata(state, 1, "Renderer"))
    {
        return luaL_typerror(state, 1, "Renderer");
    }
    Vector* position = &DefaultPosition;
    int paramIndex = 3;
    if (lua_isuserdata(state, 2) && LuaState::IsType(state, 2, "Vector"))
    {
        position = (Vector*)lua_touserdata(state, 2);
    }
    else if(lua_isnumber(state, 2) && lua_isnumber(state, 3))
    {
        DefaultPosition.SetXyzw(lua_tonumber(state, 2), lua_tonumber(state, 3), 0, 0);
        paramIndex = 4;
    }
    else
    {
        return luaL_typerror(state, 2, "Vector");
    }

    if(!lua_isstring(state, paramIndex))
    {
        return luaL_typerror(state, paramIndex, "string");
    }
    const char* text = lua_tostring(state, paramIndex);

    paramIndex++;
    Vector* color = &DefaultColor;
    if(lua_isuserdata(state, paramIndex) && luaL_checkudata(state, paramIndex, "Vector"))
    {
        color = (Vector*)lua_touserdata(state, paramIndex);
    }

    paramIndex++;
    float width = (float) luaL_optnumber(state, paramIndex, -1);

    GraphicsPipeline* gp = renderer->Graphics();
    gp->PushText
    (
        position->x,
        position->y,
        text,
        (*color),
        width
    );
    return 0;
}


static int lua_DrawCircle2d(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL or !luaL_checkudata(state, 1, "Renderer"))
    {
        return luaL_typerror(state, 1, "Renderer");
    }
    static Vector RGBA;
    RGBA.SetXyzw(1,1,1,1);
    int defaultSegments = 16;

    if(lua_isnumber(state, 2))
    {
        double x        = luaL_checknumber(state, 2);
        double y        = luaL_checknumber(state, 3);
        double radius   = luaL_checknumber(state, 4);
        int segments    = luaL_optnumber(state, 5, defaultSegments);

        Vector* color = &RGBA;

        if (lua_isuserdata(state, 6) and luaL_checkudata (state, 6, "Vector"))
        {
            color = LuaState::GetFuncParam<Vector>(state, 6);
        }
        renderer->DrawCircle2d(x, y, radius, segments, (*color));
    }
    else if(LuaState::IsType(state, 2, "Vector"))
    {
        Vector* vector = LuaState::GetFuncParam<Vector>(state, 2);

        if(NULL == vector)
        {
            return luaL_typerror(state, 2, "number or Vector");
        }
        double radius   = luaL_checknumber(state, 3);
        int segments    = luaL_optnumber(state, 4, defaultSegments);

        Vector* color = &RGBA;

        if (lua_isuserdata(state, 5) and luaL_checkudata (state, 5, "Vector"))
        {
            color = LuaState::GetFuncParam<Vector>(state, 5);
        }
        renderer->DrawCircle2d(vector->x, vector->y, radius, segments, (*color));
    }
    else
    {
        return luaL_typerror(state, 2, "number or Vector");
    }

    return 0;
}


static int lua_DrawRect2d(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    if (renderer == NULL or !luaL_checkudata (state, 1, "Renderer"))
    {
        return luaL_typerror(state, 1, "Renderer");
    }

    static Vector DefaultBottomLeft;
    static Vector DefaultTopRight;
    Vector* bottomLeftVector = &DefaultBottomLeft;
    Vector* topRightVector = &DefaultTopRight;

    // Going to ignore texture

    int colorPosition = 4;
    if (lua_isuserdata(state, 2) and luaL_checkudata (state, 2, "Vector"))
    {
        bottomLeftVector = (Vector*)lua_touserdata(state, 2);
        topRightVector = (Vector*)lua_touserdata(state, 3);
        if (topRightVector == NULL or !luaL_checkudata (state, 3, "Vector"))
        {
            return luaL_typerror(state, 3, "Vector");
        }
    }
    else
    {
        // Numbers
        double left = luaL_checknumber(state, 2);
        double bottom = luaL_checknumber(state, 3);
        double right = luaL_checknumber(state, 4);
        double top = luaL_checknumber(state, 5);
        colorPosition = 6;
        DefaultBottomLeft.SetXyzw(left, bottom, 0, 0);
        DefaultTopRight.SetXyzw(right, top, 0, 0);
    }

    Vector defaultColour;
    Vector* colour = (Vector*) lua_touserdata(state, colorPosition);
    if(colour == NULL)
    {
         if(!luaL_checkudata (state, colorPosition, "Vector"))
         {
            return luaL_typerror(state, colorPosition, "Vector");
         }
         colour = &defaultColour;
    }

    renderer->DrawRect2d(*bottomLeftVector, *topRightVector, *colour);
    return 0;
}


int lua_gc(lua_State* state)
{
    Renderer* renderer = (Renderer*)lua_touserdata(state, 1);
    assert(renderer);

    // Iterate through all renders
    // This could be sped up but it's probably not going to be a bottleneck?
    for(std::vector<Renderer*>::reverse_iterator riter = Renderer::mRenderers.rbegin(); riter < Renderer::mRenderers.rend(); ++riter )
    {
        if(renderer == (*riter))
        {
            Renderer::mRenderers.erase((riter+1).base()); // oh stl
            return 0;
        }
    }
    assert(!"ERROR: Failed to find renderer pointer to clear up. Something has gone wrong.\n");
    // I thought I might need this
    // if renderers need calling from the minigame
    // like they queue up all their render calls
    // and run then at the end of each frame
    // But perhaps not right now.
    return 0;
}


static int lua_ScaleText(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    double scaleX = luaL_optnumber(state, 2,
        renderer->Graphics()->GetFontScaleX()*Renderer::TextScale);
    double scaleY = luaL_optnumber(state, 2,
        renderer->Graphics()->GetFontScaleY()*Renderer::TextScale);

    renderer->Graphics()->SetFontScale
    (
        scaleX / Renderer::TextScale,
        scaleY / Renderer::TextScale
    );
    return 0;
}


static int lua_ScaleTextX(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    double scaleX = luaL_optnumber(state, 2,
        renderer->Graphics()->GetFontScaleX() * Renderer::TextScale);
    renderer->Graphics()->SetFontScale
    (
        scaleX / Renderer::TextScale,
        renderer->Graphics()->GetFontScaleY()
    );
    return 0;
}


static int lua_ScaleTextY(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    double scaleY = luaL_optnumber(state, 2,
        renderer->Graphics()->GetFontScaleY() * Renderer::TextScale);

    renderer->Graphics()->SetFontScale
    (
        renderer->Graphics()->GetFontScaleX(),
        scaleY / Renderer::TextScale
    );
    return 0;
}


static int lua_AlignText(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    // Horizontal
    if(lua_isstring(state, 2))
    {
        const char* alignStr = lua_tostring(state, 2);
        std::string align = std::string(alignStr);
        for(int i = 0; i < AlignX::Count; i++)
        {
            if(align == Renderer::AlignXStr[i])
            {
                renderer->Graphics()->SetTextAlignX((AlignX::Enum)i);
            }
        }
    }

    // Vertical
    if(lua_isstring(state, 3))
    {
        const char* alignStr = lua_tostring(state, 3);
        std::string align = std::string(alignStr);
        for(int i = 0; i < AlignY::Count; i++)
        {
            if(align == Renderer::AlignYStr[i])
            {
                renderer->Graphics()->SetTextAlignY((AlignY::Enum)i);
            }
        }
    }

    return 0;
}


static int lua_AlignTextX(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    const char* alignStr = LuaState::GetParam(state, 2);
    if(alignStr == NULL)
    {
        return 0;
    }

    std::string align = std::string(alignStr);
    for(int i = 0; i < AlignX::Count; i++)
    {
        if(align == Renderer::AlignXStr[i])
        {
            renderer->Graphics()->SetTextAlignX((AlignX::Enum)i);
        }
    }
    return 0;
}


static int lua_AlignTextY(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    const char* alignStr = LuaState::GetParam(state, 2);
    if(alignStr == NULL)
    {
        return 0;
    }

    std::string align = std::string(alignStr);
    for(int i = 0; i < AlignY::Count; i++)
    {
        if(align == Renderer::AlignYStr[i])
        {
            renderer->Graphics()->SetTextAlignY((AlignY::Enum)i);
        }
    }
    return 0;
}

static int lua_NextLine(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    GraphicsPipeline* gp = renderer->Graphics();

    const char* text = luaL_checkstring(state, 2);
    if(text == NULL)
    {
        return 0;
    }

    // Cursor, and ensure cursor isn't greater than #text
    if(!lua_isnumber(state, 3))
    {
        luaL_typerror(state, 3, "number");
    }
    int cursor = (int)lua_tonumber(state, 3);
    cursor--; // Lua indexes from 1, C from 0
    if(cursor < 0 || (int)std::string(text).length() <= cursor)
    {
        luaL_error(state, "Invalid cursor value. Should be between 0 and %d",
                   std::string(text).length());
    }



    // Max width
    if(!lua_isnumber(state, 4))
    {
        luaL_typerror(state, 4, "number");
    }
    float maxwidth = lua_tonumber(state, 4);

    //dsprintf("NextLine(\"%s\", %d, %f)\n", text, cursor, maxwidth);

    // Rest are outs
    FTTextureFont* font = gp->GetFont();

    int outStart = 0;
    int outFinish = 0;
    float outPixelWidth = 0;

    FormatText::NextLine
    (
        font,
        text,
        cursor,
        maxwidth / gp->GetFontScaleX(),
        &outStart,
        &outFinish,
        &outPixelWidth
    );

    lua_pushnumber(state, outStart + 1);
    lua_pushnumber(state, outFinish + 1);

    return 2;
}

static int lua_MeasureText(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    GraphicsPipeline* gp = renderer->Graphics();
    assert(gp != NULL);

    const char* text = lua_tostring(state, 2);
    if(text == NULL)
    {
        return 0;
    }

    Vector* vector = NULL;
    int returnValues = 0;
    int widthParam = 3;
    if (lua_isuserdata(state, 3) && LuaState::IsType(state, 3, "Vector"))
    {
        vector = (Vector*)lua_touserdata(state, 3);
        widthParam = 4;
    }
    else
    {
        // No vector passed in, so create one.
        vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
        luaL_getmetatable(state, "Vector");
        lua_setmetatable(state, -2);
        returnValues = 1;
    }

    // Optional width argument.
    float width = -1;
    if(lua_isnumber(state, widthParam))
    {
        width = lua_tonumber(state, widthParam);
    }

    assert(vector != NULL);

    gp->MeasureText(text, width, vector);
    return returnValues;
}


static int lua_SetTextRotation(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    GraphicsPipeline* gp = renderer->Graphics();
    if(!lua_isnumber(state, 2))
    {
        luaL_typerror(state, 2, "number");
    }
    double rot = lua_tonumber(state, 2);
    gp->SetTextRotation(rot);
    return 0;
}


static int lua_GetTextRotation(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    GraphicsPipeline* gp = renderer->Graphics();
    lua_pushnumber(state, gp->GetTextRotation());
    return 1;
}


static int lua_DrawLine2d(lua_State* state)
{
    static Vector DefaultStart;
    static Vector DefaultEnd;
    static Vector DefaultColor;
    DefaultColor.SetXyzw(1, 1, 1, 1);
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    int colourParam = 6;
    Vector* start = &DefaultStart;
    Vector* end = &DefaultEnd;
    if(lua_isnumber(state, 2))
    {
        double x1 = luaL_checknumber(state, 2);
        double y1 = luaL_checknumber(state, 3);
        double x2 = luaL_checknumber(state, 4);
        double y2 = luaL_checknumber(state, 5);
        DefaultStart.SetX(x1);
        DefaultStart.SetY(y1);
        DefaultEnd.SetX(x2);
        DefaultEnd.SetY(y2);
        colourParam = 6;
    }
    else if (lua_isuserdata(state, 2) and luaL_checkudata (state, 2, "Vector"))
    {
        start = LuaState::GetFuncParam<Vector>(state, 2);
        if(start == NULL)
        {
            return 0;
        }
        end = LuaState::GetFuncParam<Vector>(state, 3);
        if(end == NULL)
        {
            return 0;
        }
        colourParam = 4;
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or number");
    }

    Vector* color = &DefaultColor;
    if(lua_isuserdata(state, colourParam) and luaL_checkudata (state, colourParam, "Vector"))
    {
        color = LuaState::GetFuncParam<Vector>(state, colourParam);
    }

    renderer->DrawLine2d(*start, *end, *color);

    return 0;
}


static int lua_Translate(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    if(lua_isuserdata(state, 2) and luaL_checkudata (state, 2, "Vector"))
    {
        Vector* position = LuaState::GetFuncParam<Vector>(state, 2);
        renderer->Graphics()->SetCameraPosition(*position);
    }
    else if(lua_isnumber(state, 2))
    {
        const Vector& pos = renderer->Graphics()->CameraPosition();
        float transX = lua_tonumber(state, 2);
        float transY = luaL_optnumber(state, 3, pos.y);
        renderer->Graphics()->SetCameraPosition(transX, transY);
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or number");
    }
    return 0;
}


static int lua_GetTranslate(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    const Vector& pos = renderer->Graphics()->CameraPosition();
    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    assert(vector != NULL);
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    vector->SetXyzw(pos);
    return 1;
}


static int lua_SetFont(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    if(!lua_isstring(state,  2))
    {
        return luaL_typerror(state, 2, "string");
    }

    const char* fontname = lua_tostring(state, 2);

    if(!renderer->Graphics()->SetFont(fontname))
    {
        return luaL_error(state, "Font: \"%s\" does not exist.", fontname);
    }

    return 0;
}


static int lua_SetBlend(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    if(!lua_isnumber(state,  2))
    {
        return luaL_typerror(state, 2, "number");
    }

    int blendNum = lua_tonumber(state, 2);

    if(blendNum < 0 or blendNum >= BLEND_COUNT)
    {
        return luaL_error(state, "Invalid blend mode.");
    }

    eBlendMode blendMode = (eBlendMode) blendNum;
    renderer->Graphics()->SetBlend(blendMode);
    return 0;
}


static int lua_Scale(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    if(lua_isuserdata(state, 2) and luaL_checkudata (state, 2, "Vector"))
    {
        Vector* position = LuaState::GetFuncParam<Vector>(state, 2);
        renderer->Graphics()->SetCameraScale(*position);
    }
    else if(lua_isnumber(state, 2))
    {
        const Vector& scale = renderer->Graphics()->CameraScale();
         float scaleX = lua_tonumber(state, 2);
         float scaleY = luaL_optnumber(state, 3, scale.y);
         renderer->Graphics()->SetCameraScale(scaleX, scaleY);
    }
    else
    {
        return luaL_typerror(state, 2, "Vector or number");
    }
    return 0;
}


static int lua_GetScale(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    const Vector& pos = renderer->Graphics()->CameraScale();
    Vector* vector = new (lua_newuserdata(state, sizeof(Vector))) Vector();
    assert(vector != NULL);
    luaL_getmetatable(state, "Vector");
    lua_setmetatable(state, -2);
    vector->SetXyzw(pos);

    return 1;
}
static int lua_Rotate(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }
    float rot = luaL_optnumber(state, 2, renderer->Graphics()->CameraRotation());
    renderer->Graphics()->SetCameraRotation(rot);
    return 0;
}


static int lua_GetRotate(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    float rot = renderer->Graphics()->CameraRotation();
    lua_pushnumber(state, rot);
    return 1;
}

static int lua_GetKern(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    const char* currentStr = luaL_optstring(state, 2, "\0");
    const char* nextStr = luaL_optstring(state, 3, "\0");

    if(std::string(currentStr) == "" or std::string(nextStr) == "")
    {
        return 0;
    }

    float kern = FormatText::GetKern(renderer->Graphics()->GetFont(),
    (int) currentStr[0],
    (int) nextStr[0]);
    lua_pushnumber(state, kern);
    return 1;
}

static int lua_Clip(lua_State* state)
{
    Renderer* renderer = LuaState::GetFuncParam<Renderer>(state, 1);
    if(renderer == NULL)
    {
        return 0;
    }

    // Expecting an x, y, w, h
    if(!lua_isnumber(state,  2)) { return luaL_typerror(state, 2, "number"); }
    if(!lua_isnumber(state,  3)) { return luaL_typerror(state, 3, "number"); }
    if(!lua_isnumber(state,  4)) { return luaL_typerror(state, 4, "number"); }
    if(!lua_isnumber(state,  5)) { return luaL_typerror(state, 5, "number"); }

    int x = (int)lua_tonumber(state, 2);
    int y = (int)lua_tonumber(state, 3);
    int w = std::max(0, (int)lua_tonumber(state, 4));
    int h = std::max(0, (int)lua_tonumber(state, 5));

    if(!lua_isfunction(state, 6))
    {
        return luaL_typerror(state, 6, "function");
    }

    renderer->Graphics()->PushScissor(x, y, w, h);
    {
        // Call the function, not sure what happens if it errors.
        lua_call (state, 0, 0);
    }
    renderer->Graphics()->PopScissor();

    return 0;
}

static const struct luaL_reg luaBinding [] =
{
    {"__gc", lua_gc},
    {"AlignText", lua_AlignText},
    {"AlignTextX", lua_AlignTextX},
    {"AlignTextY", lua_AlignTextY},
    {"Translate", lua_Translate},
    {"GetTranslate", lua_GetTranslate},
    {"Scale", lua_Scale},
    {"GetScale", lua_GetScale},
    {"Rotate", lua_Rotate},
    {"GetRotate", lua_GetRotate},
    {"Create", lua_Create},
    {"DrawCircle2d", lua_DrawCircle2d},
    {"DrawLine2d", lua_DrawLine2d},
    {"DrawRect2d", lua_DrawRect2d},
    {"DrawSprite", lua_DrawSprite},
    {"DrawText2d", lua_DrawText2d},
    {"GetTextRotation", lua_GetTextRotation},
    {"MeasureText", lua_MeasureText},
    {"NextLine", lua_NextLine},
    {"RotateText", lua_SetTextRotation},
    {"ScaleText", lua_ScaleText},
    {"ScaleTextX", lua_ScaleTextX},
    {"ScaleTextY", lua_ScaleTextY},
    {"SetFont", lua_SetFont},
    {"SetBlend", lua_SetBlend},
    {"GetKern", lua_GetKern},
    {"Clip", lua_Clip},
    {NULL, NULL}  /* sentinel */
};


void Renderer::Bind(LuaState* state)
{
    state->Bind
    (
        Renderer::Meta.Name(),
        luaBinding
    );

    for(int i = 0; i < BLEND_COUNT; i++)
    {
        lua_pushnumber(state->State(), i);
        lua_setglobal(state->State(), GraphicsPipeline::BlendStr[i]);
    }
}


Renderer::Renderer()
{
    mGraphics = new GraphicsPipeline();
}


Renderer::~Renderer()
{
    if(mGraphics)
    {
        delete mGraphics;
    }
}


void Renderer::DrawCircle2d(double x, double y, double radius, int segments,
                          const Vector& rgba)
{
    mGraphics->PushCircle(x, y, radius, segments, rgba);
}


void Renderer::DrawRect2d(const Vector& bottomLeft, const Vector& topRight,
                          const Vector& colour)
{
    mGraphics->PushRectangle
    (
        bottomLeft.y,   // bottom
        bottomLeft.x,     // left
        topRight.y,     // top
        topRight.x,   // right
        colour
    );
}


void Renderer::DrawLine2d(const Vector& start, const Vector& end,
                          const Vector& colour)
{
    mGraphics->PushLine
    (
        start.x,
        start.y,
        end.x,
        end.y,
        colour
    );
}


void Renderer::DrawSprite(const Sprite& sprite)
{
    mGraphics->PushSprite
    (
        &sprite
    );
}
