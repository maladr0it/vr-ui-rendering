#!/usr/bin/env zsh
rm -rf tmp

mkdir tmp
mkdir tmp/textures

deno run --allow-run --allow-env --allow-read --allow-write \
meta/genGlyphData.ts