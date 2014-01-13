
#PSP
#######################################################
export ARCH=mips
export HOST_PARAM=--host=psp
export TARGET_CC=psp-gcc
export PATH=/usr/local/pspdev/bin/:$PATH

export PSP_CFLAGS=-D_PSP_FW_VERSION=371
export PSP_INCLUDES="-I/usr/local/pspdev/psp/sdk/include/"
export PSP_LIBS="-L/usr/local/pspdev/psp/sdk/lib/"
export PSP_LDFLAGS="-lg -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk \
	-lc \
	-lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
	-lpspuser -lpspkernel -lm -lz"

export BUILD_DIR=$PWD/$ARCH
export PREFIX=$BUILD_DIR/usr
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig/
export LDFLAGS="-L$PREFIX/lib $PSP_LIBS $PSP_LDFLAGS"
export CFLAGS="-G0 -I$PREFIX/include $PSP_INCLUDES $PSP_LDFLAGS $PSP_LIBS"
export LIBS=$LDFLAGS
