#!/bin/bash

# Create the directories and remove the old rules.txt
rm -rf tiled-maps/tilesets
mkdir -p tiled-maps/tilesets
rm -f tiled-maps/rules.txt

# Make the tileset converter tool
make -C tileset-converter

echo '# Rules' > tiled-maps/rules.txt

# Build each tileset
for tileset in data/tilesets/*.png
do
    # Remove the dir name and the extension
    tset="${tileset##*/}"
    tset="${tset%.*}"
    # And build the tileset
    echo "Generating tileset $tset"
    tileset-converter/tileset-converter $tileset tiled-maps/tilesets/$tset
    echo tilesets/$tset/rules.txt >> tiled-maps/rules.txt
done
