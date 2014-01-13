LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libftk

#-DUSE_LINEBREAK

LOCAL_CFLAGS :=	\
	-O2 \
	-DANDROID_NDK \
	-DFTK_PROFILE -DHAVE_ENDIAN_H \
	-DFTK_FONT=\"/system/fonts/DroidSansFallback.ttf\" \
	-DDATA_DIR=\"/assets\" \
	-DLOCAL_DATA_DIR=\"./local_data\" \
	-DFTK_DATA_ROOT=\"/assets\" \
	-DTESTDATA_DIR=\"/assets/testdata\" -DPNG_ERROR_TEXT_SUPPORTED\
	-I$(LOCAL_PATH)/../../../../../src \
	-I$(LOCAL_PATH)/../../../../../src/im \
	-I$(LOCAL_PATH)/../../../../../src/backend/android \
	-I$(LOCAL_PATH)/../../../../../src/os/android \
	-I$(EXT_INCLUDE_DIR)/skia/include \
	-I$(EXT_INCLUDE_DIR)/skia/include/core \
	-I$(EXT_INCLUDE_DIR)/skia/include/images \
    -I$(EXT_INCLUDE_DIR)/system/core/include \
    -I$(EXT_INCLUDE_DIR)/frameworks/base/include 

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../ftkapp  $(JNI_H_INCLUDE)

LOCAL_SRC_FILES := \
	../../../..//canvas/skia/ftk_bitmap_factory_skia.cpp \
	../../../..//canvas/skia/ftk_bitmap_skia.cpp \
	../../../..//canvas/skia/ftk_canvas_skia.cpp \
	../../../..//ftk_allocator.c \
	../../../..//ftk_allocator_default.c \
	../../../..//ftk_allocator_profile.c \
	../../../..//ftk_animation_alpha.c \
	../../../..//ftk_animation.c \
	../../../..//ftk_animation_expand.c \
	../../../..//ftk_animation_scale.c \
	../../../..//ftk_animation_translate.c \
	../../../..//ftk_animation_trigger_default.c \
	../../../..//ftk_animation_trigger_silence.c \
	../../../..//ftk_app_window.c \
	../../../..//ftk_bitmap.c \
	../../../..//ftk_button.c \
	../../../..//ftk.c \
	../../../..//ftk_canvas.c \
	../../../..//ftk_check_button.c \
	../../../..//ftk_clipboard.c \
	../../../..//ftk_combo_box.c \
	../../../..//ftk_config.c \
	../../../..//ftk_dialog.c \
	../../../..//ftk_display.c \
	../../../..//ftk_display_mem.c \
	../../../..//ftk_display_rotate.c \
	../../../..//ftk_dlfcn.c \
	../../../..//ftk_entry.c \
	../../../..//ftk_expr.c \
	../../../..//ftk_file_browser.c \
	../../../..//ftk_file_system.c \
	../../../..//ftk_file_system_posix.c \
	../../../..//ftk_font_desc.c \
	../../../..//ftk_gesture.c \
	../../../..//ftk_globals.c \
	../../../..//ftk_group_box.c \
	../../../..//ftk_icon_cache.c \
	../../../..//ftk_icon_view.c \
	../../../..//ftk_image.c \
	../../../..//ftk_input_method_chooser_default.c \
	../../../..//ftk_input_method_manager.c \
	../../../..//ftk_input_method_preeditor.c \
	../../../..//ftk_input_method_preeditor_default.c \
	../../../..//ftk_input_pattern.c \
	../../../..//ftk_interpolator_acc_decelerate.c \
	../../../..//ftk_interpolator_accelerate.c \
	../../../..//ftk_interpolator_bounce.c \
	../../../..//ftk_interpolator_decelerate.c \
	../../../..//ftk_interpolator_linear.c \
	../../../..//ftk_key_board.c \
	../../../..//ftk_label.c \
	../../../..//ftk_list_model_default.c \
	../../../..//ftk_list_render_default.c \
	../../../..//ftk_list_view.c \
	../../../..//ftk_log.c \
	../../../..//ftk_main_loop.c \
	../../../..//ftk_main_loop_select.c \
	../../../..//ftk_menu_item.c \
	../../../..//ftk_menu_panel.c \
	../../../..//ftk_message_box.c \
	../../../..//ftk_painter.c \
	../../../..//ftk_pairs.c \
	../../../..//ftk_params.c \
	../../../..//ftk_path.c \
	../../../..//ftk_pipe_socket.c \
	../../../..//ftk_popup_menu.c \
	../../../..//ftk_progress_bar.c \
	../../../..//ftk_scroll_bar.c \
	../../../..//ftk_source_idle.c \
	../../../..//ftk_source_primary.c \
	../../../..//ftk_sources_manager.c \
	../../../..//ftk_source_timer.c \
	../../../..//ftk_sprite.c \
	../../../..//ftk_status_item.c \
	../../../..//ftk_status_panel.c \
	../../../..//ftk_tab.c \
	../../../..//ftk_text_buffer.c \
	../../../..//ftk_text_layout_normal.c \
	../../../..//ftk_text_view.c \
	../../../..//ftk_theme.c \
	../../../..//ftk_translator.c \
	../../../..//ftk_util.c \
	../../../..//ftk_wait_box.c \
	../../../..//ftk_widget.c \
	../../../..//ftk_window.c \
	../../../..//ftk_wnd_manager.c \
	../../../..//ftk_wnd_manager_default.c \
	../../../..//ftk_xml_parser.c \
	../../../..//ftk_xul.c \
	./SkImageDecoder_libpng.cpp\
	png.c \
	pngerror.c \
	pnggccrd.c \
	pngget.c \
	pngmem.c \
	pngpread.c \
	pngread.c \
	pngrio.c \
	pngrtran.c \
	pngrutil.c \
	pngset.c \
	pngtrans.c \
	pngvcrd.c \
	pngwio.c \
	pngwrite.c \
	pngwtran.c \
	pngwutil.c\
	../../../..//backend/android/ftk_backend_android.c \
	../../../..//backend/android/ftk_display_android_skia.c \
	../../../..//backend/android/ftk_source_android.c \
	../../../..//os/android/ftk_android.c \
	../../../..//os/android/ftk_image_android_decoder.c \
	../../../..//os/android/ftk_input_method_android.c \
	../../../..//os/android/ftk_input_method_chooser_android.c \
	../../../..//os/android/ftk_mmap_android.c

include $(BUILD_STATIC_LIBRARY)
