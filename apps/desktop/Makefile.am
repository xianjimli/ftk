SUBDIRS=icons xul
include $(abs_top_srcdir)/src/rules.mk
LOCAL_CFLAGS=-I$(abs_top_srcdir)/apps/common  -DAPP_DATA_DIR="\"$(datadir)/ftk/desktop\"" \
	-DAPP_LOCAL_DATA_DIR="\"$(srcdir)\"" -I$(abs_top_srcdir)/script_binding/lua \
	-I$(abs_top_srcdir)/script_binding/lua/lua \
	-DFTK_HAS_LUA -DFTK_HAS_TESTER

LUALDFLAGS=$(abs_top_builddir)/script_binding/lua/libftklua.la $(abs_top_builddir)/script_binding/lua/lua/liblua.la

noinst_PROGRAMS=app_info_test
app_info_test_SOURCES=app_info.c ftk_app_lua.c
app_info_test_CFLAGS=-DAPP_INFO_TEST $(LOCAL_CFLAGS)
app_info_test_LDFLAGS=$(MYLDFLAGS) -ldl $(LUALDFLAGS)

bin_PROGRAMS=desktop
desktop_SOURCES=main.c app_info.c ftk_tester.c
desktop_LDFLAGS=$(MYLDFLAGS) -ldl $(LUALDFLAGS)
desktop_CFLAGS=$(LOCAL_CFLAGS) 

if ENABLE_VNC
SUBDIRS += libvncserver .
desktop_SOURCES += vnc_service.c ftk_app_lua.c
desktop_CFLAGS += -I$(srcdir)/libvncserver
desktop_LDFLAGS += $(builddir)/libvncserver/libvncserver.la -lpthread 
endif

EXTRA_DIST=app_info.h vnc_service.h ftk_tester.h ftk_app_lua.h
