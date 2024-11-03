#!/usr/bin/bash
set -ex
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_INSTALL_PREFIX=build/AppDir/usr
cmake --build ./build
cmake --install ./build
cd build
linuxdeploy --appimage-extract-and-run --appdir ./AppDir --output appimage --plugin=checkrt
