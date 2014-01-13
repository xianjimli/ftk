lib_LTLIBRARIES=libvncserver.la
libvncserver_la_SOURCES=\
	auth.c      httpd.c      stats.c                 vncauth.c\
	cargs.c     main.c       zlib.c\
	corre.c     minilzo.c    zrle.c\
	cursor.c    rfbregion.c  \
	cutpaste.c  rfbserver.c  zrleoutstream.c\
	d3des.c     rre.c        zrlepalettehelper.c\
	draw.c      scale.c      tight.c\
	font.c      selbox.c     translate.c\
	hextile.c   sockets.c    ultra.c
libvncserver_la_LDFLAGS=-ljpeg -lpng -lz -lpthread
libvncserver_la_CFLAGS=-I$(srcdir)/rfb

EXTRA_DIST=\
	$(srcdir)/lzoconf.h \
	$(srcdir)/d3des.h \
	$(srcdir)/zrletypes.h \
	$(srcdir)/rfb/rfb.h \
	$(srcdir)/rfb/rfbint.h \
	$(srcdir)/rfb/rfbregion.h \
	$(srcdir)/rfb/rfbconfig.h \
	$(srcdir)/rfb/keysym.h \
	$(srcdir)/rfb/rfbproto.h \
	$(srcdir)/rfb/rfbclient.h \
	$(srcdir)/rfb/default8x16.h \
	$(srcdir)/private.h \
	$(srcdir)/zrlepalettehelper.h \
	$(srcdir)/zrleoutstream.h \
	$(srcdir)/scale.h \
	$(srcdir)/minilzo.h \
	tableinit24.c          tableinittctemplate.c   tabletranstemplate.c\
	tableinitcmtemplate.c  tabletrans24template.c  zrleencodetemplate.c\
	tableinitcmtemplate.c  tabletrans24template.c\
	tableinittctemplate.c  tabletranstemplate.c

