#!/bin/bash
# usage: ./package.sh binary
export BIN=$1
cd ftk/src/demos
psp-fixup-imports $BIN
mksfo 'FTK Demo' PARAM.SFO
psp-strip $BIN -o $BIN.elf
pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL $BIN.elf NULL
cp EBOOT.PBP ../../../
cp PARAM.SFO ../../../
cd ../../../
