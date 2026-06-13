#!/usr/bin/env bash
# Build the native TOML support library (tomlc17 + shim) into lib/libztoml.a.
# Run this once before `thor build` / `thor run`; the Zap binding links -lztoml.
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CC="${CC:-cc}"
OUT="$DIR/lib"
mkdir -p "$OUT"

echo "Compiling tomlc17.c ..."
$CC -O2 -fPIC -I"$DIR/tomlc17/src" -c "$DIR/tomlc17/src/tomlc17.c" -o "$OUT/tomlc17.o"

echo "Compiling toml_shim.c ..."
$CC -O2 -fPIC -I"$DIR/tomlc17/src" -c "$DIR/csrc/toml_shim.c" -o "$OUT/toml_shim.o"

echo "Archiving libztoml.a ..."
ar rcs "$OUT/libztoml.a" "$OUT/tomlc17.o" "$OUT/toml_shim.o"

echo "Done: $OUT/libztoml.a"
