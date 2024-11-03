#!/usr/bin/bash
set -ex
cd $(dirname "$0")
docker build -t shadok_and_gibby --build-arg _UID="$(id -u)" --build-arg _GID="$(id -g)" --build-arg _USER=$USER app/linux/AppImage
docker run --rm --user $(id -u):$(id -g) --mount type=bind,src=$(pwd),dst=/tmp/project --workdir=/tmp/project shadok_and_gibby \
  sh app/linux/AppImage/build.sh
