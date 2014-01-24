all: check zlib png jpeg $(TSLIB) freetype $(CAIRO) libftk
all_clean:  zlib_clean png_clean jpeg_clean  tslib_clean freetype_clean libftk_clean

all_dfb: check zlib png jpeg $(TSLIB) freetype directfb directfb_examples $(CAIRO) libftk
all_dfb_clean:  zlib_clean png_clean jpeg_clean tslib_clean freetype_clean directfb_clean directfb_examples_clean libftk_clean

check:
	if [ ! -e packages ]; then mkdir packages;fi
	cp -fv ftk/third/* packages

zlib-1.2.8:
	tar xf packages/zlib-1.2.8.tar.gz
zlib: zlib-1.2.8
	export CC=$(TARGET_CC) PREFIX=$(PREFIX) && \
	cd zlib-1.2.8 && \
	./configure --prefix=$(PREFIX) --shared &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
zlib_clean:
	cd zlib-1.2.8 && make clean

libpng-1.6.8:
	tar xf packages/libpng-1.6.8.tar.xz
png: libpng-1.6.8
	mkdir libpng-1.6.8/$(ARCH); cd libpng-1.6.8/$(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX)  &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
png_clean:
	rm -rf libpng-1.6.8/$(ARCH)

jpegsrc.v9:
	tar xf packages/jpegsrc.v9.tar.gz
jpeg: jpegsrc.v9
	mkdir jpeg-9/$(ARCH); cd jpeg-9/$(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX)  &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
jpeg_clean:
	rm -rf jpeg-9/$(ARCH)

tslib-1.x: 
	tar xf packages/tslib-1.x.tar.gz

tslib: tslib-1.x
	cd tslib-1.x && ./autogen.sh; cd -;\
	mkdir tslib-1.x/$(ARCH); cd tslib-1.x/$(ARCH) && \
	CC=$(TARGET_CC) ../configure $(HOST_PARAM) --prefix=$(PREFIX) -sysconfdir=$(PREFIX)/etc  ac_cv_func_malloc_0_nonnull=yes &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 

tslib_clean:
	rm -rf tslib-1.x/$(ARCH)
tslib_source_clean:
	rm -rf tslib-1.x

packages/freetype-2.5.2.tar.bz2:
	cd packages && wget http://ftp.twaren.net/Unix/NonGNU/freetype/freetype-2.5.2.tar.bz2
freetype-2.5.2: packages/freetype-2.5.2.tar.bz2
	tar xf packages/freetype-2.5.2.tar.bz2
freetype: freetype-2.5.2
	mkdir freetype-2.5.2/$(ARCH); cd freetype-2.5.2/$(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX)  &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
freetype_clean:
	rm -rf freetype-2.5.2/$(ARCH)

packages/DirectFB-1.2.9.tar.gz:
	cd packages && wget http://www.directfb.org/downloads/Core/DirectFB-1.2/DirectFB-1.2.9.tar.gz
DirectFB-1.2.9: packages/DirectFB-1.2.9.tar.gz
	tar xf packages/DirectFB-1.2.9.tar.gz
directfb: DirectFB-1.2.9
	mkdir DirectFB-1.2.9/$(ARCH); cd DirectFB-1.2.9/$(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX) --with-inputdrivers="keyboard,linuxinput,tslib" --enable-unique --enable-freetype --enable-png --enable-jpeg --enable-zlib --enable-fbdev --enable-debug-support --with-gfxdrivers= &&\
	make clean; make && make install DESTDIR=${STAGING} && make install DESTDIR=/ 
directfb_clean:
	rm -rf DirectFB-1.2.9/$(ARCH)

packages/DirectFB-examples-1.2.0.tar.gz:
	cd packages && wget http://www.directfb.org/downloads/Extras/DirectFB-examples-1.2.0.tar.gz

DirectFB-examples-1.2.0: packages/DirectFB-examples-1.2.0.tar.gz
	tar xf packages/DirectFB-examples-1.2.0.tar.gz
directfb_examples:DirectFB-examples-1.2.0
	mkdir DirectFB-examples-1.2.0/$(ARCH); cd DirectFB-examples-1.2.0/$(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX) && \
	make clean; make && make install DESTDIR=${STAGING} && make install DESTDIR=/
directfb_examples_clean:
	rm -rf DirectFB-examples-1.2.0/$(ARCH)

packages/expat-2.0.1.tar.gz:
	cd packages && wget http://downloads.sourceforge.net/project/expat/expat/2.0.1/expat-2.0.1.tar.gz?use_mirror=nchc
expat-2.0.1: packages/expat-2.0.1.tar.gz
	tar xf packages/expat-2.0.1.tar.gz
expat:expat-2.0.1
	cd expat* && mkdir $(ARCH); cd $(ARCH) && \
	../configure $(HOST_PARAM) --prefix=$(PREFIX) && \
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
expat_clean:
	cd expat* && rm -fr $(ARCH)

packages/fontconfig-2.8.0.tar.gz:
	cd packages && wget http://www.fontconfig.org/release/fontconfig-2.8.0.tar.gz
fontconfig-2.8.0:packages/fontconfig-2.8.0.tar.gz
	tar xf packages/fontconfig-2.8.0.tar.gz
fontconfig: fontconfig-2.8.0 expat 
	cd fontconfig* && mkdir $(ARCH); cd $(ARCH) && \
	../configure --disable-libxml2 --with-arch=$(ARCH)-linux $(HOST_PARAM) --prefix=$(PREFIX) && \
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
fontconfig_clean:
	cd fontconfig* && rm -fr $(ARCH)

packages/pixman-0.22.2.tar.gz:
	cd packages && wget http://cairographics.org/releases/pixman-0.22.2.tar.gz
pixman-0.22.2:packages/pixman-0.22.2.tar.gz
	tar xf packages/pixman-0.22.2.tar.gz
pixman:pixman-0.22.2
	cd pixman* && mkdir $(ARCH); cd $(ARCH) && \
	../configure --disable-gtk $(HOST_PARAM) --prefix=$(PREFIX) && \
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
pixman_clean:
	cd pixman* && rm -fr $(ARCH)

packages/cairo-1.10.2.tar.gz:
	cd packages && wget http://cairographics.org/releases/cairo-1.10.2.tar.gz
cairo-1.10.2:packages/cairo-1.10.2.tar.gz
	tar xf packages/cairo-1.10.2.tar.gz
cairo: cairo-1.10.2 pixman fontconfig
	cd cairo* && mkdir $(ARCH); cd $(ARCH) && \
	../configure  --enable-ft   --disable-svg  --disable-pdf  --disable-ps \
	--disable-directfb  --disable-xlib  --disable-xlib-xrender --disable-gobject \
	$(HOST_PARAM) --prefix=$(PREFIX) && \
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
cairo_clean:
	cd cairo* && rm -fr $(ARCH)

libftk:
	cd ftk* && . ./autogen.sh; \
	mkdir $(ARCH);  cd $(ARCH) && \
	../configure $(FTK_CONF_OPTION) $(WITH_BACKEND) ac_cv_func_realloc_0_nonnull=yes ac_cv_func_malloc_0_nonnull=yes $(HOST_PARAM) --prefix=$(PREFIX)  &&\
	make clean; make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
libftk_clean:
	cd ftk* && rm -rf $(ARCH)
ftkc:
	cd ftk* && cd $(ARCH) && \
	make && make install DESTDIR=${STAGING} && \
	make install DESTDIR=/ 
libftk_clean:
	cd ftk* && rm -rf $(ARCH)


