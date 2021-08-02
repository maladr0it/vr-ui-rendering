#ifndef FONT_H
#define FONT_H

#include "v3.h"
#include "v2.h"

typedef struct font_glyphVert
{
    v3_t pos;
    v2_t texCoords;
} font_glyphVert_t;

typedef struct font_glyph
{
    float atlasTop;
    float atlasRight;
    float atlasBottom;
    float atlasLeft;
    float planeTop;
    float planeRight;
    float planeBottom;
    float planeLeft;
    float advance;
} font_glyph_t;

void font_createGlyphVerts(font_glyph_t *glyphData, char c, font_glyphVert_t *verts);

#endif