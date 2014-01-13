all: check zlib libpng jpeg libftk
all_clean: zlib_clean libpng_clean jpeg_clean libftk_clean 

check:
	if [ ! -e packages ]; then mkdir packages;fi

zlib_download:
	if [ ! -f "packages/.zlib_downloaded" ]; then \
		cd packages;\
		svn checkout svn://svn.ps2dev.org/psp/trunk/zlib; \
	fi
	touch packages/.zlib_downloaded
zlib: zlib_download
	if [ ! -f "packages/.zlib_compiled" ]; then \
		cd packages/zlib && \
		make clean; make && sudo make install; \
	fi
	touch packages/.zlib_compiled
zlib_clean:
	rm packages/.zlib_compiled && \
	cd packages/zlib && make clean

jpeg_download:
	if [ ! -f "packages/.jpeg_downloaded" ]; then \
		cd packages;\
		svn checkout svn://svn.ps2dev.org/psp/trunk/jpeg; \
	fi
	touch packages/.jpeg_downloaded 
jpeg: jpeg_download
	if [ ! -f "packages/.jpeg_compiled" ]; then \
		cd packages/jpeg && \
		make clean; make && sudo make install; \
	fi
	touch  packages/.jpeg_compiled
jpeg_clean:
	rm packages/.jpeg_compiled && \
	cd packages/jpeg && make clean

libpng_download: 
	if [ ! -f "packages/.libpng_downloaded" ]; then \
		cd packages;\
		svn checkout svn://svn.ps2dev.org/psp/trunk/libpng; \
	fi
	touch packages/.libpng_downloaded
libpng: libpng_download
	if [ ! -f "packages/.libpng_compiled" ]; then \
		cd packages/libpng && \
		make clean; make && sudo make install; \
	fi
	touch packages/.libpng_compiled
libpng_clean:
	rm packages/.libpng_compiled && \
	cd packages/libpng && make clean

libftk:
	cd ftk && ./autogen.sh;
	mkdir ftk/$(ARCH);  cd ftk/$(ARCH) && \
	../configure ac_cv_func_realloc_0_nonnull=yes ac_cv_func_malloc_0_nonnull=yes  --with-backend=psp $(HOST_PARAM) --prefix=$(PREFIX)  &&\
	make clean; make && make install
libftk_clean:
	rm -rf ftk/$(ARCH)

