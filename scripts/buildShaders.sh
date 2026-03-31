#!/bin/bash

cd ../shaders/ || exit

for f in *.vert.slang; do
    [ -e "$f" ] || continue
    slangc "$f" -stage vertex -target spirv -o "${f}.spv"
done

# Compile all Fragment files
for f in *.frag.slang; do
    [ -e "$f" ] || continue
    slangc "$f" -stage fragment -target spirv -o "${f}.spv"
done

cd ../scripts/ || exit