#broncho 1900
#######################################################
#export ARCH=arm
#export HOST_PARAM=--host=arm-linux
##export TARGET_CC=arm-linux-gcc
#export PATH=/usr/local/arm-linux-4.1.1/bin/:$PATH

#sh
#######################################################
export ARCH=sh4
export HOST_PARAM=--host=sh4-linux
export TARGET_CC=sh4-linux-gcc
export PATH=/opt/STM/STLinux-2.3/devkit/sh4/bin/:$PATH
export TARGET_CFLAGS="-DFTK_FONT_SIZE=32"

#openmoko
#######################################################
#export ARCH=arm
#export HOST_PARAM=--host=arm-linux
#export TARGET_CC=arm-linux-gcc
#export PATH=/usr/local/openmoko/arm/bin/:$PATH

#mini4020
#######################################################
#export ARCH=arm
#export HOST_PARAM=--host=arm-linux
#export TARGET_CC=arm-linux-gcc
#export PATH=/usr/local/arm/3.3.2/bin/:$PATH

#jz4740
#######################################################
#export ARCH=mipsel
#export HOST_PARAM=--host=mipsel-linux
#export TARGET_CC=mipsel-linux-gcc
#export PATH=/work/board/jz/mipseltools-gcc412-glibc261/bin:$PATH

#ben
#######################################################
#export ARCH=mipsel
#export HOST_PARAM=--host=mipsel-openwrt-linux
#export TARGET_CC=mipsel-openwrt-linux-gcc
#export PATH=/work/board/jz/openwrt-xburst/staging_dir/toolchain-mipsel_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin:$PATH

#zoran
#######################################################
#export ARCH=mipsisa32
#export TARGET_CFLAGS="-DUSE_FB_ACTIVATE_ALL -DFTK_FONT_SIZE=20"
#export HOST_PARAM=--host=mipsisa32-linux
#export TARGET_CC=mipsisa32-linux-gcc
#export PATH=/opt/timesys/toolchains/mipsisa32-linux/bin:$PATH
#export WITH_BACKEND="--with-backend=linux-dfb"

#codesourery
#######################################################
#export ARCH=arm
#export HOST_PARAM=--host=arm-none-linux-gnueabi
#export TARGET_CC=arm-none-linux-gnueabi-gcc
#export PATH=/root/CodeSourcery/Sourcery_G++_Lite/bin/:$PATH

#sigma, 
# maybe you need change the path.
#######################################################
#cd /home/smp/smp86xx_toolchain_2.8.3.0 && source toolchain-path.env && cd -
#cd /home/smp/mrua_SMP8634_2.8.3.0_GCC4_dev.mips && source MRUA.env && cd -
#export ARCH=mipsel
#export HOST_PARAM=--host=mipsel-linux
#export TARGET_CC=mipsel-linux-gcc
#export SIGMA_OBJS="$RUA_DIR/samples/dbgimplementation.o $RUA_DIR/samples/rmmmimplementation.o $RUA_DIR/samples/get_time.o $RUA_DIR/samples/get_key.o $RUA_DIR/samples/process_key.o $RUA_DIR/samples/parse_display_cmdline.o $RUA_DIR/samples/parse_video_cmdline.o $RUA_DIR/samples/parse_capture_cmdline.o $RUA_DIR/samples/parse_audio_cmdline.o $RUA_DIR/samples/parse_playback_cmdline.o $RUA_DIR/samples/osdlib.o $RUA_DIR/samples/dvi_hdmi.o $RUA_DIR/samples/dvi_hdmi_update.o $RUA_DIR/samples/dss_sha.o $RUA_DIR/samples/boot_osd.o $RUA_DIR/samples/rminputstream.o $RUA_DIR/samples/parsemp4dsi.o $RUA_DIR/samples/bitmaps.o $RUA_DIR/samples/rmmemfile.o $RUA_DIR/samples/ccparse.o $RUA_DIR/samples/bcc_init.o $RUA_DIR/samples/bcc_feed.o $RUA_DIR/samples/bcc_close.o $RUA_DIR/samples/psfdemux_drm.o $RUA_DIR/samples/psfdemux_parsing.o $RUA_DIR/samples/outports_options.o $RUA_DIR/samples/checksum.o $RUA_DIR/samples/mp4descriptors.o $RUA_DIR/samples/mp4scene.o $RUA_DIR/samples/rmttx.o $RUA_DIR/samples/rua_memory.o $RUA_DIR/samples/rmpfs.o $RUA_DIR/samples/asfdemux_callback.o $RUA_DIR/samples/ad9380.o $RUA_DIR/samples/cpu_uart_gpio.o $RUA_DIR/samples/i2c.o $RUA_DIR/samples/mcp23008.o $RUA_DIR/samples/msp4450g.o $RUA_DIR/samples/tw9919eid.o"
#export SIGMA_CFLAGS="-I$RUA_DIR/samples/ -DWITH_PROD=1 -DEM86XX_CHIP=EM86XX_CHIPID_TANGO2 -DEM86XX_REVISION=6 -DDEMUX_PSF=1 -DEM86XX_MODE=EM86XX_MODEID_STANDALONE -DWITH_IRQHANDLER_BOOTLOADER=1 -DWITH_XLOADED_UCODE=1  -DGCC4_TOOLCHAIN -DWITH_RM_FILE_JPEG -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1 -mips32r2 -Wa,-mips32r2 -I$RUA_DIR -I. -D_REENTRANT -I.. -U_DEBUG -DNDEBUG -DRMPLATFORM=RMPLATFORMID_TANGO2 -Wundef -Wall -Wchar-subscripts -Wsign-compare -Wuninitialized -O -Wno-missing-braces -Wnested-externs -Wmissing-declarations -Wmissing-prototypes"
#export SIGMA_LIBS="-L$RUA_DIR/lib -Xlinker --start-group -ldcc -lrmmonitoring -lrmvdemux -lrmjpeg -lrmungif -lrmpng -lrmzlib -lrmhttp -lrmscc -lrmavicore -lrmmp4api -lrmmp4 -lrmmp4core -lrmdescriptordecoder -lrmmpeg4framework -lrmrtk86 -lrmwmaprodecoder -lrmwmaprodecodercore -lrmasfdemux -lrmasfdemuxcore -lrmstreamingprotocols -lrmcpputils -lrmcapture -lrmcw -lrmcore -lrmcdfs -lrua -lllad -lrmdtcpinterface -lrmdemuxwritekeyapi -lrmsoftmixer -lrmwmdrmndstub -lgbus -lrmdrm -Xlinker --end-group  -ldl -rdynamic"

export BUILD_DIR=$PWD/$ARCH
export PREFIX=/opt/ftk-$ARCH
export STAGING=${BUILD_DIR}/staging
export DIST=${BUILD_DIR}/dist
export PKG_CONFIG_PATH=${STAGING}/${PREFIX}/lib/pkgconfig/
export LDFLAGS="-L${STAGING}/${PREFIX}/lib -Wl,-rpath -Wl,${PREFIX}/lib"
export CPPFLAGS="-I${STAGING}/${PREFIX}/include $TARGET_CFLAGS"
export CFLAGS=$CPPFLAGS 
export CXXFLAGS=$CPPFLAGS
export DIRS="--libdir=${STAGING}/lib --includedir=${STAGING}/include"
#export CAIRO=cairo
#export TSLIB=tslib
export FTK_CONF_OPTION="--disable-cairo --disable-tslib --disable-profile --with-fontengine=freetype"

