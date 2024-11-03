FROM gcc:14.2.0
RUN --mount=type=bind,target=/tmp/scripts,source=scripts \
    sh tmp/scripts/docker_build.sh
ARG _UID
ARG _GID
ARG _USER
RUN groupadd --gid ${_GID} ${_USER} \
    && useradd --create-home --uid ${_UID} --gid ${_GID} --shell /bin/bash ${_USER}
