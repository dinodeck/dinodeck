#ifndef FORMATTEXT_H
#define FORMATTEXT_H

#include "DDTextAlign.h"

class FTTextureFont;
class Vector;

class FormatText
{
public:
    static float GetKern(FTTextureFont* font, int current, int next);
    static float CharPixelWidth(FTTextureFont* font, int c);
    static float GetFaceMaxHeight(FTTextureFont* font);

    // Still using FTGL pipeline, probably be nicer in grahpics pipeline.
    static void PushText
    (
        FTTextureFont* font,
        float x,
        float y,
        const char* text,
        const Vector& colour,
        AlignX::Enum alignX,
        AlignY::Enum alignY
    );

    static void PushTextWrapped
    (
        FTTextureFont* font,
        float x,
        float y,
        const char* text,
        const Vector& colour,
        AlignX::Enum alignX,
        AlignY::Enum alignY,
        float maxwidth
    );

    static bool IsWhiteSpace(char c);

    static void NextLine
    (
        FTTextureFont* font,
        const char* text,
        unsigned int cursor,
        float maxwidth,
        int* outStart,
        int* outFinish,
        float* outPixelWidth
    );

    static void RenderLine
    (
        FTTextureFont* font,
        float x, float y,
        const char* text,
        int start, int finish
    );

    static float MeasureTextWidth
    (
        FTTextureFont* font,
        const char* text
    );

    static int CountLines
    (
        FTTextureFont* font,
        const char* text,
        float maxwidth,
        float* outMaxLineWidth
    );

    static void MeasureText
    (
        FTTextureFont* font,
        const char* text,
        float maxwidth,
        Vector* sizeOut
    );
};

#endif
