import { config } from "https://deno.land/x/dotenv@v1.0.1/mod.ts";

config({ export: true });

const ATLAS_GEN_PATH = Deno.env.get("MSDF_ATLAS_GEN_PATH")!;
const ASSET_PATH = `./assets`;
const TEMP_PATH = `./tmp`;
const TEXTURES_PATH = `./tmp/textures`;

type AtlasData = {
  atlas: {
    width: number;
    height: number;
  };
  glyphs: {
    unicode: number;
    advance: number;
    planeBounds?: {
      top: number;
      right: number;
      bottom: number;
      left: number;
    };
    atlasBounds?: {
      top: number;
      right: number;
      bottom: number;
      left: number;
    };
  }[];
};

const decoder = new TextDecoder();
const encoder = new TextEncoder();

// read file
const generateHeaderFile = async (
  inPath: string,
  outPath: string,
  name: string,
) => {
  const fileData = await Deno.readFile(inPath);
  const atlasData = JSON.parse(decoder.decode(fileData)) as AtlasData;

  const formatNum = (num: number) => {
    if (num === 0) {
      return `0.0f`;
    }
    return `${num}f`;
  };

  let code = "";

  code += `#ifndef ${name.toUpperCase()}_DATA_H\n`;
  code += `#define ${name.toUpperCase()}_DATA_H\n`;
  code += `\n`;
  code += `#include "../src/font.h"\n`;
  code += `\n`;
  code += `static font_glyph_t ${name.toUpperCase()}_DATA[] = {\n`;

  for (const glyph of atlasData.glyphs) {
    const atlasTop = (glyph.atlasBounds?.top ?? 0) / atlasData.atlas.height;
    const atlasRight = (glyph.atlasBounds?.right ?? 0) / atlasData.atlas.width;
    const atlasBottom = (glyph.atlasBounds?.bottom ?? 0) /
      atlasData.atlas.height;
    const atlasLeft = (glyph.atlasBounds?.left ?? 0) / atlasData.atlas.width;

    const planeTop = glyph.planeBounds?.top ?? 0;
    const planeRight = glyph.planeBounds?.right ?? 0;
    const planeBottom = glyph.planeBounds?.bottom ?? 0;
    const planeLeft = glyph.planeBounds?.left ?? 0;

    code += `    {`;
    code += `${formatNum(atlasTop)}, `;
    code += `${formatNum(atlasRight)}, `;
    code += `${formatNum(atlasBottom)}, `;
    code += `${formatNum(atlasLeft)}, `;
    code += `${formatNum(planeTop)}, `;
    code += `${formatNum(planeRight)}, `;
    code += `${formatNum(planeBottom)}, `;
    code += `${formatNum(planeLeft)}`;
    code += `},\n`;
  }

  code += `};\n`;
  code += `\n`;
  code += `#endif`;

  await Deno.writeFile(outPath, encoder.encode(code));
};

// Generate softmask
await Deno.run({
  cmd: [
    ATLAS_GEN_PATH,
    "-font",
    `${ASSET_PATH}/NewYork.ttf`,
    "-type",
    "softmask",
    "-imageout",
    `${TEXTURES_PATH}/softmask-atlas.png`,
    "-json",
    `${TEMP_PATH}/softmask-data.json`,
  ],
}).status();
await generateHeaderFile(
  `${TEMP_PATH}/softmask-data.json`,
  `${TEMP_PATH}/softmask_data.h`,
  "softmask",
);

// Generate SDF
await Deno.run({
  cmd: [
    ATLAS_GEN_PATH,
    "-font",
    `${ASSET_PATH}/NewYork.ttf`,
    "-type",
    "sdf",
    "-imageout",
    `${TEXTURES_PATH}/sdf-atlas.png`,
    "-json",
    `${TEMP_PATH}/sdf-data.json`,
  ],
}).status();
await generateHeaderFile(
  `${TEMP_PATH}/sdf-data.json`,
  `${TEMP_PATH}/sdf_data.h`,
  "sdf",
);

// Generate MSDF
await Deno.run({
  cmd: [
    ATLAS_GEN_PATH,
    "-font",
    `${ASSET_PATH}/NewYork.ttf`,
    "-type",
    "msdf",
    "-imageout",
    `${TEXTURES_PATH}/msdf-atlas.png`,
    "-json",
    `${TEMP_PATH}/msdf-data.json`,
  ],
}).status();
await generateHeaderFile(
  `${TEMP_PATH}/msdf-data.json`,
  `${TEMP_PATH}/msdf_data.h`,
  "msdf",
);
