SUBDIRS=icons data
include $(abs_top_srcdir)/src/rules.mk

LOCAL_CFLAGS=-DAPP_DATA_DIR="\"$(datadir)/ftk/shell\"" -DAPP_LOCAL_DATA_DIR="\"$(srcdir)\"" \
	-I$(abs_top_srcdir)/apps/common

lib_LTLIBRARIES=libapp-shell.la
libapp_shell_la_SOURCES=ftk_app_shell.c ftk_source_shell.c
libapp_shell_la_LDFLAGS=$(MYLDFLAGS) -lpthread
libapp_shell_la_CFLAGS=$(LOCAL_CFLAGS)

noinst_PROGRAMS=shell
shell_SOURCES=main.c ftk_app_shell.c ftk_source_shell.c
shell_LDFLAGS=$(MYLDFLAGS) -lpthread
shell_CFLAGS=$(LOCAL_CFLAGS) -DHAS_MAIN

EXTRA_DIST=ftk_app_shell.h  ftk_source_shell.h
