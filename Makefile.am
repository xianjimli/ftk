SUBDIRS=. po src script_binding data theme tools testdata docs

if ENABLE_OPENGLES
SUBDIRS += opengles
endif

if FTK_BACKEND_PSP
else
SUBDIRS+=fbus services apps
endif
pkgconfigdir=$(libdir)/pkgconfig

pkgconfig_DATA=ftk.pc 
ACLOCAL_AMFLAGS=-I m4

cnfdir=$(datadir)/ftk/
cnf_DATA=ftk.cnf

EXTRA_DIST=autogen.sh ftk.cnf.in ftk.pc.in gtk-doc.make

ftkdir=$(includedir)/ftk
ftk_HEADERS=config_ftk.h
