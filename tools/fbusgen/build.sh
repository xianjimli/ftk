#!/bin/bash
SRC_FILES="main.c coder.c coder_intf.c coder_client.c coder_service.c coder_xxxx.c coder_share.c"
export PKG_CONFIG_PATH=/usr/lib/pkgconfig/:/usr/local/lib/pkgconfig/
gcc -Wall -g `pkg-config --cflags --libs libIDL-2.0` $SRC_FILES -o codegen
./codegen -type=share fbus_conf.idl
./codegen -type=impl fbus_conf.idl
./codegen -type=intf fbus_conf.idl
./codegen -type=client fbus_conf.idl
./codegen -type=service fbus_conf.idl
