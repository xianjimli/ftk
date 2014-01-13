/*
 * File: ftk_display_win32_win32.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   win32 display
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2009-11-17 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <windows.h>

#include "ftk.h"
#include "ftk_typedef.h"
#include "ftk_log.h"
#include "ftk_bitmap.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_wnd_manager.h"
#include "ftk_display_win32.h"
#include "ftk_input_method_manager.h"
#include "ftk_input_method_win32.h"

#define DISPLAY_WIDTH  640
#define DISPLAY_HEIGHT 480

static char szClassName[ ] = "FtkEmulator";
static LRESULT CALLBACK WinProc (HWND, UINT, WPARAM, LPARAM);

static HWND ftk_create_display_window(void)
{
    HWND hwnd;               /* This is the handle for our window */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
	int width = DISPLAY_WIDTH + GetSystemMetrics(SM_CXFRAME) * 2;
	int height = DISPLAY_HEIGHT + GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

    /* The Window structure */
    wincl.hInstance = NULL;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WinProc;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 64;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "FtkEmulator",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           width,       /* The programs width */
           height,      /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           NULL,                /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

	return hwnd;
}


typedef struct _PrivInfo
{
	HWND wnd;
	void* bits;
	void* revert_bits;
	HBITMAP hBitmap;
	FtkEvent event;
}PrivInfo;

static LRESULT WinOnPaint(HWND hwnd)
{
	HDC dc = NULL;
	PAINTSTRUCT ps;
	HBITMAP hBitmap;
	PrivInfo* priv = NULL;
	HBITMAP hDisplay = NULL;
	
	BeginPaint(hwnd, &ps);
	priv = (PrivInfo*)GetWindowLong(hwnd, GWL_USERDATA);
	hDisplay = priv->hBitmap;
	dc = CreateCompatibleDC(ps.hdc);
	hBitmap = (HBITMAP)SelectObject(dc, hDisplay);
	BitBlt(ps.hdc, 0, 0, DISPLAY_WIDTH,	DISPLAY_HEIGHT, dc, 0, 0, SRCCOPY);
	SelectObject(dc, hBitmap);
	DeleteObject(dc);

	EndPaint(hwnd, &ps);

	return 0;
}

static int s_key_map[0xff] = {0};

static Ret ftk_on_key_event(PrivInfo* priv, int down, unsigned char vkey)
{
	priv->event.type = down ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
	priv->event.u.key.code = s_key_map[vkey];

	return RET_OK;
}

static char* ftk_on_ime_event(HWND hwnd, LPARAM lParam)
{
	LONG n;
	char* gb2312;
	wchar_t* unicode;
	char* utf8;
	HIMC himc;

	himc = ImmGetContext(hwnd);
	if(himc == NULL)
	{
		return NULL;
	}
	n = ImmGetCompositionString(himc, GCS_RESULTSTR, NULL, 0);
	if(n <= 0)
	{
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	gb2312 = (char*) FTK_ALLOC(n + 1);
	if(gb2312 == NULL)
	{
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	if(ImmGetCompositionString(himc, GCS_RESULTSTR, gb2312, n) != n)
	{
		FTK_FREE(gb2312);
		ImmReleaseContext(hwnd, himc);
		return NULL;
	}
	ImmReleaseContext(hwnd, himc);

	gb2312[n] = '\0';
	ftk_logd("%s:%d WM_IME_COMPOSITION:%s\n", __FILE__, __LINE__, gb2312);
	n = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	if(n < 1)
	{
		FTK_FREE(gb2312);
		return NULL;
	}
	unicode = (wchar_t*) FTK_ALLOC(n * 2);
	if(unicode == NULL)
	{
		FTK_FREE(gb2312);
		return NULL;
	}
	n = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, unicode, n);
	if(n < 1)
	{
		FTK_FREE(gb2312);
		FTK_FREE(unicode);
		return NULL;
	}

	FTK_FREE(gb2312);

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

	return utf8;
}

static LRESULT CALLBACK WinProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PrivInfo* priv = (PrivInfo*)GetWindowLong(hwnd, GWL_USERDATA);

	if(priv != NULL)
	{
		memset(&priv->event, 0, sizeof(FtkEvent));
	}

    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            FTK_QUIT();
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
		case WM_PAINT:
			{
				WinOnPaint(hwnd);
				break;
			}
		case WM_KEYDOWN:
		case WM_KEYUP:
			{
				ftk_on_key_event(priv, message==WM_KEYDOWN, 0xFF & wParam);
				break;
			}
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
			{
				int xPos = LOWORD(lParam); 
				int yPos = HIWORD(lParam); 
				priv->event.u.mouse.x = xPos;
				priv->event.u.mouse.y = yPos;
				priv->event.type = message == WM_LBUTTONUP? FTK_EVT_MOUSE_UP : FTK_EVT_MOUSE_DOWN;

				break;
			}
		case WM_MOUSEMOVE:
			{
				int xPos = LOWORD(lParam); 
				int yPos = HIWORD(lParam); 
				priv->event.u.mouse.x = xPos;
				priv->event.u.mouse.y = yPos;
				priv->event.type = FTK_EVT_MOUSE_MOVE;

				break;
			}
		case WM_IME_COMPOSITION:
			{
				char* buf;
				FtkInputMethod* im = NULL;

				if(!(lParam & GCS_RESULTSTR))
				{
					return DefWindowProc(hwnd, message, wParam, lParam);
				}
				if(ftk_input_method_manager_get(ftk_default_input_method_manager(), 2, &im) == RET_FAIL || im == NULL)
				{
					return DefWindowProc(hwnd, message, wParam, lParam);
				}
				if(ftk_input_method_win32_get_editor(im) == NULL)
				{
					return DefWindowProc(hwnd, message, wParam, lParam);
				}

				buf = ftk_on_ime_event(hwnd, lParam);
				if(buf == NULL)
				{
					return DefWindowProc(hwnd, message, wParam, lParam);
				}
				priv->event.u.extra = buf;
				priv->event.type = FTK_EVT_OS_IM_COMMIT;
				break;
			}
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

	if(priv->event.type != FTK_EVT_NOP)
	{
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
		priv->event.type = FTK_EVT_NOP;
	}
    return 0;
}

static Ret ftk_display_win32_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	int j = 0;
	MSG msg;
	DECL_PRIV(thiz, priv);
	BITMAPINFO bmi;
	int display_width  = DISPLAY_WIDTH;
	int display_height = DISPLAY_HEIGHT;
	FtkColor* bits = (FtkColor*)priv->bits;

	memset(&bmi, 0x00, sizeof(bmi));
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = DISPLAY_WIDTH;
	bmi.bmiHeader.biHeight = DISPLAY_HEIGHT;
	bmi.bmiHeader.biPlanes = 1;

	ftk_bitmap_copy_to_data_bgra32(bitmap, rect, 
		priv->bits, xoffset, yoffset, display_width, display_height); 

	/* top-down bitmap */
	FtkColor* src = (FtkColor*)priv->bits;
	FtkColor* dst = (FtkColor*)priv->revert_bits + ((DISPLAY_HEIGHT - 1) * DISPLAY_WIDTH);
	for(j = 0; j < DISPLAY_HEIGHT; j++)
	{
		for(i = 0; i < DISPLAY_WIDTH; i++)
		{
			dst[i] = src[i];
		}
		src += DISPLAY_WIDTH;
		dst -= DISPLAY_WIDTH;
	}

	/* The origin for DIBs is the lower-left corner of the bitmap */
	::SetDIBits(GetDC(priv->wnd), priv->hBitmap, 0, DISPLAY_HEIGHT, priv->revert_bits, &bmi, DIB_RGB_COLORS);

	InvalidateRect(priv->wnd, NULL, FALSE);

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(msg.message == WM_PAINT) break;
	}

	return RET_OK;
}

static int ftk_display_win32_width(FtkDisplay* thiz)
{
	return DISPLAY_WIDTH;
}

static int ftk_display_win32_height(FtkDisplay* thiz)
{
	return DISPLAY_HEIGHT;
}

static Ret ftk_display_win32_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
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

static void ftk_display_win32_destroy(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	if(priv != NULL)
	{
		CloseWindow(priv->wnd);
		FTK_FREE(priv->bits);
		FTK_FREE(priv->revert_bits);
		DeleteObject(priv->hBitmap);
		FTK_FREE(thiz);
	}

	return;
}

static void ftk_keymap_init(void)
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

	return;
}

FtkDisplay* ftk_display_win32_create(void)
{
	FtkDisplay* thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		PrivInfo* p = NULL;
		BITMAPINFO bmi = {0};
		HBITMAP hBitmap = NULL;
		DECL_PRIV(thiz, priv);

		thiz->update   = ftk_display_win32_update;
		thiz->width    = ftk_display_win32_width;
		thiz->height   = ftk_display_win32_height;
		thiz->snap     = ftk_display_win32_snap;
		thiz->destroy  = ftk_display_win32_destroy;

		priv->wnd = ftk_create_display_window();

		memset(&bmi, 0x00, sizeof(bmi));

		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biHeight   = DISPLAY_HEIGHT;
		bmi.bmiHeader.biWidth    = DISPLAY_HEIGHT;
		bmi.bmiHeader.biSize     = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biPlanes   = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;

		hBitmap = CreateBitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1, 32, NULL);
		priv->hBitmap = hBitmap;
		priv->bits = FTK_ZALLOC(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
		priv->revert_bits = FTK_ZALLOC(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);

		SetWindowLong(priv->wnd, GWL_USERDATA, (LONG)priv);
		ShowWindow (priv->wnd, SW_SHOW);
		UpdateWindow(priv->wnd);

		ftk_keymap_init();
	}

	return thiz;
}

