
#include "ftk_typedef.h"
#include "ftk.h"
#include "ftk_log.h"
#include "ftk_bitmap.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_wince.h"
#include "ftk_input_method_manager.h"
#include "ftk_input_method_wince.h"
#include <aygshell.h>

typedef struct _PrivInfo
{
	HWND wnd;
	HBITMAP hBitmap;
	int width;
	int height;
	void* bits;
	FtkEvent event;
}PrivInfo;

static int s_key_map[0xff] = {0};
static wchar_t szClassName[] = L"FtkEmulator";

static void ftk_display_wince_set_fullscreen(PrivInfo* priv)
{
	SHFullScreen(priv->wnd, SHFS_HIDESIPBUTTON|SHFS_HIDETASKBAR|SHFS_HIDESTARTICON);
	SetWindowPos(priv->wnd, HWND_TOPMOST, 0, 0, priv->width, priv->height, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);
}

static void ftk_display_wince_keymap_init(void)
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

static char* ftk_display_wince_on_ime(HWND hwnd, LPARAM lParam)
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

static void ftk_display_wince_on_paint(PrivInfo* priv)
{
	HDC dc;
	int x, y, w, h;
	PAINTSTRUCT ps;
	HBITMAP bmp, old_bmp;

	BeginPaint(priv->wnd, &ps);

	x = ps.rcPaint.left;
	y = ps.rcPaint.top;
	w = ps.rcPaint.right - ps.rcPaint.left;
	h = ps.rcPaint.bottom - ps.rcPaint.top;

	bmp = CreateBitmap(priv->width, priv->height, 1, 32, priv->bits);

	dc = CreateCompatibleDC(ps.hdc);
	old_bmp = (HBITMAP)SelectObject(dc, bmp);
	BitBlt(ps.hdc, x, y, w, h, dc, x, y, SRCCOPY);
	SelectObject(dc, old_bmp);
	DeleteObject(dc);

	DeleteObject(bmp);
	EndPaint(priv->wnd, &ps);
}

static LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char* buf = NULL;
	PrivInfo* priv = (PrivInfo*) GetWindowLongW(hwnd, GWL_USERDATA);

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
			ftk_display_wince_on_paint(priv);
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
			buf = ftk_display_wince_on_ime(hwnd, lParam);
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

static HWND ftk_display_wince_create_win(PrivInfo *priv)
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

static Ret ftk_display_wince_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	//HDC hdc_src, hdc_dest;
	RECT dirty_rect;
	//HBITMAP hbm_src;
	DECL_PRIV(thiz, priv);
	//BITMAPINFO bmi;

	ftk_bitmap_copy_to_data_bgra32(bitmap, rect, 
		priv->bits, xoffset, yoffset, priv->width, priv->height); 
#if 0
	memset(&bmi, 0x00, sizeof(bmi));
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = priv->width;
	bmi.bmiHeader.biHeight = priv->height;
	bmi.bmiHeader.biPlanes = 1;

	hdc_dest = CreateCompatibleDC(0);
	SelectObject(hdc_dest, priv->hBitmap);
	hdc_src = CreateCompatibleDC(0);
	hbm_src = CreateBitmap(priv->width, priv->height, 1, 32, priv->bits);
	SelectObject(hdc_src, hbm_src);
	BitBlt(hdc_dest, 0, 0, priv->width, priv->height, hdc_src, 0, 0, SRCCOPY);
	DeleteDC(hdc_src);
	DeleteObject(hbm_src);
	DeleteDC(hdc_dest);
#endif
	dirty_rect.left = xoffset;
	dirty_rect.top = yoffset;
	dirty_rect.right = xoffset + rect->width;
	dirty_rect.bottom = yoffset + rect->height;

	InvalidateRect(priv->wnd, &dirty_rect, FALSE);

	return RET_OK;
}

static int ftk_display_wince_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->width;
}

static int ftk_display_wince_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->height;
}

static Ret ftk_display_wince_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
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
}

static void ftk_display_wince_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->bits);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}
}

FtkDisplay* ftk_display_wince_create(void)
{
	FtkDisplay* thiz;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_wince_update;
		thiz->snap = ftk_display_wince_snap;
		thiz->width = ftk_display_wince_width;
		thiz->height = ftk_display_wince_height;
		thiz->destroy = ftk_display_wince_destroy;

		priv->width = GetSystemMetrics(SM_CXSCREEN);
		priv->height = GetSystemMetrics(SM_CYSCREEN);
		priv->bits = FTK_ZALLOC(priv->width * priv->height * 4);

		priv->wnd = ftk_display_wince_create_win(priv);

		ftk_display_wince_keymap_init();

		SetWindowLongW(priv->wnd, GWL_USERDATA, (LONG)priv);
		ShowWindow(priv->wnd, SW_SHOW);
		//UpdateWindow(priv->wnd);

		ftk_display_wince_set_fullscreen(priv);
	}

	return thiz;
}
