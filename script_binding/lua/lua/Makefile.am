lib_LTLIBRARIES=liblua.la
liblua_la_SOURCES= lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
    lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c  \
    lundump.c lvm.c lzio.c\
	lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c \
    lstrlib.c loadlib.c linit.c print.c

if FTK_BACKEND_PSP
liblua_la_CFLAGS=-DLUA_USE_PSP
else
liblua_la_CFLAGS=-DLUA_USE_LINUX
endif


if FTK_BACKEND_PSP
else
liblua_la_LDFLAGS=-ldl
endif

EXTRA_DIST=lapi.h     ldebug.h  lgc.h      lmem.h      lparser.h  ltable.h   lua.h      lvm.h\
lauxlib.h  ldo.h     llex.h     lobject.h   lstate.h   ltm.h      lualib.h   lzio.h\
lcode.h    lfunc.h   llimits.h  lopcodes.h  lstring.h  luaconf.h  lundump.h lua.c      luac.c
