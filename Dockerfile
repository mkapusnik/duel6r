# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS builder

ARG DEBIAN_FRONTEND=noninteractive
ARG D6R_RENDERER=gl4

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

WORKDIR /src

COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DD6R_RENDERER=${D6R_RENDERER} \
    && cmake --build build -j"$(nproc)"

RUN mkdir -p /release \
    && cp build/duel6r /release/duel6r \
    && cp -R resources/* /release/

FROM busybox:1.36.1-uclibc AS artifact-exporter

COPY --from=builder /release/ /release/

CMD ["true"]
