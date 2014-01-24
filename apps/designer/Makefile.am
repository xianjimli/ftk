SUBDIRS=icons data
include $(abs_top_srcdir)/src/rules.mk

LOCAL_CFLAGS=-DAPP_DATA_DIR="\"$(datadir)/ftk/designer\"" -DAPP_LOCAL_DATA_DIR="\"$(srcdir)\"" \
	-I$(abs_top_srcdir)/apps/common

lib_LTLIBRARIES=libapp-designer.la
libapp_designer_la_SOURCES=code_gen.c  ftk_app_designer.c widget_editor.c  widgets_info.c save.c open.c select_file.c
libapp_designer_la_LDFLAGS=$(MYLDFLAGS)
libapp_designer_la_CFLAGS=$(LOCAL_CFLAGS)

noinst_PROGRAMS=designer
designer_SOURCES=code_gen.c  ftk_app_designer.c  main.c  widget_editor.c  widgets_info.c save.c open.c select_file.c
designer_LDFLAGS=$(MYLDFLAGS)
designer_CFLAGS=$(LOCAL_CFLAGS) -DHAS_MAIN

EXTRA_DIST=ftk_app_designer.h
