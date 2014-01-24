SUBDIRS=. demo
include $(abs_top_srcdir)/src/rules.mk

lib_LTLIBRARIES=libfbus.la
libfbus_la_SOURCES=\
	fbus_parcel.c   fbus_service_infos.c           fbus_source_listen.c   fbus_stream.c \
	fbus_proxy.c    fbus_service_manager.c         fbus_source_proxy.c    fbus_stream_socket.c \
	fbus_service.c  fbus_source_service.c
libfbus_la_LDFLAGS=$(MYLDFLAGS)

noinst_PROGRAMS=parcel_test service_manager_test service_infos_test

parcel_test_SOURCES=fbus_parcel.c
parcel_test_CFLAGS=-DFBUS_PARCEL_TEST
parcel_test_LDFLAGS=$(MYLDFLAGS)

service_infos_test_SOURCES=fbus_service_infos.c
service_infos_test_CFLAGS=-DFBUS_SERVICE_INFOS_TEST
service_infos_test_LDFLAGS=$(MYLDFLAGS)

service_manager_test_SOURCES=service_manager_test.c
service_manager_test_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la

bin_PROGRAMS=service_manager
service_manager_SOURCES=fbus_service_manager_daemon.c
service_manager_LDFLAGS=$(MYLDFLAGS) $(abs_top_builddir)/fbus/libfbus.la 

ftkdir=$(includedir)/fbus
ftk_HEADERS=\
	fbus_parcel.h         fbus_service_manager_common.h  fbus_source_service.h\
	fbus_proxy.h          fbus_service_manager.h         fbus_stream.h\
	fbus_service.h        fbus_source_listen.h           fbus_stream_socket.h\
	fbus_service_infos.h  fbus_source_proxy.h            fbus_typedef.h

