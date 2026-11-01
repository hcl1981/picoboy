#!/bin/sh
# Baut den Emulator (micropython.mjs/.wasm) im lokalen Emscripten-Image,
# ohne Netz. Die Ausgabe liegt in ports/webassembly/build-picoboy.
#   sh ports/webassembly/variants/picoboy/baue.sh   (aus firmware/mp)
set -e
docker run --rm --pull=never --network none -u "$(id -u):$(id -g)" -e HOME=/tmp \
  -v "$PWD:/src" -w /src emscripten/emsdk:3.1.74 sh -c '
    make -s -C mpy-cross BUILD=build-docker -j8 &&
    make -C ports/webassembly VARIANT=picoboy -j8 \
      MICROPY_MPYCROSS=/src/mpy-cross/build-docker/mpy-cross \
      MICROPY_MPYCROSS_DEPENDENCY=/src/mpy-cross/build-docker/mpy-cross "$@"' sh "$@"
