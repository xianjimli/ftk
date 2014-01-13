include $(abs_top_builddir)/src/rules.mk

SUBDIRS=. im os backend compiler demos test

lib_LTLIBRARIES=libftk.la

LINUX_FB=$(srcdir)/backend/native/ftk_display_fb.c \
	$(srcdir)/backend/native/ftk_source_input.c \
	$(srcdir)/backend/native/ftk_source_ps2mouse.c \
	$(srcdir)/backend/native/ftk_backend_fb.c

LINUX_ST7781=$(srcdir)/backend/native/ftk_display_st7781.c \
	$(srcdir)/backend/native/ftk_source_input.c \
	$(srcdir)/backend/native/ftk_backend_st7781.c

LINUX_X11=$(srcdir)/backend/x11-emu/ftk_display_x11.c \
	$(srcdir)/backend/x11-emu/ftk_source_x11.c \
	$(srcdir)/backend/x11-emu/ftk_backend_x11.c

LINUX_DFB=$(srcdir)/backend/directfb/ftk_display_dfb.c \
	$(srcdir)/backend/directfb/ftk_source_dfb.c \
	$(srcdir)/backend/directfb/ftk_backend_dfb.c

LINUX_SIGMA=$(srcdir)/backend/sigma/ftk_backend_sigma.c \
	$(srcdir)/backend/native/ftk_source_input.c \
	$(srcdir)/backend/sigma/ftk_display_sigma.c

PSP=$(srcdir)/backend/psp/ftk_backend_psp.c \
	$(srcdir)/backend/psp/ftk_source_psp.c \
	$(srcdir)/backend/psp/ftk_display_psp.c

OS_LINUX=\
	$(srcdir)/os/linux/ftk_mmap_linux.c  \
	$(srcdir)/os/linux/ftk_linux.c 
OS_PSP=\
	$(srcdir)/os/psp/ftk_mmap_psp.c \
	$(srcdir)/os/psp/ftk_psp.c 

LINEBREAK= \
	$(srcdir)/linebreak/linebreak.c  \
	$(srcdir)/linebreak/linebreakdata.c  \
	$(srcdir)/linebreak/linebreakdef.c

OPTIONAL_FUNCS=ftk_file_system_posix.c ftk_file_system.c ftk_file_browser.c 

libftk_la_SOURCES=\
	ftk_canvas.c          ftk_source_primary.c      ftk_wnd_manager_default.c   \
	ftk_label.c           ftk_source_timer.c \
	ftk_bitmap.c          ftk_main_loop.c     ftk_util.c \
	ftk_source_idle.c   ftk_widget.c \
	ftk.c                 ftk_window.c        ftk_sources_manager.c\
	ftk_globals.c         ftk_button.c              ftk_image.c         ftk_log.c \
	ftk_dialog.c          ftk_status_item.c         ftk_status_panel.c  \
	ftk_menu_panel.c      ftk_menu_item.c           ftk_app_window.c    ftk_entry.c \
	ftk_text_buffer.c     ftk_icon_cache.c          \
	ftk_progress_bar.c    ftk_group_box.c         ftk_check_button.c  \
	ftk_gesture.c         ftk_wait_box.c  ftk_scroll_bar.c \
	ftk_sprite.c          ftk_display.c             ftk_xml_parser.c    ftk_xul.c \
	ftk_expr.c            ftk_list_view.c           ftk_list_model_default.c \
	ftk_list_render_default.c ftk_icon_view.c ftk_theme.c ftk_painter.c \
	ftk_text_view.c       ftk_pipe_socket.c        ftk_message_box.c \
	ftk_popup_menu.c      ftk_combo_box.c \
	ftk_clipboard.c       ftk_input_method_chooser_default.c ftk_input_method_manager.c \
	ftk_allocator.c            ftk_allocator_default.c \
	ftk_allocator_profile.c  \
	ftk_input_method_preeditor.c ftk_dlfcn.c \
	ftk_input_method_preeditor_default.c \
	ftk_main_loop_select.c ftk_config.c ftk_display_mem.c \
	ftk_display_rotate.c ftk_wnd_manager.c ftk_path.c \
	ftk_tab.c ftk_font_desc.c \
	ftk_animation_alpha.c   ftk_animation_scale.c      ftk_animation_trigger_default.c \
	ftk_animation_expand.c  ftk_animation_translate.c  ftk_animation_trigger_silence.c \
	ftk_animation.c ftk_pairs.c ftk_params.c ftk_interpolator_linear.c \
	ftk_interpolator_acc_decelerate.c  ftk_interpolator_bounce.c   \
	ftk_interpolator_accelerate.c      ftk_interpolator_decelerate.c \
	ftk_input_pattern.c ftk_key_board.c \
	$(OPTIONAL_FUNCS)

if ENABLE_LINEBREAK
libftk_la_SOURCES += $(LINEBREAK)
endif

libftk_la_CFLAGS=$(CANVAS_CFLAGS)
libftk_la_LDFLAGS=$(CANVAS_LDFLAGS)

if FTK_CANVAS_DUMMY
CANVAS_SOURCES=$(abs_srcdir)/canvas/dummy/ftk_bitmap_factory_dummy.c $(abs_srcdir)/canvas/dummy/ftk_bitmap_dummy.c  $(abs_srcdir)/canvas/dummy/ftk_canvas_dummy.c
endif

if FTK_CANVAS_SKIA 
CANVAS_SOURCES=$(abs_srcdir)/canvas/skia/ftk_bitmap_factory_skia.cpp  $(abs_srcdir)/canvas/skia/ftk_bitmap_skia.cpp  $(abs_srcdir)/canvas/skia/ftk_canvas_skia.cpp
endif

if FTK_CANVAS_CAIRO
CANVAS_SOURCES=$(abs_srcdir)/canvas/skia/ftk_bitmap_factory_skia.cpp  $(abs_srcdir)/canvas/skia/ftk_bitmap_skia.cpp  $(abs_srcdir)/canvas/skia/ftk_canvas_skia.cpp
endif

if FTK_CANVAS_DIRECTFB
CANVAS_SOURCES=$(abs_srcdir)/canvas/directfb/ftk_bitmap_factory_directfb.c $(abs_srcdir)/canvas/directfb/ftk_bitmap_directfb.c  $(abs_srcdir)/canvas/directfb/ftk_canvas_directfb.c
libftk_la_CFLAGS+=-DFTK_FONT="\"font.ttf\"" 
endif

if FTK_CANVAS_DEFAULT

if FONT_ENGINE_FREETYPE
libftk_la_CFLAGS+=-DFTK_FONT="\"font.ttf\"" 
endif

if FONT_ENGINE_DEFAULT
libftk_la_CFLAGS+=-DFTK_FONT="\"gb2312.fnt\"" 
endif

CANVAS_SOURCES=\
	$(abs_srcdir)/canvas/default/ftk_font.c \
	$(abs_srcdir)/canvas/default/ftk_canvas_default.c \
	$(abs_srcdir)/canvas/default/ftk_bitmap_default.c \
	$(abs_srcdir)/canvas/default/ftk_font_manager.c \
	$(abs_srcdir)/canvas/default/ftk_bitmap_factory.c \
	$(abs_srcdir)/canvas/default/ftk_image_png_decoder.c \
	$(abs_srcdir)/canvas/default/ftk_image_jpeg_decoder.c \
	$(abs_srcdir)/canvas/default/ftk_image_bmp_decoder.c

if FONT_ENGINE_FREETYPE
CANVAS_SOURCES+=$(abs_srcdir)/canvas/default/ftk_font_freetype.c
endif

if FONT_ENGINE_DEFAULT
CANVAS_SOURCES+=$(abs_srcdir)/canvas/default/ftk_font_default.c $(abs_srcdir)/canvas/default/fontdata.c
endif
endif

libftk_la_SOURCES+=$(CANVAS_SOURCES)

if ENABLE_CAIRO
libftk_la_SOURCES+=ftk_cairo.c
libftk_la_SOURCES += im/default/ftk_stroke_painter_cairo.c
endif

if ENABLE_TSLIB
LINUX_FB+=$(srcdir)/backend/native/ftk_source_tslib.c 
LINUX_ST7781+=$(srcdir)/backend/native/ftk_source_tslib.c 
endif

if FTK_BACKEND_FB
libftk_la_SOURCES+=$(LINUX_FB) $(OS_LINUX)
endif

if FTK_BACKEND_ST7781
libftk_la_SOURCES+=$(LINUX_ST7781) $(OS_LINUX)
endif

if FTK_BACKEND_X11
libftk_la_SOURCES+=$(LINUX_X11) $(OS_LINUX)
endif

if ENABLE_FRIBIDI
libftk_la_SOURCES+=ftk_text_layout_normal.c
else
libftk_la_SOURCES+=ftk_text_layout_normal.c
endif
if FTK_BACKEND_DFB
libftk_la_SOURCES+=$(LINUX_DFB) $(OS_LINUX)
endif

if FTK_BACKEND_SIGMA
libftk_la_SOURCES+=$(LINUX_SIGMA) $(OS_LINUX)
libftk_la_CFLAGS+=-I$(abs_top_srcdir)/src/backend/sigma
libftk_la_CFLAGS+=-I$(abs_top_srcdir)/src/backend/native
endif

if FTK_BACKEND_PSP
libftk_la_SOURCES+=$(PSP) $(OS_PSP)
libftk_la_CFLAGS+=-I$(abs_top_srcdir)/src/backend/psp
endif

if ENABLE_GPINYIN
libftk_la_SOURCES += im/gpinyin/share/ngram.cpp \
	im/gpinyin/share/sync.cpp \
	im/gpinyin/share/spellingtrie.cpp \
	im/gpinyin/share/dicttrie.cpp \
	im/gpinyin/share/dictbuilder.cpp \
	im/gpinyin/share/spellingtable.cpp \
	im/gpinyin/share/splparser.cpp \
	im/gpinyin/share/searchutility.cpp \
	im/gpinyin/share/utf16char.cpp \
	im/gpinyin/share/mystdlib.cpp \
	im/gpinyin/share/utf16reader.cpp \
	im/gpinyin/share/userdict.cpp \
	im/gpinyin/share/matrixsearch.cpp \
	im/gpinyin/share/pinyinime.cpp \
	im/gpinyin/share/lpicache.cpp \
	im/gpinyin/share/dictlist.cpp \
	im/gpinyin/ftk/ftk_input_method_gpinyin.cpp
else
libftk_la_SOURCES += im/default/ftk_input_method_util.c 
if ENABLE_HANDWRITE
libftk_la_SOURCES += im/default/ftk_input_method_hw.c fhw/ftk_handwrite_engine_fhw.c \
	fhw/fhw_primitive_recognizer.c
endif
endif

ftkdir=$(includedir)/ftk
ftk_HEADERS=\
	ftk_allocator_default.h \
	ftk_allocator.h \
	ftk_allocator_profile.h \
	ftk_animation_alpha.h \
	ftk_animation_expand.h \
	ftk_animation.h \
	ftk_animation_scale.h \
	ftk_animation_translate.h \
	ftk_animation_trigger_default.h \
	ftk_animation_trigger.h \
	ftk_animation_trigger_silence.h \
	ftk_app_window.h \
	ftk_backend.h \
	ftk_bitmap_factory.h \
	ftk_bitmap.h \
	ftk_button.h \
	ftk_cairo.h \
	ftk_canvas.h \
	ftk_check_button.h \
	ftk_clipboard.h \
	ftk_combo_box.h \
	ftk_config.h \
	ftk_dialog.h \
	ftk_display.h \
	ftk_display_mem.h \
	ftk_display_rotate.h \
	ftk_dlfcn.h \
	ftk_entry.h \
	ftk_event.h \
	ftk_expr.h \
	ftk_file_browser.h \
	ftk_file_system.h \
	ftk_font_desc.h \
	ftk_gc.h \
	ftk_gesture.h \
	ftk_gesture_listener.h \
	ftk_globals.h \
	ftk.h \
	ftk_icon_cache.h \
	ftk_icon_view.h \
	ftk_image_decoder.h \
	ftk_image.h \
	ftk_input_method_chooser.h \
	ftk_input_method.h \
	ftk_input_method_manager.h \
	ftk_input_method_preeditor_default.h \
	ftk_input_method_preeditor.h \
	ftk_interpolator_acc_decelerate.h \
	ftk_interpolator_accelerate.h \
	ftk_interpolator_bounce.h \
	ftk_interpolator_decelerate.h \
	ftk_interpolator.h \
	ftk_interpolator_linear.h \
	ftk_key.h \
	ftk_input_pattern.h \
	ftk_label.h \
	ftk_list_model_default.h \
	ftk_list_model.h \
	ftk_list_render_default.h \
	ftk_list_render.h \
	ftk_list_view.h \
	ftk_log.h \
	ftk_main_loop.h \
	ftk_menu_item.h \
	ftk_menu_panel.h \
	ftk_message_box.h \
	ftk_mmap.h \
	ftk_painter.h \
	ftk_pairs.h \
	ftk_params.h \
	ftk_path.h \
	ftk_pipe.h \
	ftk_platform.h \
	ftk_popup_menu.h \
	ftk_progress_bar.h \
	ftk_group_box.h \
	ftk_scroll_bar.h \
	ftk_source.h \
	ftk_source_idle.h \
	ftk_source_primary.h \
	ftk_sources_manager.h \
	ftk_source_timer.h \
	ftk_sprite.h \
	ftk_status_item.h \
	ftk_status_panel.h \
	ftk_tab.h \
	ftk_text_buffer.h \
	ftk_text_layout.h \
	ftk_text_view.h \
	ftk_theme.h \
	ftk_translator.h \
	ftk_typedef.h \
	ftk_util.h \
	ftk_wait_box.h \
	ftk_widget.h \
	ftk_window.h \
	ftk_wnd_manager_default.h \
	ftk_wnd_manager.h \
	ftk_xml_builder.h \
	ftk_xml_parser.h \
	ftk_xul.h \
	im/default/ftk_input_method_util.h \
	fhw/fhw_primitive_recognizer.h 

EXTRA_DIST=	\
	ftk_input_method_preeditor_default.h \
	linebreak/linebreakdef.h linebreak/linebreak.h \
	ftk_translator.c  ftk_translator.h ftk_file_system_dummy.c


