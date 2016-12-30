#include "GraphicsPipeline.h"

#include <assert.h>
#include <cmath>

#include "Dinodeck.h"   // Used to get the default font.
#include "Game.h" // Used to get system font, could be store statically in gp
#include "DinodeckGL.h"
#include "DDLog.h"
#include "Matrix.h"
#include "Sprite.h"
#include "Texture.h"
#include "FormatText.h"

// TEMP
#ifdef ANDROID
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif

const double PI = 3.141592;

const char* GraphicsPipeline::BlendStr[BLEND_COUNT] =
{
    "BLEND_BLEND",
    "BLEND_ADDITIVE",
};

void GraphicsPipeline::Flush()
{

    if(mVertCount == 0)
    {
        return; // Nothing to flush.
    }

    // Needs a test
    if(mTexture == NULL)
    {
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
    }

    glVertexPointer(POSITION_SIZE, GL_FLOAT, sizeof(Vertex), mVertexBuffer);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColorPointer(COLOUR_SIZE, GL_FLOAT, sizeof(Vertex), &mVertexBuffer[0].r);
    glEnableClientState(GL_COLOR_ARRAY);

    glTexCoordPointer(TEXCOORD_SIZE, GL_FLOAT, sizeof(Vertex), &mVertexBuffer[0].u);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //
    // Send off the draw commands
    //
    glPushMatrix();
    {
        glRotatef(mRotateAngle, 0.f, 0.f, 1.f);
        glTranslatef(mCamPosition.x, mCamPosition.y, mCamPosition.z);
        glScalef(mCamScale.x, mCamScale.y, mCamScale.z);


        glDrawArrays(mDrawMode, 0, mVertCount);
    }
    glPopMatrix();

    //
    // Disable the various pointers
    //
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);


    //
    // Reset the vert array
    //
    mVertCount = 0;
}

void GraphicsPipeline::PushCircle(float x,
                                  float y,
                                  float radius,
                                  float segments,
                                  const Vector& colour)
{

    unsigned int numVerts = (2 * segments) + 2;
    assert(numVerts < BUFFER_SIZE_IN_VERTS);
    bool needToFlush = mVertCount + numVerts >= BUFFER_SIZE_IN_VERTS;

    if(needToFlush || mDrawMode != LINES)
    {
        Flush();
        mDrawMode = LINES;
    }


    float prevXPos = 0;
    float prevYPos = 0;

    for (int i = 0; i < segments; i++)
    {
        float angle = i * ((2 * PI) / segments);
        float xpos = radius * cos(angle) + x;
        float ypos = radius * sin(angle) + y;

        if (i > 0)
        {
            mVertexBuffer[mVertCount] = Vertex(prevXPos, prevYPos, 0.f, colour);
            mVertCount++;
            mVertexBuffer[mVertCount] = Vertex(xpos, ypos, 0.f, colour);
            mVertCount++;
        }

        prevXPos = xpos;
        prevYPos = ypos;
    }

    // Join up last segment
    float angle = 0;
    float xpos = radius * cos(angle) + x;
    float ypos = radius * sin(angle) + y;

    mVertexBuffer[mVertCount] = Vertex(prevXPos, prevYPos, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(xpos, ypos, 0.f, colour);
    mVertCount++;

    // I don't think I should need this but I'm getting connections between
    // circles. Something to look into.
    Flush();
}

void GraphicsPipeline::PushRectangle(float bottom,
                                     float left,
                                     float top,
                                     float right,
                                     const Vector& colour)
{
    unsigned int numVerts = 6; // Two triangles
    assert(numVerts < BUFFER_SIZE_IN_VERTS);
    bool needToFlush = mVertCount + numVerts >= BUFFER_SIZE_IN_VERTS;

    if(needToFlush || mDrawMode != TRIANGLES || mTexture != NULL)
    {
        Flush();
        mDrawMode = TRIANGLES;
    }

    glDisable( GL_TEXTURE_2D );
    mTexture = NULL;

    /*
        1. TL
         |  \
         |   \
         |    \
        3. BL--2.BR
    */
    mVertexBuffer[mVertCount] = Vertex(left, top, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(right, bottom, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(left, bottom, 0.f, colour);
    mVertCount++;

    /* My ASCII art is terrible.
          1.TL----2. TR
              \    |
               \   |
                \  |
                3. BR
    */
    mVertexBuffer[mVertCount] = Vertex(left, top, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(right, top, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(right, bottom, 0.f, colour);
    mVertCount++;
}

void GraphicsPipeline::PushLine(float x1,
                                float y1,
                                float x2,
                                float y2,
                                const Vector& colour)
{
    unsigned int numVerts = 2;
    assert(numVerts < BUFFER_SIZE_IN_VERTS);
    bool needToFlush = mVertCount + numVerts >= BUFFER_SIZE_IN_VERTS;

    if(needToFlush || mDrawMode != LINES)
    {
        Flush();
        mDrawMode = LINES;
    }
    mVertexBuffer[mVertCount] = Vertex(x1, y1, 0.f, colour);
    mVertCount++;
    mVertexBuffer[mVertCount] = Vertex(x2, y2, 0.f, colour);
    mVertCount++;
    Flush();
}

void GraphicsPipeline::PushSprite(const Sprite* sprite)
{
    unsigned int numVerts = 6; // two tris of 3 verts
    assert(numVerts < BUFFER_SIZE_IN_VERTS);
    bool needToFlush = mVertCount + numVerts >= BUFFER_SIZE_IN_VERTS;


    if(mTexture != sprite->texture)
    {
        needToFlush = true;
    }

    if(needToFlush || mDrawMode != TRIANGLES)
    {
        Flush();
        mTexture = sprite->texture;
        mDrawMode = TRIANGLES;
        glEnable( GL_TEXTURE_2D );
        glBindTexture(GL_TEXTURE_2D, mTexture->GetId());
    }
    glEnable( GL_TEXTURE_2D );

    if(mTexture == NULL)
    {
        printf("Early out sprite has no texture.\n");
        return;
    }

    const Vector& colour = sprite->colour;

    Matrix modelMatrix;
    modelMatrix.SetRotation(Vector::AxisZ, sprite->rotation);
    modelMatrix.SetTranslation(sprite->position);

    // Apply the scale, maybe an applyscale function?
    modelMatrix.mCol0.x *= sprite->scale.x;
    modelMatrix.mCol1.y *= sprite->scale.y;
    modelMatrix.mCol2.z *= sprite->scale.z;


    float texScaleX = std::abs(sprite->topLeftU - sprite->bottomRightU);
    float texScaleY = std::abs(sprite->topLeftV - sprite->bottomRightV);
    float halfWidth =  ((mTexture->GetWidth()*texScaleX)/2);
    float halfHeight = ((mTexture->GetHeight()*texScaleY)/2);

    // TL
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 - halfWidth, 0 + halfHeight, 0.f, 1.f),
                                       colour,
                                       sprite->topLeftU, sprite->topLeftV);
    mVertCount++;


    // TR
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 + halfWidth, 0 + halfHeight, 0, 1.f),
                                       colour,
                                       sprite->bottomRightU, sprite->topLeftV);
    mVertCount++;

    // BL
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 - halfWidth, 0 - halfHeight, 0, 1),
                                       colour,
                                       sprite->topLeftU, sprite->bottomRightV);
    mVertCount++;

    // TR
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 + halfWidth, 0 + halfHeight, 0, 1),
                                       colour,
                                       sprite->bottomRightU, sprite->topLeftV);
    mVertCount++;

    // BR
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 + halfWidth, 0 - halfHeight, 0, 1),
                                       colour,
                                       sprite->bottomRightU, sprite->bottomRightV);
    mVertCount++;

    // BL
    mVertexBuffer[mVertCount] = Vertex(modelMatrix*Vector(0 - halfWidth, 0 - halfHeight, 0, 1),
                                       colour,
                                       sprite->topLeftU, sprite->bottomRightV);
    mVertCount++;
}




void GraphicsPipeline::PushText(float x,
                                float y,
                                const char* text,
                                const Vector& colour,
                                int width)
{
    if(!mFont)
    {
        SetFont(mFontName.c_str());
    }

    if(!mFont || mFontScaleX == 0 || mFontScaleY == 0)
    {
        return;
    }

    Flush();    // No text batching at the moment!
                // Can probably be fixed by looking into FTGL's internals
    mTexture = NULL;
    glPushMatrix();
    {

        glRotatef(mRotateAngle, 0.f, 0.f, 1.f);
        glTranslatef(mCamPosition.x, mCamPosition.y, mCamPosition.z);
        glScalef(mCamScale.x, mCamScale.y, mCamScale.z);
        glPushMatrix();
        {
            //glTranslatef(x, y, 0);
            glScalef(mFontScaleX, mFontScaleY, 1);
            glRotatef(mTextRotation, 0, 0, 1);
            glColor4f(colour.x, colour.y, colour.z, colour.w);

            if(width < 1)
            {
                FormatText::PushText(mFont, x/mFontScaleX, y/mFontScaleY, text, colour, mAlignX, mAlignY);
            }
            else
            {
                FormatText::PushTextWrapped(mFont, x/mFontScaleX, y/mFontScaleY, text, colour, mAlignX, mAlignY, width/mFontScaleX);
            }
        }
        glPopMatrix();
    }
    glPopMatrix();
}


void GraphicsPipeline::MeasureText(const char* text, float width, Vector* outSize) const
{
    // Not quite sure about the matrix stuff
    glPushMatrix();
    {
        glScalef(mCamScale.x, mCamScale.y, mCamScale.z);
        glPushMatrix();
        {
            //glTranslatef(x, y, 0);
            glScalef(mFontScaleX, mFontScaleY, 1);
            FormatText::MeasureText(mFont, text, width/mFontScaleX, outSize);
            outSize->x *=  mFontScaleX;
            outSize->y *=  mFontScaleY;
        }
        glPopMatrix();
    }
    glPopMatrix();
}

void GraphicsPipeline::SetTextAlignX(AlignX::Enum align)
{
    mAlignX = align;
}

void GraphicsPipeline::SetTextAlignY(AlignY::Enum align)
{
    mAlignY = align;
}

bool GraphicsPipeline::SetFont(const char* name)
{
    mFontName = name;
    bool success = true;
    Dinodeck* dinodeck = Dinodeck::GetInstance();
    FTTextureFont* font = dinodeck->GetGame()->GetFont(name);
    if(NULL == font)
    {
        font = dinodeck->GetGame()->GetSystemFont();
        assert(font); // This font should ALWAYs be available, it's compiled into the binary.
        success = false;
    }
    SetFont(font);
    return success;
}

void GraphicsPipeline::SetBlend(eBlendMode blend)
{
    if(mBlendMode == blend)
    {
        return;
    }

    Flush();

    if(blend == BLEND)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        mBlendMode = blend;
    }
    else if(blend == ADDITIVE)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // I think? :D
        mBlendMode = blend;
    }


}

void GraphicsPipeline::Reset()
{
    // I'd prefer not to have this but I can't think of a nice way
    // to avoid it at the moment
    mAlignX = AlignX::Left;
    mAlignY = AlignY::Top;
    mFontScaleX = 0.25;
    mFontScaleY = 0.25;
    mTextRotation = 0;
    mCamPosition.SetXyzw(0, 0, 0, 0);

    Dinodeck* dinodeck = Dinodeck::GetInstance();
    FTTextureFont* font = dinodeck->GetDefaultFont();
    if(NULL == font)
    {
        font = dinodeck->GetGame()->GetSystemFont();
        assert(font); // This font should ALWAYs be available, it's compiled into the binary.
    }
    SetFont(font);

    // Clear scissor
    glDisable(GL_SCISSOR_TEST);
    mScissorRefCount = 0;
}

void GraphicsPipeline::PushScissor(int x, int y, int width, int height)
{
    Flush();
    mScissorRefCount++;
    if(mScissorRefCount == 1)
    {
        glEnable(GL_SCISSOR_TEST);
    }
    glScissor(x, y, width, height);
}

void GraphicsPipeline::PopScissor()
{
    Flush();
    mScissorRefCount--;
    assert(mScissorRefCount >= 0);
    if(mScissorRefCount == 0)
    {
        glDisable(GL_SCISSOR_TEST);
    }
}