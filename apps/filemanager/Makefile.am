SUBDIRS=icons data
include $(abs_top_srcdir)/src/rules.mk

LOCAL_CFLAGS=-DAPP_DATA_DIR="\"$(datadir)/ftk/filem\"" -DAPP_LOCAL_DATA_DIR="\"$(srcdir)\"" \
	-I$(abs_top_srcdir)/apps/common

lib_LTLIBRARIES=libapp-filem.la
libapp_filem_la_SOURCES=ftk_app_file_browser.c
libapp_filem_la_LDFLAGS=$(MYLDFLAGS)
libapp_filem_la_CFLAGS=$(LOCAL_CFLAGS)

noinst_PROGRAMS=filem
filem_SOURCES=main.c ftk_app_file_browser.c
filem_LDFLAGS=$(MYLDFLAGS)
filem_CFLAGS=$(LOCAL_CFLAGS) -DHAS_MAIN

EXTRA_DIST=ftk_app_file_browser.h
