include $(abs_top_srcdir)/src/rules.mk

lib_LTLIBRARIES=libfconf.la

libfconf_la_SOURCES=fconf_client.c
libfconf_la_CFLAGS=-I$(top_srcdir)/fbus
libfconf_la_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la

noinst_PROGRAMS=fconf_test fconf_xml_test

fconf_xml_test_SOURCES=fconf_xml.c 
fconf_xml_test_CFLAGS=-DFCONF_XML_TEST -I$(top_srcdir)/fbus
fconf_xml_test_LDFLAGS=$(MYLDFLAGS)

#path_test_SOURCES=ftk_path.c
#path_test_CFLAGS=-DFTK_PATH_TEST
#path_test_LDFLAGS=$(MYLDFLAGS)

fconf_test_SOURCES=fconf_test.c 
fconf_test_CFLAGS=-I$(top_srcdir)/fbus
fconf_test_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la \
	$(builddir)/libfconf.la

bin_PROGRAMS=fconf_service
fconf_service_SOURCES=fconf_service.c service.c fconf_xml.c 
fconf_service_CFLAGS=-I$(top_srcdir)/fbus
fconf_service_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la

servicedir=$(datadir)/ftk/base/services
service_DATA=fconf.service

fconfincludedir=$(includedir)/fconf
fconfinclude_HEADERS=fconf_client.h  fconf.h fconf_share.h

EXTRA_DIST=fconf_xml.h fconf_service.h gen.sh fconf.c

