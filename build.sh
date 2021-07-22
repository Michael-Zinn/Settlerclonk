#!/usr/bin/env bash

# clear build directory

## Delete the old build directory if it exists
if [[ -d build ]]; then
  rm -rf build
fi
## create a new one
mkdir build



# Build 7K_Settlerclonk.c4d

## Copy, excluding files that weren't there originally
rsync -a \
  --exclude=.git \
  --exclude=.gitignore \
  --exclude=build \
  --exclude=build.sh \
  --exclude=fakevideoplayer.png \
  --exclude=LICENSE \
  --exclude=makefile \
  --exclude=README.md \
  ./ build/7K_Settlerclonk.c4d

## Pack
c4group build/7K_Settlerclonk.c4d -p
