include $(abs_top_builddir)/src/rules.mk

noinst_PROGRAMS=label_test widget_test \
	canvas_test ftk_test bitmap_test primary_source_test\
	main_loop_test interpolator_test animation_trigger_default_test font_desc_test params_test pairs_test

bitmap_test_SOURCES=bitmap_test.c
bitmap_test_LDFLAGS=$(MYLDFLAGS)

label_test_SOURCES=label_test.c
label_test_LDFLAGS=$(MYLDFLAGS)

widget_test_SOURCE=widget_test.c
widget_test_LDFLAGS=$(MYLDFLAGS)

canvas_test_SOURCES=canvas_test.c
canvas_test_LDFLAGS=$(MYLDFLAGS)

ftk_test_SOURCES=ftk_test.c
ftk_test_LDFLAGS=$(MYLDFLAGS)

#bitmap_factory_test_SOURCES=bitmap_factory_test.c
#bitmap_factory_test_LDFLAGS=$(MYLDFLAGS)

main_loop_test_SOURCES=main_loop_test.c
main_loop_test_LDFLAGS=$(MYLDFLAGS)

primary_source_test_SOURCES=primary_source_test.c
primary_source_test_LDFLAGS=$(MYLDFLAGS)

font_desc_test_SOURCES=font_desc_test.c
font_desc_test_LDFLAGS=$(MYLDFLAGS)


pairs_test_SOURCES=pairs_test.c
pairs_test_LDFLAGS=$(MYLDFLAGS)

params_test_SOURCES=params_test.c
params_test_LDFLAGS=$(MYLDFLAGS)

animation_trigger_default_test_SOURCES=animation_trigger_default_test.c
animation_trigger_default_test_LDFLAGS=$(MYLDFLAGS)

interpolator_test_SOURCES=interpolator_test.c
interpolator_test_LDFLAGS=$(MYLDFLAGS)

EXTRA_DIST=test_util.c xml_builder_dump.h
