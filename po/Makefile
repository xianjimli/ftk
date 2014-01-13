# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize

GETTEXT_PACKAGE = ftk
PACKAGE = ftk
VERSION = 1.0

SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
libdir = ${exec_prefix}/lib
localedir = $(libdir)/locale
gnulocaledir = $(datadir)/locale
gettextsrcdir = $(datadir)/glib-2.0/gettext/po
subdir = po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_srcdir)/./mkinstalldirs

CC = gcc
GENCAT = @GENCAT@
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
MSGFMT_OPTS = 
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2 -I/usr/include/libpng15   -DHAVE_CONFIG_FTK_H -DHAVE_CONFIG_FTK_H -I/usr/X11R6/include -DUSE_LINUX_X11 -DDISPLAY_PARAM="\"linux-x11:320x480\"" -DUSE_DEFAULT_FONT  -DUSE_GPINYIN -DUSE_LINEBREAK  -DUSE_VNC
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = 
POFILES =  zh_CN.po
GMOFILES =  zh_CN.gmo
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(GETTEXT_PACKAGE).pot \
$(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	../opengles/ug/ugshape.c \
	../opengles/ug/ug_win32.c \
	../opengles/ug/ug_ftk.c \
	../opengles/ug/uglu.c \
	../opengles/src/codegen/emit.c \
	../opengles/src/codegen/arm-dis.c \
	../opengles/src/codegen/heap.c \
	../opengles/src/codegen/instruction.c \
	../opengles/src/codegen/bitset.c \
	../opengles/src/codegen/arm-codegen.c \
	../opengles/src/codegen/segment.c \
	../opengles/redbook/checker.c \
	../opengles/redbook/polyoff.c \
	../opengles/redbook/varray.c \
	../opengles/redbook/mipmap.c \
	../opengles/redbook/aargb.c \
	../opengles/redbook/texsub.c \
	../opengles/redbook/cube.c \
	../opengles/redbook/fog.c \
	../opengles/redbook/movelight.c \
	../opengles/redbook/hello.c \
	../opengles/redbook/wrap.c \
	../opengles/redbook/material.c \
	../opengles/redbook/texbind.c \
	../opengles/redbook/alpha3D.c \
	../opengles/redbook/stencil.c \
	../opengles/redbook/colormat.c \
	../opengles/redbook/scene.c \
	../opengles/redbook/lines.c \
	../opengles/redbook/smooth.c \
	../opengles/redbook/model.c \
	../opengles/redbook/robot.c \
	../opengles/redbook/torus.c \
	../opengles/redbook/planet.c \
	../opengles/redbook/alpha.c \
	../opengles/redbook/light.c \
	../opengles/redbook/double.c \
	../opengles/redbook/stroke.c \
	../services/fconf/fconf_xml.c \
	../services/fconf/service.c \
	../services/fconf/fconf_test.c \
	../services/fconf/fconf.c \
	../services/fconf/fconf_service.c \
	../services/fconf/fconf_client.c \
	../src/ftk_key_board.c \
	../src/ftk_popup_menu.c \
	../src/ftk_text_layout_bidi.c \
	../src/ftk_tab.c \
	../src/ftk_sources_manager.c \
	../src/ftk_animation.c \
	../src/ftk_icon_cache.c \
	../src/ftk_text_buffer.c \
	../src/ftk_canvas.c \
	../src/os/linux/ftk_linux.c \
	../src/os/linux/ftk_mmap_linux.c \
	../src/os/windows/ftk_mmap_win32.c \
	../src/os/windows/ftk_font_win32.c \
	../src/os/windows/ftk_input_method_win32.c \
	../src/os/windows/ftk_win32.c \
	../src/os/sdl/ftk_image_sdl_decoder.c \
	../src/os/rt-thread/ftk_mmap_rtthread.c \
	../src/os/rt-thread/ftk_select_rtthread.c \
	../src/os/rt-thread/ftk_input_method_chooser_rtthread.c \
	../src/os/rt-thread/ftk_pipe_rtthread.c \
	../src/os/rt-thread/ftk_rtthread.c \
	../src/os/android/ftk_mmap_android.c \
	../src/os/android/ftk_android.c \
	../src/os/android/ftk_input_method_chooser_android.c \
	../src/os/android/ftk_image_android_decoder.c \
	../src/os/android/ftk_input_method_android.c \
	../src/os/psp/ftk_psp.c \
	../src/os/psp/ftk_mmap_psp.c \
	../src/os/wince/ftk_mmap_wince.c \
	../src/os/wince/ftk_wince.c \
	../src/os/wince/ftk_input_method_wince.c \
	../src/os/gtk/ftk_gtk.c \
	../src/os/gtk/ftk_image_gtk_decoder.c \
	../src/os/sylixos/gsnap.c \
	../src/os/sylixos/ftk_mmap_sylixos.c \
	../src/os/sylixos/ftk_sylixos.c \
	../src/os/ucos-ii/ftk_mmap_ucos.c \
	../src/os/ucos-ii/ftk_ucos_sim.c \
	../src/os/ucos-ii/ftk_main_loop_ucos.c \
	../src/os/iphone/ftk_input_method_iphone.c \
	../src/os/iphone/ftk_input_method_chooser_iphone.c \
	../src/os/iphone/ftk_mmap_iphone.c \
	../src/ftk_sprite.c \
	../src/ftk_interpolator_linear.c \
	../src/ftk_config.c \
	../src/ftk_text_layout_normal.c \
	../src/ftk_interpolator_accelerate.c \
	../src/ftk_allocator_default.c \
	../src/ftk_input_method_preeditor.c \
	../src/ftk_globals.c \
	../src/ftk_xul.c \
	../src/ftk_animation_translate.c \
	../src/ftk_status_panel.c \
	../src/ftk_cairo.c \
	../src/ftk_allocator_profile.c \
	../src/ftk_file_system_dummy.c \
	../src/ftk_interpolator_acc_decelerate.c \
	../src/ftk_util.c \
	../src/linebreak/linebreakdef.c \
	../src/linebreak/linebreak.c \
	../src/linebreak/linebreakdata.c \
	../src/ftk_button.c \
	../src/ftk_source_timer.c \
	../src/ftk_list_model_default.c \
	../src/ftk_menu_item.c \
	../src/ftk_source_idle.c \
	../src/ftk_painter.c \
	../src/ftk_pipe_socket.c \
	../src/ftk_menu_panel.c \
	../src/ftk_path.c \
	../src/ftk_entry.c \
	../src/ftk_wnd_manager_default.c \
	../src/ftk_progress_bar.c \
	../src/ftk_display_rotate.c \
	../src/ftk_source_primary.c \
	../src/ftk_dialog.c \
	../src/ftk_file_system_posix.c \
	../src/ftk_display.c \
	../src/ftk_main_loop_select.c \
	../src/ftk_dlfcn.c \
	../src/ftk_font_desc.c \
	../src/ftk_image.c \
	../src/ftk_input_method_chooser_default.c \
	../src/ftk_list_view.c \
	../src/ftk_pairs.c \
	../src/ftk_status_item.c \
	../src/ftk_translator.c \
	../src/ftk_log.c \
	../src/fhw/fhw_primitive_recognizer.c \
	../src/fhw/ftk_handwrite_engine_fhw.c \
	../src/ftk_main_loop.c \
	../src/ftk_animation_alpha.c \
	../src/ftk_expr.c \
	../src/ftk_wait_box.c \
	../src/ftk_label.c \
	../src/ftk_file_browser.c \
	../src/ftk_message_box.c \
	../src/ftk_input_method_preeditor_default.c \
	../src/ftk_animation_trigger_default.c \
	../src/ftk_interpolator_decelerate.c \
	../src/ftk_allocator.c \
	../src/ftk_window.c \
	../src/ftk_widget.c \
	../src/ftk_wnd_manager.c \
	../src/ftk_check_button.c \
	../src/ftk_group_box.c \
	../src/ftk_app_window.c \
	../src/ftk_list_render_default.c \
	../src/ftk_animation_expand.c \
	../src/ftk_theme.c \
	../src/ftk_display_mem.c \
	../src/im/default/ftk_input_method_util.c \
	../src/im/default/ftk_handwrite_engine_dummy.c \
	../src/im/default/ftk_stroke_painter_cairo.c \
	../src/im/default/ftk_input_method_hw.c \
	../src/ftk_icon_view.c \
	../src/compiler/android/jni/ftk/pnggccrd.c \
	../src/compiler/android/jni/ftk/pngwtran.c \
	../src/compiler/android/jni/ftk/pngwio.c \
	../src/compiler/android/jni/ftk/pngrutil.c \
	../src/compiler/android/jni/ftk/pngmem.c \
	../src/compiler/android/jni/ftk/pngpread.c \
	../src/compiler/android/jni/ftk/pngset.c \
	../src/compiler/android/jni/ftk/png.c \
	../src/compiler/android/jni/ftk/pngtest.c \
	../src/compiler/android/jni/ftk/pngwrite.c \
	../src/compiler/android/jni/ftk/pngerror.c \
	../src/compiler/android/jni/ftk/pngrtran.c \
	../src/compiler/android/jni/ftk/pngwutil.c \
	../src/compiler/android/jni/ftk/pngget.c \
	../src/compiler/android/jni/ftk/pngvcrd.c \
	../src/compiler/android/jni/ftk/pngrio.c \
	../src/compiler/android/jni/ftk/pngread.c \
	../src/compiler/android/jni/ftk/pngtrans.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffobjs.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffparse.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffpic.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cff.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffcmap.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffdrivr.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffload.c \
	../src/compiler/android-ndk/jni/freetype/src/cff/cffgload.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/psobjs.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/psauxmod.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/psaux.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/t1cmap.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/t1decode.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/afmparse.c \
	../src/compiler/android-ndk/jni/freetype/src/psaux/psconv.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/sfntpic.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/sfnt.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttsbit0.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttkern.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/sfobjs.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttpost.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttload.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttsbit.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttbdf.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttmtx.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/sfdriver.c \
	../src/compiler/android-ndk/jni/freetype/src/sfnt/ttcmap.c \
	../src/compiler/android-ndk/jni/freetype/src/psnames/psnames.c \
	../src/compiler/android-ndk/jni/freetype/src/psnames/pspic.c \
	../src/compiler/android-ndk/jni/freetype/src/psnames/psmodule.c \
	../src/compiler/android-ndk/jni/freetype/src/raster/rastpic.c \
	../src/compiler/android-ndk/jni/freetype/src/raster/ftrend1.c \
	../src/compiler/android-ndk/jni/freetype/src/raster/raster.c \
	../src/compiler/android-ndk/jni/freetype/src/raster/ftraster.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshmod.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshpic.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshalgo.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshglob.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshrec.c \
	../src/compiler/android-ndk/jni/freetype/src/pshinter/pshinter.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afloader.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afdummy.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afangles.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afcjk.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afhints.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afpic.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/aflatin.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/aflatin2.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afwarp.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afglobal.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/autofit.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afmodule.c \
	../src/compiler/android-ndk/jni/freetype/src/autofit/afindic.c \
	../src/compiler/android-ndk/jni/freetype/src/smooth/ftsmooth.c \
	../src/compiler/android-ndk/jni/freetype/src/smooth/smooth.c \
	../src/compiler/android-ndk/jni/freetype/src/smooth/ftspic.c \
	../src/compiler/android-ndk/jni/freetype/src/smooth/ftgrays.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttobjs.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttdriver.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttpic.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttgload.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttinterp.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttgxvar.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/ttpload.c \
	../src/compiler/android-ndk/jni/freetype/src/truetype/truetype.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftrfork.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftmm.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftoutln.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftapi.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftbbox.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftcalc.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftpatent.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftgasp.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftdebug.c \
	../src/compiler/android-ndk/jni/freetype/src/base/fttrigon.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftpic.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftlcdfil.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftobjs.c \
	../src/compiler/android-ndk/jni/freetype/src/base/basepic.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftxf86.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftgloadr.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftdbgmem.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftglyph.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftsynth.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftstream.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftstroke.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftinit.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftbitmap.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftutil.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftwinfnt.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftsnames.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftadvanc.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftbase.c \
	../src/compiler/android-ndk/jni/freetype/src/base/ftsystem.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_sem.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_mutex.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_dbg_r.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_mbox.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_q.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_core.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_mem.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_time.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/ucos_ii.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_task.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii/os_flag.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/app/app.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/app/ucos_sim.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii-port/win32/os_trace.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii-port/win32/os_cpu_c.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii-lib/ucos_mutex.c \
	../src/compiler/ucos-vs2008/UCOS_SIM/src/ucosii-lib/ucos_pipe.c \
	../src/ftk_input_pattern.c \
	../src/ftk_interpolator_bounce.c \
	../src/ftk_input_method_manager.c \
	../src/ftk_xml_parser.c \
	../src/ftk_animation_scale.c \
	../src/ftk_scroll_bar.c \
	../src/ftk_gesture.c \
	../src/ftk_text_view.c \
	../src/ftk_animation_trigger_silence.c \
	../src/ftk_bitmap.c \
	../src/ftk_file_system.c \
	../src/ftk_combo_box.c \
	../src/ftk_params.c \
	../src/ftk.c \
	../src/test/bitmap_factory_test.c \
	../src/test/widget_test.c \
	../src/test/xml_parser_test.c \
	../src/test/canvas_test.c \
	../src/test/display_test.c \
	../src/test/timer_test.c \
	../src/test/tslib_test.c \
	../src/test/test_util.c \
	../src/test/theme_test.c \
	../src/test/input_test.c \
	../src/test/fs_test.c \
	../src/test/params_test.c \
	../src/test/font_manager_test.c \
	../src/test/font_test.c \
	../src/test/pairs_test.c \
	../src/test/tab_test.c \
	../src/test/font_desc_test.c \
	../src/test/clipboard_test.c \
	../src/test/log_test.c \
	../src/test/keyboard_test.c \
	../src/test/config_test.c \
	../src/test/ftk_test.c \
	../src/test/lookup_table_test.c \
	../src/test/bitmap_test.c \
	../src/test/expr_test.c \
	../src/test/sources_manager_test.c \
	../src/test/png_test.c \
	../src/test/animation_trigger_default_test.c \
	../src/test/icon_cache_test.c \
	../src/test/list_model_default_test.c \
	../src/test/interpolator_test.c \
	../src/test/xul_stress_test.c \
	../src/test/bmp_test.c \
	../src/test/main_loop_test.c \
	../src/test/backend_sigma_test.c \
	../src/test/xul_test.c \
	../src/test/util_test.c \
	../src/test/primary_source_test.c \
	../src/test/xml_builder_dump.c \
	../src/test/label_test.c \
	../src/test/text_buffer_test.c \
	../src/canvas/default/ftk_font_manager.c \
	../src/canvas/default/fontdata.c \
	../src/canvas/default/ftk_font.c \
	../src/canvas/default/ftk_image_jpeg_decoder.c \
	../src/canvas/default/ftk_canvas_default.c \
	../src/canvas/default/ftk_font_default.c \
	../src/canvas/default/ftk_image_bmp_decoder.c \
	../src/canvas/default/ftk_image_png_decoder.c \
	../src/canvas/default/f.c \
	../src/canvas/default/ftk_bitmap_default.c \
	../src/canvas/default/ftk_bitmap_factory.c \
	../src/canvas/default/ftk_font_freetype.c \
	../src/canvas/directfb/ftk_bitmap_factory_directfb.c \
	../src/canvas/directfb/ftk_canvas_directfb.c \
	../src/canvas/directfb/ftk_bitmap_directfb.c \
	../src/canvas/dummy/ftk_bitmap_factory_dummy.c \
	../src/canvas/dummy/ftk_bitmap_dummy.c \
	../src/canvas/dummy/ftk_canvas_dummy.c \
	../src/demos/ftk_app_demo.c \
	../src/demos/demo_tab.c \
	../src/demos/demo_check_button.c \
	../src/demos/demo_dialog.c \
	../src/demos/test_util.c \
	../src/demos/demo_combo_box.c \
	../src/demos/demo_button.c \
	../src/demos/demo_bidi.c \
	../src/demos/demo_file_browser.c \
	../src/demos/demo_icon_view.c \
	../src/demos/demo_hello.c \
	../src/demos/demo_image.c \
	../src/demos/demo_statusbar.c \
	../src/demos/demo_ime.c \
	../src/demos/demo_image_button.c \
	../src/demos/demo_text_view.c \
	../src/demos/demo_msgbox.c \
	../src/demos/demo_label.c \
	../src/demos/demo_listview.c \
	../src/demos/demo_draw.c \
	../src/demos/demo_keyboard.c \
	../src/demos/demo_progress_bar.c \
	../src/demos/demo_cairo.c \
	../src/demos/demo_fullscreen.c \
	../src/demos/demo_multi_win.c \
	../src/demos/demo_entry.c \
	../src/demos/demo_menu.c \
	../src/demos/demo_popup.c \
	../src/demos/demo_sprite.c \
	../src/demos/demo_xul.c \
	../src/demos/demo_transparent.c \
	../src/demos/demo_wait_box.c \
	../src/demos/demo_scroll_bar.c \
	../src/ftk_clipboard.c \
	../src/backend/sdl/ftk_source_sdl.c \
	../src/backend/sdl/ftk_display_sdl.c \
	../src/backend/sdl/ftk_backend_sdl.c \
	../src/backend/rt-thread/ftk_backend_rtthread.c \
	../src/backend/rt-thread/ftk_display_rtthread.c \
	../src/backend/rt-thread/ftk_source_touch.c \
	../src/backend/rt-thread/ftk_source_input.c \
	../src/backend/ucosii/ftk_backend_fb.c \
	../src/backend/ucosii/ftk_backend_ucosii.c \
	../src/backend/ucosii/ftk_display_ucosii.c \
	../src/backend/ucosii/ftk_source_input.c \
	../src/backend/x11-emu/ftk_display_x11.c \
	../src/backend/x11-emu/ftk_backend_x11.c \
	../src/backend/x11-emu/ftk_source_x11.c \
	../src/backend/sylixos_gmem/ftk_tscalibrate_sylixos.c \
	../src/backend/sylixos_gmem/ftk_display_sylixos.c \
	../src/backend/sylixos_gmem/ftk_source_sylixos.c \
	../src/backend/sylixos_gmem/font_8x8.c \
	../src/backend/sylixos_gmem/ftk_backend_sylixos.c \
	../src/backend/sylixos_gmem/font_8x16.c \
	../src/backend/android/ftk_backend_android.c \
	../src/backend/android/ftk_display_android_skia.c \
	../src/backend/android/ftk_display_android_gles.c \
	../src/backend/android/ftk_source_android.c \
	../src/backend/directfb/ftk_source_dfb.c \
	../src/backend/directfb/ftk_backend_dfb.c \
	../src/backend/directfb/ftk_display_dfb.c \
	../src/backend/psp/ftk_display_psp.c \
	../src/backend/psp/ftk_backend_psp.c \
	../src/backend/psp/ftk_source_psp.c \
	../src/backend/native/ftk_display_fb.c \
	../src/backend/native/ftk_backend_gles.c \
	../src/backend/native/ftk_backend_fb.c \
	../src/backend/native/ftk_source_ps2mouse.c \
	../src/backend/native/ftk_source_input.c \
	../src/backend/native/ftk_display_st7781.c \
	../src/backend/native/ftk_backend_st7781.c \
	../src/backend/native/ftk_source_tslib.c \
	../src/backend/sigma/ftk_display_sigma.c \
	../src/backend/sigma/ftk_backend_sigma.c \
	../src/backend/win32/ftk_source_win32.c \
	../src/backend/win32/ftk_backend_win32.c \
	../src/backend/wince/ftk_display_wince_ddraw.c \
	../src/backend/wince/ftk_display_wince_gles.c \
	../src/backend/wince/ftk_backend_wince.c \
	../src/backend/wince/ftk_display_wince_gdi.c \
	../src/backend/wince/ftk_source_wince.c \
	../src/backend/gtk/ftk_display_gtk.c \
	../src/backend/gtk/ftk_source_gtk.c \
	../src/backend/gtk/ftk_backend_gtk.c \
	../src/backend/iphone/ftk_backend_iphone.c \
	../fbus/fbus_stream.c \
	../fbus/fbus_service_manager.c \
	../fbus/service_manager_test.c \
	../fbus/fbus_stream_socket.c \
	../fbus/fbus_service.c \
	../fbus/fbus_service_manager_daemon.c \
	../fbus/fbus_source_service.c \
	../fbus/demo/echo_test.c \
	../fbus/demo/fbus_service_echo.c \
	../fbus/demo/fbus_echo.c \
	../fbus/fbus_service_infos.c \
	../fbus/fbus_proxy.c \
	../fbus/fbus_source_proxy.c \
	../fbus/fbus_source_listen.c \
	../fbus/fbus_parcel.c \
	../apps/shell/ftk_app_shell.c \
	../apps/shell/main.c \
	../apps/shell/ftk_source_shell.c \
	../apps/calculator/main.c \
	../apps/calculator/ftk_app_calc.c \
	../apps/designer/ftk_app_designer.c \
	../apps/designer/code_gen.c \
	../apps/designer/open.c \
	../apps/designer/select_file.c \
	../apps/designer/main.c \
	../apps/designer/save.c \
	../apps/designer/widgets_info.c \
	../apps/designer/widget_editor.c \
	../apps/filemanager/main.c \
	../apps/filemanager/ftk_app_file_browser.c \
	../apps/common/ftk_app_dummy.c \
	../apps/desktop/app_info.c \
	../apps/desktop/libvncserver/tableinittctemplate.c \
	../apps/desktop/libvncserver/stats.c \
	../apps/desktop/libvncserver/font.c \
	../apps/desktop/libvncserver/tight.c \
	../apps/desktop/libvncserver/tabletrans24template.c \
	../apps/desktop/libvncserver/scale.c \
	../apps/desktop/libvncserver/hextile.c \
	../apps/desktop/libvncserver/rfbserver.c \
	../apps/desktop/libvncserver/cutpaste.c \
	../apps/desktop/libvncserver/zrleoutstream.c \
	../apps/desktop/libvncserver/ultra.c \
	../apps/desktop/libvncserver/selbox.c \
	../apps/desktop/libvncserver/cursor.c \
	../apps/desktop/libvncserver/tableinitcmtemplate.c \
	../apps/desktop/libvncserver/zrlepalettehelper.c \
	../apps/desktop/libvncserver/translate.c \
	../apps/desktop/libvncserver/main.c \
	../apps/desktop/libvncserver/d3des.c \
	../apps/desktop/libvncserver/zrle.c \
	../apps/desktop/libvncserver/rfbregion.c \
	../apps/desktop/libvncserver/tabletranstemplate.c \
	../apps/desktop/libvncserver/corre.c \
	../apps/desktop/libvncserver/rre.c \
	../apps/desktop/libvncserver/sockets.c \
	../apps/desktop/libvncserver/zlib.c \
	../apps/desktop/libvncserver/vncauth.c \
	../apps/desktop/libvncserver/auth.c \
	../apps/desktop/libvncserver/cargs.c \
	../apps/desktop/libvncserver/zrleencodetemplate.c \
	../apps/desktop/libvncserver/tableinit24.c \
	../apps/desktop/libvncserver/draw.c \
	../apps/desktop/libvncserver/httpd.c \
	../apps/desktop/libvncserver/minilzo.c \
	../apps/desktop/vnc_service.c \
	../apps/desktop/main.c \
	../apps/desktop/ftk_app_lua.c \
	../apps/desktop/ftk_tester.c \
	../apps/launcher/main.c \
	../tools/luagen/luagen.c \
	../tools/fbusgen/coder_client.c \
	../tools/fbusgen/coder_intf.c \
	../tools/fbusgen/coder_share.c \
	../tools/fbusgen/main.c \
	../tools/fbusgen/coder.c \
	../tools/fbusgen/fbusgen.c \
	../tools/fbusgen/coder_xxxx.c \
	../tools/fbusgen/coder_service.c \
	../tools/imconvert/shrink.c \
	../tools/imconvert/fitx_convert.c \
	../tools/fontextract/unicode.c \
	../tools/fontextract/fontdata.c \
	../tools/fontextract/fontextract.c \
	../tools/fontextract/gb2312.c \
	../script_binding/lua/lua_ftk_structs.c \
	../script_binding/lua/lua_ftk_callbacks.c \
	../script_binding/lua/lua_ftk_display.c \
	../script_binding/lua/lua_ftk_dir.c \
	../script_binding/lua/lua_ftk_list_render_default.c \
	../script_binding/lua/lua_ftk_window.c \
	../script_binding/lua/lua_ftk_button.c \
	../script_binding/lua/lua_ftk_check_button.c \
	../script_binding/lua/lua_ftk_typedef.c \
	../script_binding/lua/lua_ftk_source.c \
	../script_binding/lua/lua_ftk_progress_bar.c \
	../script_binding/lua/lua_ftk_combo_box.c \
	../script_binding/lua/lua_ftk_menu_item.c \
	../script_binding/lua/tolua_push.c \
	../script_binding/lua/lua_ftk_list_model_default.c \
	../script_binding/lua/lua_ftk_list_render.c \
	../script_binding/lua/ftk_lua.c \
	../script_binding/lua/lua_ftk_menu_panel.c \
	../script_binding/lua/lua_ftk_config.c \
	../script_binding/lua/lua_ftk_wait_box.c \
	../script_binding/lua/lua_ftk_source_timer.c \
	../script_binding/lua/lua_ftk_source_idle.c \
	../script_binding/lua/lua_ftk_entry.c \
	../script_binding/lua/lua_ftk_bitmap_factory.c \
	../script_binding/lua/tolua_event.c \
	../script_binding/lua/lua_ftk_label.c \
	../script_binding/lua/lua_ftk_fs.c \
	../script_binding/lua/lua_ftk_list_view.c \
	../script_binding/lua/lua_ftk_app_window.c \
	../script_binding/lua/lua_ftk_scroll_bar.c \
	../script_binding/lua/lua_ftk_dialog.c \
	../script_binding/lua/lua_ftk_painter.c \
	../script_binding/lua/lua_ftk_file_browser.c \
	../script_binding/lua/lua_ftk_xul.c \
	../script_binding/lua/lua_ftk_text_view.c \
	../script_binding/lua/lua_ftk.c \
	../script_binding/lua/lua_ftk_theme.c \
	../script_binding/lua/lua_ftk_group_box.c \
	../script_binding/lua/lua_ftk_file.c \
	../script_binding/lua/lua/lstrlib.c \
	../script_binding/lua/lua/loadlib.c \
	../script_binding/lua/lua/lauxlib.c \
	../script_binding/lua/lua/lparser.c \
	../script_binding/lua/lua/ldump.c \
	../script_binding/lua/lua/lgc.c \
	../script_binding/lua/lua/loslib.c \
	../script_binding/lua/lua/ltablib.c \
	../script_binding/lua/lua/lstring.c \
	../script_binding/lua/lua/lvm.c \
	../script_binding/lua/lua/ltable.c \
	../script_binding/lua/lua/luac.c \
	../script_binding/lua/lua/ltm.c \
	../script_binding/lua/lua/lopcodes.c \
	../script_binding/lua/lua/lapi.c \
	../script_binding/lua/lua/ldebug.c \
	../script_binding/lua/lua/llex.c \
	../script_binding/lua/lua/lobject.c \
	../script_binding/lua/lua/lmem.c \
	../script_binding/lua/lua/ldblib.c \
	../script_binding/lua/lua/lstate.c \
	../script_binding/lua/lua/print.c \
	../script_binding/lua/lua/lmathlib.c \
	../script_binding/lua/lua/lundump.c \
	../script_binding/lua/lua/linit.c \
	../script_binding/lua/lua/lbaselib.c \
	../script_binding/lua/lua/liolib.c \
	../script_binding/lua/lua/lzio.c \
	../script_binding/lua/lua/lua.c \
	../script_binding/lua/lua/lcode.c \
	../script_binding/lua/lua/ldo.c \
	../script_binding/lua/lua/lfunc.c \
	../script_binding/lua/lua_ftk_widget.c \
	../script_binding/lua/lua_ftk_wnd_manager.c \
	../script_binding/lua/tolua_is.c \
	../script_binding/lua/lua_ftk_status_panel.c \
	../script_binding/lua/tolua_map.c \
	../script_binding/lua/lua_ftk_clipboard.c \
	../script_binding/lua/lua_ftk_icon_cache.c \
	../script_binding/lua/lua_runner.c \
	../script_binding/lua/lua_ftk_translator.c \
	../script_binding/lua/lua_ftk_mmap.c \
	../script_binding/lua/lua_ftk_image.c \
	../script_binding/lua/lua_ftk_status_item.c \
	../script_binding/lua/lua_ftk_icon_view.c \
	../script_binding/lua/lua_ftk_list_model.c \
	../script_binding/lua/tolua_to.c \
	../script_binding/lua/lua_ftk_bitmap.c \
	../script_binding/lua/lua_ftk_sprite.c \
	../script_binding/lua/lua_ftk_popup_menu.c \
	../script_binding/lua/lua_ftk_main_loop.c

CATALOGS =  zh_CN.gmo
CATOBJEXT = .gmo
INSTOBJEXT = .mo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) $(MSGFMT_OPTS) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(srcdir)/$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
          --flag=g_strdup_printf:1:c-format \
          --flag=g_string_printf:2:c-format \
          --flag=g_string_append_printf:2:c-format \
          --flag=g_error_new:3:c-format \
          --flag=g_set_error:4:c-format \
          --flag=g_markup_printf_escaped:1:c-format \
          --flag=g_log:3:c-format \
          --flag=g_print:1:c-format \
          --flag=g_printerr:1:c-format \
          --flag=g_printf:1:c-format \
          --flag=g_fprintf:2:c-format \
          --flag=g_sprintf:2:c-format \
          --flag=g_snprintf:3:c-format \
          --flag=g_scanner_error:2:c-format \
          --flag=g_scanner_warn:2:c-format \
	  --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(GETTEXT_PACKAGE).po \
	   || ( rm -f $(srcdir)/$(GETTEXT_PACKAGE).pot \
		&& mv $(GETTEXT_PACKAGE).po $(srcdir)/$(GETTEXT_PACKAGE).pot )

install: install-exec install-data
install-exec:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	if test -r "$(MKINSTALLDIRS)"; then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(SHELL) $(top_srcdir)/install-sh -d $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(SHELL) $(top_srcdir)/install-sh -d $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(SHELL) $(top_srcdir)/install-sh -d $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(GETTEXT_PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(GETTEXT_PACKAGE)-$(VERSION)/$(subdir)
dist distdir: $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.po $(GETTEXT_PACKAGE).pot -o $$tmpdir/$$lang.new.po; then \
	    if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

# POTFILES is created from POTFILES.in by stripping comments, empty lines
# and Intltool tags (enclosed in square brackets), and appending a full
# relative path to them
POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' 						\
		    -e "s/^\[.*\] +//" 					\
		    -e '/^[ 	]*$$/d' 				\
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in	\
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/$@.in CONFIG_HEADERS= \
	       $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
