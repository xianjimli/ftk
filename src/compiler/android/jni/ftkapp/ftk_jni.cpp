
#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include "ftk.h"
#include "ftk_jni.h"
#include "ftk_log.h"
#include "ftk_key.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display.h"
#include "ftk_display_rotate.h"

#include "SkColor.h"
#include "SkCanvas.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkImageDecoder.h"
#include "effects/SkPorterDuff.h"

typedef struct _FtkJni
{
	JNIEnv* env;

	jclass activity;
	jmethodID get_asset_file_descriptor;
	jmethodID decode_image;
	jmethodID init_egl;
	jmethodID create_egl_surface;
	jmethodID destroy_egl_surface;
	jmethodID flip_egl;
	jmethodID draw_bitmap;
	jmethodID show_keyboard;
	jmethodID hide_keyboard;
	jmethodID show_input_method_picker;

	jmethodID get_file_descriptor;
	jmethodID get_start_offset;
	jmethodID get_length;
	jmethodID close_file_descriptor;

	jfieldID descriptor;

	jmethodID get_pixels;
	jmethodID get_width;
	jmethodID get_height;

	jintArray pixels;

	int render_enabled;
	int surface_valid;
	FtkEvent event;
}FtkJni;

FtkJni jni;

#if 0
FTK_KEY_EXCLAM
FTK_KEY_QUOTEDBL
FTK_KEY_NUMBERSIGN
FTK_KEY_DOLLAR
FTK_KEY_PERCENT
FTK_KEY_AMPERSAND
FTK_KEY_QUOTERIGHT
FTK_KEY_PARENLEFT
FTK_KEY_PARENRIGHT
FTK_KEY_ASTERISK
FTK_KEY_COLON
FTK_KEY_LESS
FTK_KEY_GREATER
FTK_KEY_QUESTION
FTK_KEY_ASCIICIRCUM
FTK_KEY_UNDERSCORE
FTK_KEY_QUOTELEFT
FTK_KEY_LEFTBRACE
FTK_KEY_OR
FTK_KEY_RIGHTBRACE
FTK_KEY_NOT
FTK_KEY_DOT
FTK_KEY_HOME
FTK_KEY_PRIOR
FTK_KEY_NEXT
FTK_KEY_END
FTK_KEY_BEGIN
FTK_KEY_INSERT
FTK_KEY_ESC
FTK_KEY_RIGHTCTRL
FTK_KEY_LEFTCTRL
FTK_KEY_CAPSLOCK
FTK_KEY_SEND
FTK_KEY_REPLY
FTK_KEY_SAVE
FTK_KEY_BATTERY
FTK_KEY_BLUETOOTH
FTK_KEY_WLAN
FTK_KEY_DELETE
#endif

static const int key_maps[] =
{
    /*KeyCodeUnknown[0] = */0,
    /*KeyCodeSoftLeft[1] = */0,
    /*KeyCodeSoftRight[2] = */0,
    /*KeyCodeHome[3] = */0,
    /*KeyCodeBack[4] = */0,//FTK_KEY_RETURN,
    /*KeyCodeCall[5] = */0,
    /*KeyCodeEndCall[6] = */0,
    /*KeyCode0[7] = */FTK_KEY_0,
    /*KeyCode1[8] = */FTK_KEY_1,
    /*KeyCode2[9] = */FTK_KEY_2,
    /*KeyCode3[10] = */FTK_KEY_3,
    /*KeyCode4[11] = */FTK_KEY_4,
    /*KeyCode5[12] = */FTK_KEY_5,
    /*KeyCode6[13] = */FTK_KEY_6,
    /*KeyCode7[14] = */FTK_KEY_7,
    /*KeyCode8[15] = */FTK_KEY_8,
    /*KeyCode9[16] = */FTK_KEY_9,
    /*KeyCodeStar[17] = */0,
    /*KeyCodePound[18] = */0,
    /*KeyCodeDpadUp[19] = */FTK_KEY_UP,
    /*KeyCodeDpadDown[20] = */FTK_KEY_DOWN,
    /*KeyCodeDpadLeft[21] = */FTK_KEY_LEFT,
    /*KeyCodeDpadRight[22] = */FTK_KEY_RIGHT,
    /*KeyCodeDpadCenter[23] = */0,
    /*KeyCodeVolumeUp[24] = */0,
    /*KeyCodeVolumeDown[25] = */0,
    /*KeyCodePower[26] = */FTK_KEY_POWER,
    /*KeyCodeCamera[27] = */0,
    /*KeyCodeClear[28] = */0,
    /*KeyCodeA[29] = */FTK_KEY_A,
    /*KeyCodeB[30] = */FTK_KEY_B,
    /*KeyCodeC[31] = */FTK_KEY_C,
    /*KeyCodeD[32] = */FTK_KEY_D,
    /*KeyCodeE[33] = */FTK_KEY_E,
    /*KeyCodeF[34] = */FTK_KEY_F,
    /*KeyCodeG[35] = */FTK_KEY_G,
    /*KeyCodeH[36] = */FTK_KEY_H,
    /*KeyCodeI[37] = */FTK_KEY_I,
    /*KeyCodeJ[38] = */FTK_KEY_J,
    /*KeyCodeK[39] = */FTK_KEY_K,
    /*KeyCodeL[40] = */FTK_KEY_L,
    /*KeyCodeM[41] = */FTK_KEY_M,
    /*KeyCodeN[42] = */FTK_KEY_N,
    /*KeyCodeO[43] = */FTK_KEY_O,
    /*KeyCodeP[44] = */FTK_KEY_P,
    /*KeyCodeQ[45] = */FTK_KEY_Q,
    /*KeyCodeR[46] = */FTK_KEY_R,
    /*KeyCodeS[47] = */FTK_KEY_S,
    /*KeyCodeT[48] = */FTK_KEY_T,
    /*KeyCodeU[49] = */FTK_KEY_U,
    /*KeyCodeV[50] = */FTK_KEY_V,
    /*KeyCodeW[51] = */FTK_KEY_W,
    /*KeyCodeX[52] = */FTK_KEY_X,
    /*KeyCodeY[53] = */FTK_KEY_Y,
    /*KeyCodeZ[54] = */FTK_KEY_Z,
    /*KeyCodeComma[55] = */FTK_KEY_COMMA,
    /*KeyCodePeriod[56] = */FTK_KEY_PERIOD,
    /*KeyCodeAltLeft[57] = */FTK_KEY_LEFTALT,
    /*KeyCodeAltRight[58] = */FTK_KEY_RIGHTALT,
    /*KeyCodeShiftLeft[59] = */FTK_KEY_LEFTSHIFT,
    /*KeyCodeShiftRight[60] = */FTK_KEY_RIGHTSHIFT,
    /*KeyCodeTab[61] = */FTK_KEY_TAB,
    /*KeyCodeSpace[62] = */FTK_KEY_SPACE,
    /*KeyCodeSym[63] = */0,
    /*KeyCodeExplorer[64] = */0,
    /*KeyCodeEnvelope[65] = */0,
    /*KeyCodeNewline[66] = */0,
    /*KeyCodeDel[67] = */FTK_KEY_BACKSPACE,
    /*KeyCodeGrave[68] = */FTK_KEY_GRAVE,
    /*KeyCodeMinus[69] = */FTK_KEY_MINUS,
    /*KeyCodeEquals[70] = */FTK_KEY_EQUAL,
    /*KeyCodeLeftBracket[71] = */FTK_KEY_BRACKETLEFT,
    /*KeyCodeRightBracket[72] = */FTK_KEY_BRACKETRIGHT,
    /*KeyCodeBackslash[73] = */FTK_KEY_BACKSLASH,
    /*KeyCodeSemicolon[74] = */FTK_KEY_SEMICOLON,
    /*KeyCodeApostrophe[75] = */FTK_KEY_APOSTROPHE,
    /*KeyCodeSlash[76] = */FTK_KEY_SLASH,
    /*KeyCodeAt[77] = */FTK_KEY_AT,
    /*KeyCodeNum[78] = */0,
    /*KeyCodeHeadSetHook[79] = */0,
    /*KeyCodeFocus[80] = */0,
    /*KeyCodePlus[81] = */FTK_KEY_PLUS,
    /*KeyCodeMenu[82] = */FTK_KEY_MENU,
    /*KeyCodeNotification[83] = */0,
    /*KeyCodeSearch[84] = */0,
    /*KeyCodePlayPause[85] = */0,
    /*KeyCodeStop[86] = */0,
    /*KeyCodeNextSong[87] = */0,
    /*KeyCodePreviousSong[88] = */0,
    /*KeyCodeRewind[89] = */0,
    /*KeyCodeForward[90] = */0,
    /*KeyCodeMute[91] = */0,
    /*KeyCodePageUp[92] = */FTK_KEY_PAGEUP,
    /*KeyCodePageDown[93] = */FTK_KEY_PAGEDOWN,
    /*KeyCodePictSymbols[94] = */0,
    /*KeyCodeSwitchCharset[95] = */0
};

/******************************************************************************/

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;

	ftk_logd("JNI: OnLoad()");

	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		return -1;
	}

	jni.env = env;

	jclass cls = jni.env->FindClass("org/libftk/app/FtkActivity");
	jni.activity = (jclass)jni.env->NewGlobalRef(cls);

	jni.get_asset_file_descriptor = jni.env->GetStaticMethodID(cls, "getAssetFileDescriptor",
		"(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
	jni.decode_image = jni.env->GetStaticMethodID(cls, "decodeImage", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");

	jni.init_egl = jni.env->GetStaticMethodID(cls, "initEGL", "()V");
	jni.create_egl_surface = jni.env->GetStaticMethodID(cls, "createEGLSurface", "()V");
	jni.destroy_egl_surface = jni.env->GetStaticMethodID(cls, "destroyEGLSurface", "()V");
	jni.flip_egl = jni.env->GetStaticMethodID(cls, "flipEGL", "()V");
	jni.draw_bitmap = jni.env->GetStaticMethodID(cls, "drawBitmap", "([IIIIIII)V");

	jni.show_keyboard = jni.env->GetStaticMethodID(cls, "showKeyboard", "()V");
	jni.hide_keyboard = jni.env->GetStaticMethodID(cls, "hideKeyboard", "()V");
	jni.show_input_method_picker = jni.env->GetStaticMethodID(cls, "showInputMethodPicker", "()V");

	cls = jni.env->FindClass("android/content/res/AssetFileDescriptor");
	jni.get_file_descriptor = jni.env->GetMethodID(cls, "getFileDescriptor", "()Ljava/io/FileDescriptor;");
	jni.get_start_offset = jni.env->GetMethodID(cls, "getStartOffset", "()J");
	jni.get_length = jni.env->GetMethodID(cls, "getLength", "()J");
	jni.close_file_descriptor = jni.env->GetMethodID(cls, "close", "()V");

	cls = jni.env->FindClass("java/io/FileDescriptor");
	jni.descriptor = jni.env->GetFieldID(cls, "descriptor", "I");

	cls = jni.env->FindClass("android/graphics/Bitmap");
	jni.get_pixels = jni.env->GetMethodID(cls, "getPixels", "([IIIIIII)V");
	jni.get_width = jni.env->GetMethodID(cls, "getWidth", "()I");
	jni.get_height = jni.env->GetMethodID(cls, "getHeight", "()I");

	if(!jni.get_asset_file_descriptor || !jni.decode_image
		|| !jni.init_egl || !jni.create_egl_surface || !jni.destroy_egl_surface || !jni.flip_egl || !jni.draw_bitmap
		|| !jni.show_keyboard || !jni.hide_keyboard || !jni.show_input_method_picker
		|| !jni.get_file_descriptor || !jni.get_start_offset || !jni.get_length || !jni.close_file_descriptor
		|| !jni.descriptor
		|| !jni.get_pixels || !jni.get_width || !jni.get_height)
	{
		ftk_logd("null method or field id");
		return -1;
	}

	return JNI_VERSION_1_4;
}

extern "C" void Java_org_libftk_app_FtkActivity_nativeInit(JNIEnv* env, jobject obj)
{
	int argc, rv;
	char* argv[2];

	ftk_logd("native init");

	jni.env = env;

	argv[0] = (char *) "ftk";
	argv[1] = NULL;
	argc = 1;
	rv = FTK_MAIN(argc, argv);

	env->DeleteGlobalRef(jni.pixels);
	env->DeleteGlobalRef(jni.activity);
}

extern "C" void Java_org_libftk_app_FtkActivity_nativeQuit(JNIEnv* env, jobject obj)
{
	FTK_QUIT();
	ftk_logd("native quit");
}

extern "C" void Java_org_libftk_app_FtkActivity_nativeEnableRender(JNIEnv* env, jobject obj)
{
	jni.render_enabled = 1;
}

extern "C" void Java_org_libftk_app_FtkActivity_nativeDisableRender(JNIEnv* env, jobject obj)
{
	jni.render_enabled = 0;
	jni.surface_valid = 0;
}

extern "C" void Java_org_libftk_app_FtkActivity_onNativeKey(JNIEnv* env, jobject obj, jint action, jint keyCode)
{
	ftk_logd("native key action:%d keyCode:%d", action, keyCode);

	if(action == 2)
	{
		return;
	}
	jni.event.type = action == 0 ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
	jni.event.u.key.code = key_maps[keyCode];
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &jni.event);
}

extern "C" void Java_org_libftk_app_FtkActivity_onNativeCommitText(JNIEnv* env, jobject obj, jstring text, jint newCursorPosition)
{
	char* buf;
	jsize len;
	const char *str;

	len = env->GetStringUTFLength(text);
	str = env->GetStringUTFChars(text, NULL);
	buf = (char*)FTK_ALLOC(len + 1);
	memcpy(buf, str, len);
	buf[len] = '\0';
	env->ReleaseStringUTFChars(text, str);

	memset(&jni.event, 0, sizeof(FtkEvent));
	jni.event.u.extra = buf;
	jni.event.type = FTK_EVT_OS_IM_COMMIT;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &jni.event);
}

extern "C" void Java_org_libftk_app_FtkActivity_onNativeTouch(JNIEnv* env, jobject obj, jint action, jfloat x, jfloat y, jfloat p)
{
	ftk_logd("native touch event %d @ %f/%f, pressure %f", action, x, y, p);
	memset(&jni.event, 0, sizeof(FtkEvent));
	if(action==0)
	{
		jni.event.type = FTK_EVT_MOUSE_DOWN;
	}
	else if(action == 1 || action == 3)
	{
		jni.event.type = FTK_EVT_MOUSE_UP;
	}
	else if(action == 2)
	{
		jni.event.type = FTK_EVT_MOUSE_MOVE;
	}
	else
	{
		return;
	}
	jni.event.u.mouse.x = x;
	jni.event.u.mouse.y = y;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &jni.event);
}

extern "C" void Java_org_libftk_app_FtkActivity_nativeSetScreenSize(JNIEnv* env, jobject obj, jint width, jint height)
{
	ftk_logd("nativeSetScreenSize() %dX%d", width, height);
	Android_SetScreenResolution(width, height);

	jintArray temp = (jintArray)env->NewIntArray(width * height);
	jni.pixels = (jintArray)env->NewGlobalRef(temp);
	env->DeleteLocalRef(temp);
}

extern "C" void Java_org_libftk_app_FtkActivity_onNativeResize(JNIEnv* env, jobject obj, jint width, jint height, jint format)
{
	int w, h;
	FtkDisplay* display = NULL;
	FtkRotate r = FTK_ROTATE_0;

	display = ftk_default_display();
	r = ftk_display_get_rotate(display);
	w = ftk_display_width(display);
	h = ftk_display_height(display);

	ftk_logd("onNativeResize() old size:%dX%d new size:%dX%d format:%d", w, h, width, height, format);

	if(width != w && height != h)
	{
		memset(&jni.event, 0, sizeof(FtkEvent));
		if(r == FTK_ROTATE_0)
		{
			jni.event.u.extra = (void*)FTK_ROTATE_90;
		}
		else
		{
			jni.event.u.extra = (void*)FTK_ROTATE_0;
		}
		jni.event.type = FTK_EVT_OS_SCREEN_ROTATED;
		//ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &jni.event);
	}

	memset(&jni.event, 0, sizeof(FtkEvent));
	jni.event.type = FTK_EVT_RELAYOUT_WND;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &jni.event);
}

/******************************************************************************/

void Android_Log(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	__android_log_vprint(ANDROID_LOG_INFO, "FTK", fmt, ap);
	va_end(ap);
}

int Android_OpenAsset(const char* filename, size_t* size)
{
	int fd;
	jlong asset_offset, asset_size;
	jstring file;
	jobject asset_fd, descriptor;

	//ftk_logd("%s", filename);

	file = jni.env->NewStringUTF(filename);
	asset_fd = jni.env->CallStaticObjectMethod(jni.activity, jni.get_asset_file_descriptor, file);
	jni.env->DeleteLocalRef(file);

	descriptor = jni.env->CallObjectMethod(asset_fd, jni.get_file_descriptor);
	asset_offset = jni.env->CallLongMethod(asset_fd, jni.get_start_offset);
	asset_size = jni.env->CallLongMethod(asset_fd, jni.get_length);

	ftk_logd("%s %d", filename, (int)asset_size);

	fd = dup(jni.env->GetIntField(descriptor, jni.descriptor));

	jni.env->CallVoidMethod(asset_fd, jni.close_file_descriptor);
	jni.env->DeleteLocalRef(descriptor);
	jni.env->DeleteLocalRef(asset_fd);

	lseek(fd, (int)asset_offset, SEEK_SET);

	*size = asset_size;

	return fd;
}

#if 0
FtkBitmap* Android_LoadImage(const char* filename)
{
	jstring file;
	jobject image;
	jintArray temp;
	jint* pixels = NULL;
	SkBitmap* bmp = NULL;
	FtkBitmap* bitmap = NULL;
	jint width = 0, height = 0;
	SkColor* new_pixels = NULL;
   	const char* assert_prefix = "/assets/";

   	return_val_if_fail(filename != NULL, NULL);

	ftk_logi("Android_LoadImage: %s\n", filename);
   	if(strncmp(filename, assert_prefix, strlen(assert_prefix)) == 0)
   	{
   		filename += strlen(assert_prefix);
   	}

	file = jni.env->NewStringUTF(filename);
	image = jni.env->CallStaticObjectMethod(jni.activity, jni.decode_image, file);
	jni.env->DeleteLocalRef(file);

	return_val_if_fail(image != NULL, NULL);

	width = jni.env->CallIntMethod(image, jni.get_width);
	height = jni.env->CallIntMethod(image, jni.get_height);

	ftk_logi("%s: %dx%d", filename, width, height);

	temp = jni.env->NewIntArray(width * height);
	jni.env->CallVoidMethod(image, jni.get_pixels, temp, 0, width, 0, 0, width, height);
	pixels = jni.env->GetIntArrayElements(temp, NULL);

	size_t length = sizeof(SkColor) * width * height;

	new_pixels = (SkColor*)malloc(length);

	if(new_pixels != NULL)
	{
		bmp = new SkBitmap();
		memcpy(new_pixels, pixels, length);
		bmp->setConfig(SkBitmap::kARGB_8888_Config, width, height, 0);
		bmp->setPixels(new_pixels);
		bitmap = ftk_bitmap_create_with_native(bmp);
	}

	jni.env->ReleaseIntArrayElements(temp, pixels, 0);
	jni.env->DeleteLocalRef(temp);
	jni.env->DeleteLocalRef(image);

	return bitmap;
}
#else
extern "C" SkImageDecoder* PNG_DFactory(SkStream* stream);

static SkStream* FDStreamToMemStream(SkStream* stream, size_t size)
{
	char* buffer = (char*)malloc(size);
	SkMemoryStream* s = new SkMemoryStream();
	size_t ret = stream->read(buffer, size);
	s->setMemory(buffer, size, false);
	s->rewind();
	free(buffer);

	return s; 
}

static SkBitmap* doDecode(JNIEnv* env, SkStream* stream, size_t size)
{
	char* buffer = (char*)malloc(size);
	SkMemoryStream* s = new SkMemoryStream();
	size_t ret = stream->read(buffer, size);
	s->setMemory(buffer, ret, false);
	s->rewind();

	SkImageDecoder* decoder = PNG_DFactory(s);
	return_val_if_fail(decoder != NULL, NULL);
	
    SkAutoUnref aurs(s);
	SkAutoTDelete<SkImageDecoder> ad(decoder);
	
	s->rewind();
	SkBitmap*       bitmap = new SkBitmap;
	if (!decoder->decode(s, bitmap, SkBitmap::kARGB_8888_Config, SkImageDecoder::kDecodePixels_Mode)) 
	{
		delete bitmap;
		bitmap = NULL;
    }
	free(buffer);

	return bitmap;
}

FtkBitmap* Android_LoadImage(const char* filename)
{
	int i = 0;
   	size_t size = 0; 
   	int descriptor = 0;
	SkBitmap* bmp = NULL;
	JNIEnv* env = jni.env;
	FtkBitmap* bitmap = NULL;
	char buffer[16] = {0};
   	const char* assert_prefix = "/assets/";

   	return_val_if_fail(filename != NULL, NULL);

   	if(strncmp(filename, assert_prefix, strlen(assert_prefix)) == 0)
   	{
   		filename += strlen(assert_prefix);
   	}

    descriptor = Android_OpenAsset(filename, &size);
	return_val_if_fail(descriptor >= 0, NULL);

	ftk_logi("%s:%d open %s descriptor=%d size=%d\n", __func__, __LINE__, filename, descriptor, size);
    SkFDStream* stream = new SkFDStream(descriptor, false);
    SkAutoUnref aur(stream);
    return_val_if_fail(stream->isValid(), NULL);
	
	bmp = doDecode(env, stream, size);
	ftk_logi("%s:%d bmp=%p\n", __func__, __LINE__, bmp);

	if(bmp != NULL)
	{
		bitmap = ftk_bitmap_create_with_native(bmp);
	}

	return bitmap;
}
#endif

void Android_InitEGL()
{
	ftk_logd("Android_InitEGL()");
	jni.env->CallStaticVoidMethod(jni.activity, jni.init_egl);
}

Ret Android_PreRender()
{
	if(!jni.render_enabled)
	{
		if(jni.surface_valid)
		{
			jni.env->CallStaticVoidMethod(jni.activity, jni.destroy_egl_surface);
		}
		return RET_FAIL;
	}
	if(!jni.surface_valid)
	{
		jni.env->CallStaticVoidMethod(jni.activity, jni.create_egl_surface);
		jni.surface_valid = 1;
	}
	return RET_OK;
}

void Android_Render()
{
	if(!jni.render_enabled || !jni.surface_valid)
	{
		return;
	}
	jni.env->CallStaticVoidMethod(jni.activity, jni.flip_egl);
}

int* Android_GetBitmapPixels()
{
	return (int*)jni.env->GetIntArrayElements(jni.pixels, NULL);
}

void Android_ReleaseBitmapPixels(int* pixels)
{
	jni.env->ReleaseIntArrayElements(jni.pixels, pixels, 0);
}

void Android_DrawBitmap(int offset, int stride, int x, int y, int width, int height)
{
	if(!jni.render_enabled)
	{
		return;
	}

	jintArray temp = (jintArray)jni.env->NewLocalRef(jni.pixels);
	jni.env->CallStaticVoidMethod(jni.activity, jni.draw_bitmap, temp, offset, stride, x, y, width, height);
	jni.env->DeleteLocalRef(temp); // TODO: xxx
}

void Android_ShowKeyboard()
{
	ftk_logd("Android_ShowKeyboard()");
	jni.env->CallStaticVoidMethod(jni.activity, jni.show_keyboard);
}

void Android_HideKeyboard()
{
	ftk_logd("Android_HideKeyboard()");
	jni.env->CallStaticVoidMethod(jni.activity, jni.hide_keyboard);
}

void Android_ShowInputMethodChooser()
{
	ftk_logd("Android_ShowInputMethodChooser()");
	jni.env->CallStaticVoidMethod(jni.activity, jni.show_input_method_picker);
}
