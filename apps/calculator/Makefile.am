SUBDIRS=icons data
include $(abs_top_srcdir)/src/rules.mk

LOCAL_CFLAGS=-DAPP_DATA_DIR="\"$(datadir)/ftk/calc\"" -DAPP_LOCAL_DATA_DIR="\"$(srcdir)\""\
	-I$(abs_top_srcdir)/apps/common

lib_LTLIBRARIES=libapp-calc.la
libapp_calc_la_SOURCES=main.c ftk_app_calc.c
libapp_calc_la_LDFLAGS=$(MYLDFLAGS)
libapp_calc_la_CFLAGS=$(LOCAL_CFLAGS)

noinst_PROGRAMS=calc
calc_SOURCES=main.c ftk_app_calc.c
calc_LDFLAGS=$(MYLDFLAGS)
calc_CFLAGS=$(LOCAL_CFLAGS) -DHAS_MAIN

EXTRA_DIST=ftk_app_calc.h
