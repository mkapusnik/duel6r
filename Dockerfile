# syntax=docker/dockerfile:1

FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        libgl1-mesa-dev \
        libglew-dev \
        liblua5.3-dev \
        libsdl2-dev \
        libsdl2-image-dev \
        libsdl2-mixer-dev \
        libsdl2-ttf-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY docker/build.sh /usr/local/bin/duel6r-build

RUN chmod +x /usr/local/bin/duel6r-build

ENTRYPOINT ["/usr/local/bin/duel6r-build"]
