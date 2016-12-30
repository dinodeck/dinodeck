#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include "DinodeckGL.h"
#include "Vector.h"
#include "DDTextAlign.h"
#include "Vertex.h"

class Sprite;
class Texture;
class FTTextureFont;



enum eDrawMode
{
    TRIANGLES = GL_TRIANGLES,
    LINES = GL_LINE_STRIP,
};

enum eBlendMode
{
    BLEND,
    ADDITIVE,
    BLEND_COUNT
};

class GraphicsPipeline
{
    static const unsigned int BUFFER_SIZE_IN_VERTS = 1024;
    static const unsigned int POSITION_SIZE = 3; // no w
    static const unsigned int COLOUR_SIZE = 4;
    static const unsigned int TEXCOORD_SIZE = 2;

    static const char* AlignXStr[AlignX::Count];
    static const char* AlignYStr[AlignY::Count];

    eDrawMode mDrawMode;
    Vertex mVertexBuffer[BUFFER_SIZE_IN_VERTS];
    unsigned int mVertCount;
    Texture* mTexture;
    FTTextureFont* mFont;
    double mFontScaleX;
    double mFontScaleY;
    double mTextRotation;
    AlignX::Enum mAlignX;
    AlignY::Enum mAlignY;
    Vector mCamPosition;
    Vector mCamScale;
    float mRotateAngle;
    eBlendMode mBlendMode;
    int mScissorRefCount;
    std::string mFontName;
public:
    static const char* BlendStr[BLEND_COUNT];

    GraphicsPipeline()
        : mDrawMode(TRIANGLES),
          mVertCount(0),
          mTexture(NULL),
          mFont(NULL),
          mFontScaleX(0.25),
          mFontScaleY(0.25),
          mTextRotation(0),
          mAlignX(AlignX::Left),
          mAlignY(AlignY::Top),
          mCamPosition(),
          mCamScale(1,1,1,1),
          mRotateAngle(0),
          mBlendMode(BLEND),
          mScissorRefCount(0)
           { Reset(); }


    double GetFontScaleX() const { return mFontScaleX; }
    double GetFontScaleY() const { return mFontScaleY; }
    void SetFontScale(double x, double y) { mFontScaleX = x; mFontScaleY = y; }
    void SetTextRotation(double value) { mTextRotation = value; }
    double GetTextRotation() const { return mTextRotation; }
    void SetFont(FTTextureFont* font) { mFont = font; }
    FTTextureFont* GetFont() { return mFont; }
    void MeasureText(const char* text, float width, Vector* outSize) const;
    void SetTextAlignX(AlignX::Enum align);
    void SetTextAlignY(AlignY::Enum align);
    const Vector& CameraPosition() { return mCamPosition; }
    void SetCameraPosition(const Vector& pos) { Flush(); mCamPosition.SetXyzw(pos); }
    void SetCameraPosition(float x, float y) { Flush(); mCamPosition.SetXyzw(x, y, 0, 0); }
    const Vector& CameraScale() { return mCamScale; }
    void SetCameraScale(const Vector& pos) { mCamScale.SetXyzw(pos); }
    void SetCameraScale(float sx, float sy) { mCamScale.SetXyzw(sx, sy, 1, 0); }
    float CameraRotation() const { return mRotateAngle; }
    void SetCameraRotation(float value) { mRotateAngle = value; }
    void Reset(); // This resets some of the font state info.
    void OnNewFrame() { mTexture = NULL; }
    bool SetFont(const char* name);
    void ClearCachedFont() { mFont = NULL; }

    void SetBlend(eBlendMode blend);

    void Flush();
    void PushCircle(float x,
                    float y,
                    float radius,
                    float segments,
                    const Vector& colour);
    void PushRectangle(float bottom,
                       float left,
                       float top,
                       float right,
                       const Vector& colour);
    void PushLine(float x1,
                  float y1,
                  float x2,
                  float y2,
                  const Vector& colour);

    void PushSprite(const Sprite* sprite);

    void PushText(float x,
                  float y,
                  const char* text,
                  const Vector& colour,
                  int width);

    void PushScissor(int x, int y, int width, int height);
    void PopScissor();
};

#endif