#include "font.h"

typedef struct font
{
    unsigned int textureId;
} font_t;

static int getGlyphIndex(unsigned char code)
{
    // hack: because our glyphs represent unicode 32 to 126 (demical);
    return code - 32;
}

// TODO: this can probably be optimised a lot
void font_createGlyphVerts(font_glyph_t *glyphData, char c, font_glyphVert_t *verts)
{
    // TODO: take cursor as an argument
    v3_t cursor = v3_create(0, 0, 0);

    font_glyph_t data = glyphData[getGlyphIndex(c)];

    font_glyphVert_t bottomLeft = {
        .pos = v3_add(cursor, v3_create(data.planeLeft, data.planeBottom, 0.0f)),
        .texCoords = {data.atlasLeft, data.atlasBottom},
    };

    font_glyphVert_t bottomRight = {
        .pos = v3_add(cursor, v3_create(data.planeRight, data.planeBottom, 0.0f)),
        .texCoords = {data.atlasRight, data.atlasBottom},
    };

    font_glyphVert_t topRight = {
        .pos = v3_add(cursor, v3_create(data.planeRight, data.planeTop, 0.0f)),
        .texCoords = {data.atlasRight, data.atlasTop},
    };

    font_glyphVert_t topLeft = {
        .pos = v3_add(cursor, v3_create(data.planeLeft, data.planeTop, 0.0f)),
        .texCoords = {data.atlasLeft, data.atlasTop},
    };

    verts[0] = bottomLeft;
    verts[1] = topRight;
    verts[2] = topLeft;

    verts[3] = bottomLeft;
    verts[4] = bottomRight;
    verts[5] = topRight;
}
