#!/bin/bash
export PKG_CONFIG_PATH=/usr/lib/pkgconfig/:/usr/local/lib/pkgconfig/
gcc -Wall -g `pkg-config --cflags --libs libIDL-2.0` luagen.c -o luagen

