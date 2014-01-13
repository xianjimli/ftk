
#include "ftk_typedef.h"
#include "ftk.h"
#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_wince.h"
#include "ftk_input_method_manager.h"
#include "ftk_input_method_wince.h"
#include "GLES/egl.h"
#include "GLES/gl.h"
#include <aygshell.h>

#define FTK_EGL_FUNC(ret, func, params) ret (WINAPI *func) params;
#define FTK_GLES_FUNC(ret, func, params) ret (WINAPI *func) params;
#include "ftk_gles_funcs.h"
#undef FTK_EGL_FUNC
#undef FTK_GLES_FUNC

typedef struct _PrivInfo
{
	HWND wnd;
	HDC dc;
	HANDLE egl_dll;
	HANDLE gles_dll;
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
	EGLConfig config;
	int width;
	int height;
	int screen_width;
	int screen_height;
	void* bits;
	FtkEvent event;
}PrivInfo;

static int s_key_map[0xff] = {0};
static wchar_t szClassName[] = L"FtkEmulator";

static Ret ftk_display_wince_gles_get_funcs(PrivInfo* priv)
{
#define FTK_EGL_FUNC(ret, func, params) \
	func = (ret (WINAPI *) params)GetProcAddress(priv->egl_dll, L#func); \
	if(func == NULL) \
	{ \
		ftk_loge("GetProcAddress(\"%s\") failed\n", #func); \
		return RET_FAIL; \
	}
#define FTK_GLES_FUNC(ret, func, params) \
	func = (ret (WINAPI *) params)GetProcAddress(priv->gles_dll, L#func); \
	if(func == NULL) \
	{ \
		ftk_loge("GetProcAddress(\"%s\") failed\n", #func); \
		return RET_FAIL; \
	}
#include "ftk_gles_funcs.h"
#undef FTK_EGL_FUNC
#undef FTK_GLES_FUNC
	return RET_OK;
}

static void ftk_display_wince_gles_set_fullscreen(PrivInfo* priv)
{
	SHFullScreen(priv->wnd, SHFS_HIDESIPBUTTON|SHFS_HIDETASKBAR|SHFS_HIDESTARTICON);
	SetWindowPos(priv->wnd, HWND_TOPMOST, 0, 0, priv->screen_width, priv->screen_height, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);
}

static void ftk_display_wince_gles_keymap_init(void)
{
	s_key_map[0xBD] = FTK_KEY_MINUS;
	s_key_map[0x20] = FTK_KEY_SPACE;
	s_key_map[0xBB] = FTK_KEY_EQUAL;
	s_key_map[0x08] = FTK_KEY_BACKSPACE;
	s_key_map[0x09] = FTK_KEY_TAB;
	s_key_map[0xC0] = FTK_KEY_QUOTELEFT;
	s_key_map[0xDE] = FTK_KEY_QUOTERIGHT;
	s_key_map[0xDB] = FTK_KEY_BRACKETLEFT;
	s_key_map[0xDD] = FTK_KEY_BRACKETRIGHT;
	s_key_map[0x0d] = FTK_KEY_ENTER;
	s_key_map[0xBA] = FTK_KEY_SEMICOLON;
	s_key_map[0x30] = FTK_KEY_0;
	s_key_map[0x31] = FTK_KEY_1; 
	s_key_map[0x32] = FTK_KEY_2;
	s_key_map[0x33] = FTK_KEY_3;
	s_key_map[0x34] = FTK_KEY_4;
	s_key_map[0x35] = FTK_KEY_5;
	s_key_map[0x36] = FTK_KEY_6;
	s_key_map[0x37] = FTK_KEY_7;
	s_key_map[0x38] = FTK_KEY_8;
	s_key_map[0x39] = FTK_KEY_9;
	s_key_map[0x70] = FTK_KEY_F1; 
	s_key_map[0x71] = FTK_KEY_F2;
	s_key_map[0x72] = FTK_KEY_F3;
	s_key_map[0x73] = FTK_KEY_F4;
	s_key_map[0x74] = FTK_KEY_F5;
	s_key_map[0x75] = FTK_KEY_F6;
	s_key_map[0x76] = FTK_KEY_F7;
	s_key_map[0x77] = FTK_KEY_F8;
	s_key_map[0x78] = FTK_KEY_F9;
	s_key_map[0xBC] = FTK_KEY_COMMA;
	s_key_map[0xBE] = FTK_KEY_DOT;
	s_key_map[0xBF] = FTK_KEY_SLASH;
	s_key_map[0x10] = FTK_KEY_RIGHTSHIFT;
	s_key_map[0x11] = FTK_KEY_LEFTCTRL;
	s_key_map[0x14] = FTK_KEY_CAPSLOCK;
	s_key_map[0x40] = FTK_KEY_LEFTALT;
	s_key_map[0x41] = FTK_KEY_a;
	s_key_map[0x42] = FTK_KEY_b;
	s_key_map[0x43] = FTK_KEY_c;
	s_key_map[0x44] = FTK_KEY_d;
	s_key_map[0x45] = FTK_KEY_e;
	s_key_map[0x46] = FTK_KEY_f;
	s_key_map[0x47] = FTK_KEY_g;
	s_key_map[0x48] = FTK_KEY_h;
	s_key_map[0x49] = FTK_KEY_i;
	s_key_map[0x4a] = FTK_KEY_j;
	s_key_map[0x4b] = FTK_KEY_k;
	s_key_map[0x4c] = FTK_KEY_l;
	s_key_map[0x4d] = FTK_KEY_m;
	s_key_map[0x4e] = FTK_KEY_n;
	s_key_map[0x4f] = FTK_KEY_o;
	s_key_map[0x50] = FTK_KEY_p;
	s_key_map[0x51] = FTK_KEY_q;
	s_key_map[0x52] = FTK_KEY_r;
	s_key_map[0x53] = FTK_KEY_s;
	s_key_map[0x54] = FTK_KEY_t;
	s_key_map[0x55] = FTK_KEY_u;
	s_key_map[0x56] = FTK_KEY_v;
	s_key_map[0x57] = FTK_KEY_w;
	s_key_map[0x58] = FTK_KEY_x;
	s_key_map[0x59] = FTK_KEY_y;
	s_key_map[0x5a] = FTK_KEY_z;
	s_key_map[0x21] = FTK_KEY_PAGEUP;
	s_key_map[0x25] = FTK_KEY_LEFT;
	s_key_map[0x27] = FTK_KEY_RIGHT;
	s_key_map[0x24] = FTK_KEY_HOME;
	s_key_map[0x23] = FTK_KEY_END;
	s_key_map[0x28] = FTK_KEY_DOWN;
	s_key_map[0x22] = FTK_KEY_PAGEDOWN;
	s_key_map[0x2d] = FTK_KEY_INSERT;
	s_key_map[0x2e] = FTK_KEY_DELETE;
	s_key_map[0x26] = FTK_KEY_UP;
}

static char* ftk_display_wince_gles_on_ime(HWND hwnd, LPARAM lParam)
{
	LONG n;
	wchar_t* unicode;
	char* utf8;
	HIMC himc;
	FtkInputMethod* im = NULL;

	if(!(lParam & GCS_RESULTSTR))
	{
		return NULL;
	}
	if(ftk_input_method_manager_get_current(ftk_default_input_method_manager(), &im) == RET_FAIL || im == NULL)
	{
		return NULL;
	}
	if(ftk_input_method_wince_get_editor(im) == NULL)
	{
		return NULL;
	}

	himc = ImmGetContext(hwnd);
	if((void*)himc == NULL)
	{
		return NULL;
	}
	n = ImmGetCompositionStringW(himc, GCS_RESULTSTR, NULL, 0);
	if(n <= 0)
	{
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	unicode = (wchar_t*)FTK_ALLOC(n + 2);
	if(unicode == NULL)
	{
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	if(ImmGetCompositionStringW(himc, GCS_RESULTSTR, unicode, n) != n)
	{
		FTK_FREE(unicode);
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	ImmReleaseContext(hwnd, himc);

	ftk_logd("%s:%d WM_IME_COMPOSITION:%s\n", __FILE__, __LINE__, unicode);
	unicode[n / 2] = L'\0';
	ftk_logd("%s:%d WM_IME_COMPOSITION:%s\n", __FILE__, __LINE__, unicode);
	n = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	if(n < 1)
	{
		FTK_FREE(unicode);
		return NULL;
	}
	utf8 = (char*) FTK_ALLOC(n);
	if(utf8 == NULL)
	{
		FTK_FREE(unicode);
		return NULL;
	}
	n = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, n, NULL, NULL);
	if(n < 1)
	{
		FTK_FREE(unicode);
		FTK_FREE(utf8);
		return NULL;
	}

	FTK_FREE(unicode);

	ftk_logd("%s:%d WM_IME_COMPOSITION:%s\n", __FILE__, __LINE__, utf8);

	return utf8;
}

static void ftk_display_wince_gles_on_paint(PrivInfo* priv)
{
	GLfloat	coordinates[] = { 0, priv->screen_height / (float)priv->height,
				  priv->screen_width / (float)priv->width, priv->screen_height / (float)priv->height,
				  0, 0,
				  priv->screen_width / (float)priv->width, 0 };
	GLfloat	vertices[] = {
		0.0f,				0.0f,	0.0,
		0.0f + priv->screen_width,	0.0f,	0.0,
		0.0f,				0.0f + priv->screen_height, 0.0,
		0.0f + priv->screen_width,	0.0f + priv->screen_height, 0.0 };

	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, priv->width, priv->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, priv->bits);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//eglWaitNative(EGL_NATIVE_RENDERABLE);
	//eglWaitGL();
	eglSwapBuffers(priv->display, priv->surface);
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char* buf = NULL;
	HDC dc;
	PAINTSTRUCT ps;
	PrivInfo* priv = (PrivInfo*)GetWindowLongW(hwnd, GWL_USERDATA);

	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			FTK_QUIT();
			break;
		//case WM_ACTIVATE:
		//	set_fullscreen(priv);
		//	break;
		case WM_PAINT:
			dc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			if(priv != NULL)
			{
				ftk_display_wince_gles_on_paint(priv);
			}
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			priv->event.type = message == WM_KEYDOWN ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
			priv->event.u.key.code = s_key_map[0xFF & wParam];
			break;
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
			priv->event.u.mouse.x = LOWORD(lParam);
			priv->event.u.mouse.y = HIWORD(lParam);
			priv->event.type = message == WM_LBUTTONUP? FTK_EVT_MOUSE_UP : FTK_EVT_MOUSE_DOWN;
			break;
		case WM_MOUSEMOVE:
			priv->event.u.mouse.x = LOWORD(lParam);
			priv->event.u.mouse.y = HIWORD(lParam);
			priv->event.type = FTK_EVT_MOUSE_MOVE;
			break;
		case WM_IME_COMPOSITION:
			buf = ftk_display_wince_gles_on_ime(hwnd, lParam);
			if(buf == NULL)
			{
				return DefWindowProcW(hwnd, message, wParam, lParam);
			}
			priv->event.u.extra = buf;
			priv->event.type = FTK_EVT_OS_IM_COMMIT;
			break;
		default:
			return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	if(priv->event.type != FTK_EVT_NOP)
	{
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
		priv->event.type = FTK_EVT_NOP;
	}

	return 0;
}

static HWND ftk_display_wince_gles_create_win(PrivInfo* priv)
{
	HWND hwnd;
	WNDCLASSW wincl;

	wincl.hInstance = NULL;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WinProc;
	wincl.style = CS_DBLCLKS;
	wincl.hIcon = NULL;
	wincl.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 64;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if (!RegisterClassW(&wincl))
	{
		return NULL;
	}

	hwnd = CreateWindowExW(0, szClassName, L"FtkEmulator", WS_DLGFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, priv->width, priv->height,
		HWND_DESKTOP, NULL, NULL, NULL);

	return hwnd;
}

static Ret ftk_display_wince_gles_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);

	ret = ftk_bitmap_copy_to_data_rgba32(bitmap, rect, priv->bits, xoffset, yoffset, priv->width, priv->height);

	ftk_display_wince_gles_on_paint(priv);

	return ret;
}

static int ftk_display_wince_gles_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->screen_width;
}

static int ftk_display_wince_gles_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->screen_height;
}

static Ret ftk_display_wince_gles_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
#if 0
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = ftk_display_width(thiz);
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);

	rect.x = r->x;
	rect.y = r->y;
	rect.width = FTK_MIN(bw, r->width);
	rect.height = FTK_MIN(bh, r->height);

	return ftk_bitmap_copy_from_data_bgra32(bitmap, priv->bits, w, h, &rect);
#else
	//DECL_PRIV(thiz, priv);

	/*TODO*/
	/*snap_bitmap(priv, bitmap, r->x, r->y, r->width, r->height);*/

	return RET_OK;
#endif
}

static void ftk_display_wince_gles_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		eglMakeCurrent(priv->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroySurface(priv->display, priv->surface);
		eglDestroyContext(priv->display, priv->context);
		eglTerminate(priv->display);

		FreeLibrary(priv->egl_dll);
		FreeLibrary(priv->gles_dll);

		ReleaseDC(priv->wnd, priv->dc);

		FTK_FREE(priv->bits);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}
}

FtkDisplay* ftk_display_wince_create(void)
{
	EGLint i, n, attrs[64];
	EGLint major = 0;
	EGLint minor = 0;
	FtkDisplay* thiz;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_wince_gles_update;
		thiz->snap = ftk_display_wince_gles_snap;
		thiz->width = ftk_display_wince_gles_width;
		thiz->height = ftk_display_wince_gles_height;
		thiz->destroy = ftk_display_wince_gles_destroy;

#if 0
		priv->egl_dll = LoadLibraryW(L"/Windows/libEGL.dll");
		if(priv->egl_dll == NULL)
		{
			ftk_loge("%s: LoadLibrary(\"libEGL.dll\") failed (%d)\n", __func__, GetLastError());
			return NULL;
		}
#endif

		priv->gles_dll = LoadLibraryW(L"/Windows/libGLESv1_CM.dll");
		if(priv->gles_dll == NULL)
		{
			ftk_loge("%s: LoadLibrary(\"libGLESv1_CM.dll\") failed (%d)\n", __func__, GetLastError());
			return NULL;
		}

#if 1
		priv->egl_dll = priv->gles_dll;
#endif

		if(ftk_display_wince_gles_get_funcs(priv) != RET_OK)
		{
			return NULL;
		}

		priv->screen_width = GetSystemMetrics(SM_CXSCREEN);
		priv->screen_height = GetSystemMetrics(SM_CYSCREEN);
		priv->width = 512;
		priv->height = 512;

		priv->bits = FTK_ZALLOC(priv->width * priv->height * 4);
		if(priv->bits == NULL)
		{
			ftk_loge("%s: FTK_ZALLOC() failed\n", __func__);
			return NULL;
		}

		priv->wnd = ftk_display_wince_gles_create_win(priv);
		if(priv->wnd == NULL)
		{
			ftk_loge("%s: ftk_display_wince_gles_create_win() failed\n", __func__);
			FTK_FREE(priv->bits);
			return NULL;
		}

		priv->dc = GetDC(priv->wnd);

		priv->display = eglGetDisplay((NativeDisplayType)priv->dc);
		if(priv->display == EGL_NO_DISPLAY)
		{
			ftk_loge("%s:%d eglGetDisplay() failed\n", __FILE__, __LINE__);
			return NULL;
		}

		if(eglInitialize(priv->display, &major, &minor) != EGL_TRUE)
		{
			ftk_loge("%s:%d eglInitialize() failed\n", __FILE__, __LINE__);
			return NULL;
		}

		i = 0;

		attrs[i++] = EGL_RED_SIZE;
		attrs[i++] = 8;
		attrs[i++] = EGL_GREEN_SIZE;
		attrs[i++] = 8;
		attrs[i++] = EGL_BLUE_SIZE;
		attrs[i++] = 8;
		attrs[i++] = EGL_ALPHA_SIZE;
		attrs[i++] = EGL_DONT_CARE;

		attrs[i++] = EGL_SURFACE_TYPE;
		attrs[i++] = EGL_WINDOW_BIT;

		attrs[i++] = EGL_DEPTH_SIZE;
		attrs[i++] = 16;

		attrs[i++] = EGL_STENCIL_SIZE;
		attrs[i++] = EGL_DONT_CARE;

		attrs[i] = EGL_NONE;

		n = 0;

		if(eglChooseConfig(priv->display, attrs, &priv->config, 1, &n) != EGL_TRUE || n == 0)
		{
			ftk_loge("%s:%d eglChooseConfig() failed\n", __FILE__, __LINE__);
			return NULL;
		}

		priv->surface = eglCreateWindowSurface(priv->display, priv->config, priv->wnd, NULL);
		if(priv->surface == EGL_NO_SURFACE)
		{
			ftk_loge("%s:%d eglCreateWindowSurface() failed\n", __FILE__, __LINE__);
			return NULL;
		}

		priv->context = eglCreateContext(priv->display, priv->config, EGL_NO_CONTEXT, NULL);
		if(priv->context == EGL_NO_CONTEXT)
		{
			ftk_loge("%s:%d eglCreateContext() failed\n", __FILE__, __LINE__);
			return NULL;
		}

		if(eglMakeCurrent(priv->display, priv->surface, priv->surface, priv->context) != EGL_TRUE)
		{
			ftk_loge("%s:%d eglMakeCurrent() failed\n", __FILE__, __LINE__);
			return NULL;
		}


		glViewport(0, 0, priv->screen_width, priv->screen_height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0, (float)priv->screen_width, 0, (float)priv->screen_height, 0, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnableClientState (GL_VERTEX_ARRAY);
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//ftk_logd("%s:%d %s\n", __FILE__, __LINE__, glGetString(GL_EXTENSIONS));

		ftk_display_wince_gles_keymap_init();

		SetWindowLongW(priv->wnd, GWL_USERDATA, (LONG)priv);
		ShowWindow(priv->wnd, SW_SHOW);
		//UpdateWindow(priv->wnd);

		ftk_display_wince_gles_set_fullscreen(priv);
	}

	return thiz;
}
