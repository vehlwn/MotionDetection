FROM ubuntu:24.04 AS builder

RUN apt update \
    && apt upgrade --yes \
    && apt install --yes \
        cmake \
        g++ \
        gcc \
        meson \
        ninja-build \
        pkg-config \
        python3 \
        python3-venv

WORKDIR /app

RUN python3 -m venv venv
RUN ./venv/bin/pip install conan==2.26.0
RUN ./venv/bin/conan profile detect

COPY conanfile.txt .
RUN ./venv/bin/conan install . \
    --output-folder build \
    --build missing \
    --settings build_type=Release \
    --settings compiler.cppstd=23 \
    --conf tools.system.package_manager:mode=install

COPY meson.build meson_options.txt app.ini drogon.json .
COPY meson meson
COPY src src
RUN cat build/conan_meson_native.ini
RUN cd build && meson setup --native-file conan_meson_native.ini -D build_testing=true ..
RUN meson compile -C build
RUN meson test -C build
RUN ls -lah build/src
