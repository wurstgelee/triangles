#!/bin/bash
# create multiresolution windows icon
ICON_DST=../../src/qt/res/icons/triangles.ico

convert ../../src/qt/res/icons/triangles-32.png ../../src/qt/res/icons/triangles-32.png ../../src/qt/res/icons/triangles-48.png ${ICON_DST}
