#!/usr/bin/env zsh

rm -rf build
mkdir build

cp -R src/shaders build/shaders

cp -R tmp/textures build/textures

clang -g -Wall \
-I /usr/local/include -I ./libs -framework OpenGL \
/usr/local/lib/libglfw.dylib ./libs/**/*.c ./src/**/*.c \
-o ./build/main