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

float font_getTextWidth(font_glyph_t *glyphData, char *string)
{
    float width = 0;

    char *pString = string;
    while (*pString != '\0')
    {
        font_glyph_t data = glyphData[getGlyphIndex(*pString)];
        width += data.advance;
        ++pString;
    }
    return width;
}

// TODO: this can probably be optimised a lot
int font_createGlyphVerts(font_glyph_t *glyphData, char *string, mesh_vert_t *verts)
{
    int numVerts = 0;
    v3_t cursor = v3_create(0, 0, 0);

    char *pString = string;

    while (*pString != '\0')
    {
        font_glyph_t data = glyphData[getGlyphIndex(*pString)];

        mesh_vert_t bottomLeft = {
            .pos = v3_add(cursor, v3_create(data.planeLeft, data.planeBottom, 0.0f)),
            .texCoords = v2_create(data.atlasLeft, data.atlasBottom),
        };

        mesh_vert_t bottomRight = {
            .pos = v3_add(cursor, v3_create(data.planeRight, data.planeBottom, 0.0f)),
            .texCoords = v2_create(data.atlasRight, data.atlasBottom),
        };

        mesh_vert_t topRight = {
            .pos = v3_add(cursor, v3_create(data.planeRight, data.planeTop, 0.0f)),
            .texCoords = v2_create(data.atlasRight, data.atlasTop),
        };

        mesh_vert_t topLeft = {
            .pos = v3_add(cursor, v3_create(data.planeLeft, data.planeTop, 0.0f)),
            .texCoords = v2_create(data.atlasLeft, data.atlasTop),
        };

        cursor = v3_add(cursor, v3_create(data.advance, 0, 0));

        verts[numVerts++] = bottomLeft;
        verts[numVerts++] = topRight;
        verts[numVerts++] = topLeft;
        verts[numVerts++] = bottomLeft;
        verts[numVerts++] = bottomRight;
        verts[numVerts++] = topRight;
        ++pString;
    }

    return numVerts;
}
