#ifndef FONT_H
#define FONT_H

#include "v3.h"
#include "v2.h"
#include "mesh.h"

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

int font_createGlyphVerts(font_glyph_t *glyphData, char *string, mesh_vert_t *verts);

float font_getTextWidth(font_glyph_t *glyphData, char *string);

#endif