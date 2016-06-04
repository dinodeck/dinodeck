#include "FormatText.h"
#include <assert.h>
#include <cmath>

#include "DinodeckGL.h"
#include "Vector.h"
#include "DDLog.h"


// TEMP
#ifdef ANDROID
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif

// Additional requirements for manually handling wrapping.
#include "FTGL/../FTGlyph/FTTextureGlyphImpl.h"
#include "FTGL/../FTFont/FTTextureFontImpl.h"
#include "FTGL/../FTGlyphContainer.h"
#include "FTGL/../FTCharmap.h"
#include "FTGL/../FTVector.h"


float FormatText::GetKern(FTTextureFont* font, int current, int next)
{
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();

    // CheckGlyph also loads the data into a cache
    // so it needs to be called here.
    if(!impl->CheckGlyph(current) || !impl->CheckGlyph(next))
    {
        return 0;
    }

    return glyphList->Advance(current, next);
}

float FormatText::CharPixelWidth(FTTextureFont* font, int c)
{
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();
    unsigned int charCode = (unsigned int) c;

    // This isn't just a check! It's lazy loader
    if(!impl->CheckGlyph(charCode))
    {
        return 0;
    }

    unsigned int index = charMap->GlyphListIndex(charCode);
    FTTextureGlyphImpl* glyph = (FTTextureGlyphImpl*) (*glyphVector)[index]->GetImpl();

    return glyph->GetWidth();
}

float FormatText::GetFaceMaxHeight(FTTextureFont* font)
{
    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    return impl->GetMaxHeight();
}


float FormatText::MeasureTextWidth(FTTextureFont* font, const char* text)
{
    float width = 0;
    int prevC = -1;
    for (int i = 0; text[i] != '\0'; i++)
    {
        int c = text[i];
        if(prevC != -1)
        {
            float kern = FormatText::GetKern(font, prevC, c);
            width += kern;
        }

        if(text[i + 1] == '\0')
        {
            width += FormatText::CharPixelWidth(font, c);

            // Punctuation like '!' at the end of a line isn't contributing
            // to the width. So this is a fix for that.
            // A correct fix would mean looking into glyph->GetWidth
            // Better might be add kern iff c width == 0
            static const int alphaNumericBoundary = 48;
            if(c < alphaNumericBoundary)
            {
                float kern = FormatText::GetKern(font, c, '\0');
                width += kern / 2;
            }
        }
        prevC = c;
    }
    return width;//     return width
}

void FormatText::RenderLine(FTTextureFont* font,
                float x, float y,
                const char* text,
                int start, int finish)
{
    assert((start == 0 && finish == 0) || start < finish);

    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

#ifndef ANDROID
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    FTTextureGlyphImpl::ResetActiveTexture();
#endif

#ifdef ANDROID
    impl->PreRender();
#endif

    int prevC = -1;
    for(int i = start; i < finish; i++)
    {
        int c = text[i];

        if(prevC != -1)
        {
            x += GetKern(font, prevC, c);
        }

        { // Actually do the render.
            unsigned int charCode = (unsigned int) c;

            // This isn't just a check! It's lazy loader
            if(!impl->CheckGlyph(charCode))
            {
                return;
            }

            unsigned int index = charMap->GlyphListIndex(charCode);

            //FTTextureGlyphImpl::ResetActiveTexture();
            (*glyphVector)[index]->Render(FTPoint(x, y), FTGL::RENDER_ALL);
        }

        prevC = c;
    }

#ifndef ANDROID
    glPopAttrib();
#endif

#ifdef ANDROID
    impl->PostRender();
#endif

    return;
}

int FormatText::CountLines
(
    FTTextureFont* font,
    const char* text,
    float maxwidth,
    float* outMaxLineWidth
)
{
    assert(outMaxLineWidth);
    int lineCount = 0;
    int lineEnd = 0;
    (*outMaxLineWidth) = -1;
    do
    {
        int outStart = 0;
        float outPixelWidth = 0;
        NextLine(font, text, lineEnd, maxwidth,
                    &outStart,
                    &lineEnd,
                    &outPixelWidth);

        (*outMaxLineWidth) = std::max((*outMaxLineWidth), outPixelWidth);

        lineCount++;
    } while (text[lineEnd] != '\0');
    return lineCount;
}

void FormatText::PushTextWrapped(
        FTTextureFont* font,
        float x,
        float y,
        const char* text,
        const Vector& colour,
        AlignX::Enum alignX,
        AlignY::Enum alignY,
        float maxwidth)
{
    float ignore = -1;
    float yOffset = 0;
    if(alignY == AlignY::Bottom)
    {
        float lines = (float) CountLines(font, text, maxwidth, &ignore)  - 1;
        yOffset = ((float)lines) * FormatText::GetFaceMaxHeight(font);
    }
    else if(alignY == AlignY::Center)
    {
        float lines = (float) CountLines(font, text, maxwidth, &ignore)  - 1;
        lines *= 0.5;
        float lineHeight = FormatText::GetFaceMaxHeight(font);
        yOffset = (lines *  lineHeight) - lineHeight*0.25;
    }
    else
    {
        yOffset = -GetFaceMaxHeight(font) * 0.75;
    }

    int lineEnd = 0;
    do
    {
        int outStart = 0;
        float outPixelWidth = 0;
        //dsprintf("\tNextline cursor[%d] char[%c]\n", lineEnd, text[lineEnd]);
        NextLine(font, text, lineEnd, maxwidth,
                    &outStart,
                    &lineEnd,
                    &outPixelWidth);
        //dsprintf("Output: outStart[%d] outFinish[%d], outPixelWidth[%f]\n",
        //     outStart, lineEnd, outPixelWidth);
        float xPos = x;
        if(alignX == AlignX::Right)
        {
            xPos -= outPixelWidth;
        }
        else if(alignX == AlignX::Center)
        {
            xPos -= outPixelWidth / 2;
        }

        RenderLine(font, xPos, y + yOffset, text, outStart, lineEnd);
        y = y - GetFaceMaxHeight(font);
    } while (text[lineEnd] != '\0');

    return;
}


void FormatText::PushText(FTTextureFont* font,
                        float x,
                        float y,
                        const char* text,
                        const Vector& colour,
                        AlignX::Enum alignX,
                        AlignY::Enum alignY)
{
    // Apply X alignment local x = self:ApplyAlignX(x, text)
    {

        if(alignX == AlignX::Right) //     if self.mAlignX == "right" then
        {
            x -= MeasureTextWidth(font, text);//local width = self:MeasureText(text)
        }
        else if(alignX == AlignX::Center) //     elseif self.mAlignX == "center" then
        {
            x -= MeasureTextWidth(font, text) / 2;
        }
    }

    // local y = self:ApplyAlignY(y)
    // The numbers are "ratio-ed" down because the height is from the bottom
    // of the glyph to the top, regardless of where the baseline. So this is
    // simple heurisitc that guesses the height of the face above the base line
    // only.
    {
        if(alignY == AlignY::Top)
        {
            float maxHeight = FormatText::GetFaceMaxHeight(font) * 0.75;
            y -= maxHeight;
        }
        else if(alignY == AlignY::Center)
        {
            float halfMaxHeight = FormatText::GetFaceMaxHeight(font) * 0.25;
            y -= halfMaxHeight;
        }
    }


    FTTextureFontImpl* impl = (FTTextureFontImpl*) font->GetImpl();
    FTGlyphContainer* glyphList = impl->GetGlyphList();
    FTCharmap* charMap = glyphList->GetCharmap();
    FTVector<FTGlyph*>* glyphVector = glyphList->GetGlyphVector();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

#ifndef ANDROID
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    FTTextureGlyphImpl::ResetActiveTexture();
#endif


#ifdef ANDROID
    impl->PreRender();
#endif


    int prevC = -1; // local prevC = nil
    for (int i = 0; text[i] != '\0'; i++)
    {
        int c = text[i];

        if(prevC != -1)
        {
            x += FormatText::GetKern(font, prevC, c);
        }

        { // Actually do the render
            unsigned int charCode = (unsigned int) c;

            // This isn't just a check! It's lazy loader
            if(!impl->CheckGlyph(charCode))
            {
                return;
            }

            unsigned int index = charMap->GlyphListIndex(charCode);



            (*glyphVector)[index]->Render(FTPoint(x, y), FTGL::RENDER_ALL);
        }

        prevC = c;
    }

#ifndef ANDROID
    glPopAttrib();
#endif

#ifdef ANDROID
    impl->PostRender();
#endif

}

void FormatText::MeasureText(FTTextureFont* font, const char* text, float maxwidth, Vector* sizeOut)
{
    if(maxwidth < 1)
    {
        float width = MeasureTextWidth(font, text);
        float height = FormatText::GetFaceMaxHeight(font);// * 0.75; // Do I really want this scaled?
        sizeOut->SetXyzw(width, height, 0, 0);
        return;
    }
    else
    {
        float outLongestLine = -1;
        float lines = (float) CountLines(font, text, maxwidth, &outLongestLine);
        float width = outLongestLine;
        if(lines == 1)
        {
            width = MeasureTextWidth(font, text);
        }
        float height = lines * FormatText::GetFaceMaxHeight(font);
        sizeOut->SetXyzw(width, height, 0, 0);
        return;
    }
}

bool FormatText::IsWhiteSpace(char c)
{
    if(c == ' ' || c == '\t')
    {
        return true;
    }
    return false;
}

// Returns the cursor position in the text
void FormatText::NextLine(FTTextureFont* font,
                          const char* text,
                          unsigned int cursor,
                          float maxwidth,
                          int* outStart,
                          int* outFinish,
                          float* outPixelWidth)
{
    assert(outFinish);
    assert(outPixelWidth);
    assert(text);
    assert(font);

    //
    // HACKISH
    //
    // Ignore leading space on a line.
    // This stops formating "Hello World" like:
    // "Hello"
    // " World"
    // When left aligned.
    if(IsWhiteSpace(text[cursor]))
    {
        cursor++;
    }
    //
    // END HACKISH
    //

    unsigned int start = cursor;
    unsigned int finish = cursor;

    int prevC = -1;
    int prevNonWhite = -1;

    float pixelWidth = 0;
    float pixelWidthStart = 0;

    for (int i = cursor; text[i] != '\0'; i++)
    {
        int c = text[i];

        if(IsWhiteSpace(c))
        {
            start = (unsigned int) std::max((int)cursor, i - 1);
            pixelWidthStart = pixelWidth;
            prevNonWhite = prevC;
        }

        if(prevC != -1)
        {
            float kern = GetKern(font, prevC, c);
            float finishW = CharPixelWidth(font, c);

            if(start == cursor or pixelWidth + kern + finishW <= maxwidth)
            {
                pixelWidth = pixelWidth + kern;
            }
            else
            {
                finishW = CharPixelWidth(font, prevNonWhite);
                // From cursor to start of the last word.
                (*outStart) = cursor;
                (*outFinish) = start + 1;
                (*outPixelWidth) = pixelWidthStart + finishW;
                return;
            }
        }

        prevC = c;
        finish++;
    }


    float finishW = 0;

    if(prevC != -1)
    {
        finishW = CharPixelWidth(font, prevC);
    }

    // From cursor to last word
    (*outStart) = cursor;
    (*outFinish) = finish;
    (*outPixelWidth) = pixelWidth + finishW;
    return;
}