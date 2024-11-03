#!/usr/bin/bash
set -ex
set ACCEPT_EULA=Y
apt update
apt install -y --no-install-recommends cmake make ninja-build fuse libfuse2 \
  libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev

cd /root

wget https://github.com/Neargye/magic_enum/releases/download/v0.9.6/magic_enum-v0.9.6.tar.gz
mkdir magic_enum
tar -xzf magic_enum-v0.9.6.tar.gz -C magic_enum
cmake -S magic_enum -B magic_enum/build -DCMAKE_BUILD_TYPE=Release
cmake --install magic_enum/build
rm -rf *

wget https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.4.0.tar.gz
tar -xzf v3.4.0.tar.gz
mv tomlplusplus-3.4.0 tomlplusplus
cmake -S tomlplusplus -B tomlplusplus/build -DCMAKE_BUILD_TYPE=Release
cmake --install tomlplusplus/build
rm -rf *

wget https://github.com/kokkos/mdspan/archive/refs/tags/mdspan-0.6.0.tar.gz
tar -xzf mdspan-0.6.0.tar.gz
mv mdspan-mdspan-0.6.0 mdspan
cmake -S mdspan -B mdspan/build -DCMAKE_BUILD_TYPE=Release
cmake --install mdspan/build
rm -rf *

wget https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz
tar -xzf eigen-3.4.0.tar.gz
mv eigen-3.4.0 eigen
cmake -S eigen -B eigen/build -DCMAKE_BUILD_TYPE=Release
cmake --install eigen/build
rm -rf *

wget https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-x86_64.AppImage
mv linuxdeploy-x86_64.AppImage /usr/bin/linuxdeploy
chmod +x /usr/bin/linuxdeploy

wget https://github.com/darealshinji/linuxdeploy-plugin-checkrt/releases/download/continuous/linuxdeploy-plugin-checkrt.sh
mv linuxdeploy-plugin-checkrt.sh /usr/bin
chmod +x /usr/bin/linuxdeploy-plugin-checkrt.sh
