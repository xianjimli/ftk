include $(abs_top_srcdir)/src/rules.mk


lib_LTLIBRARIES=libecho.la

libecho_la_SOURCES=fbus_echo.c
libecho_la_CFLAGS=-I$(top_srcdir)/fbus
libecho_la_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la

noinst_PROGRAMS=echo_test

echo_test_SOURCES=echo_test.c
echo_test_CFLAGS=-I$(top_srcdir)/fbus
echo_test_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la \
	$(abs_top_builddir)/fbus/demo/libecho.la

bin_PROGRAMS=echo_service
echo_service_SOURCES=fbus_service_echo.c
echo_service_CFLAGS=-I$(top_srcdir)/fbus
echo_service_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la

servicedir=$(datadir)/ftk/base/services
service_DATA=echo.service

EXTRA_DIST=fbus_echo_common.h  fbus_echo.h
