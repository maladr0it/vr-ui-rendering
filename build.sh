#!/usr/bin/env zsh

rm -rf build
mkdir build

cp -R src/shaders build/.

cp -R assets/textures build/.
cp -R tmp/textures build/.

cp -R assets/meshes build/.

clang -g -Wall \
-I /usr/local/include -I ./libs -framework OpenGL \
/usr/local/lib/libglfw.dylib ./libs/**/*.c ./src/**/*.c \
-o ./build/main