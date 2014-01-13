/*
 * File:    vnc_service.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   Support VNC.
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
 * 2010-07-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <rfb/rfb.h>
#include <rfb/keysym.h>

#include "ftk_log.h"
#include <pthread.h>
#include "ftk_globals.h"
#include "vnc_service.h"

typedef struct VncContext 
{
	int quit;
	int active;
	int inited;

	int width;
	int height;
	int old_x;
	int old_y;
	int max_x;
	int max_y;
	int client_nr;
	char* vnc_buffer;
	pthread_t vnc_tid;
	rfbBool old_button;
	FtkEvent event;
	rfbScreenInfoPtr rfbScreen;
} VncContext;

static VncContext g_vnc_context = {0};

static void on_client_gone(rfbClientPtr cl)
{
	g_vnc_context.old_x = 0;
	g_vnc_context.old_y = 0;
	g_vnc_context.old_button = 0;
	g_vnc_context.client_nr--;
	ftk_logd("%s g_vnc_context.client_nr=%d\n", __func__, g_vnc_context.client_nr);

	return;
}

static enum rfbNewClientAction on_new_client(rfbClientPtr cl)
{
	if(g_vnc_context.client_nr > 0 || !g_vnc_context.active)
	{
		return RFB_CLIENT_REFUSE;
	}
	
	g_vnc_context.client_nr++;
	cl->clientGoneHook = on_client_gone;
	ftk_logd("%s g_vnc_context.client_nr=%d\n", __func__, g_vnc_context.client_nr);

	return RFB_CLIENT_ACCEPT;
}

static void on_pointer_event(int buttonMask, int x,int y, rfbClientPtr cl)
{
	if(x >=0 && y >=0 && x < g_vnc_context.max_x && y < g_vnc_context.max_y) 
	{
		g_vnc_context.event.type = FTK_EVT_NOP;
		g_vnc_context.event.u.mouse.x = x;
		g_vnc_context.event.u.mouse.y = y;
		if(buttonMask) 
		{
			 if(g_vnc_context.old_button==buttonMask) 
			 { 
			 	printf("move %d %d\n", x, y);
			 	g_vnc_context.event.type = FTK_EVT_MOUSE_MOVE;
			 } else 
			 { 
			 	printf("press %d %d\n", x, y);
			 	g_vnc_context.event.type = FTK_EVT_MOUSE_DOWN;
			 }
		}
		else
		{
			if(g_vnc_context.old_button)
			{
				printf("up %d %d\n", x, y);
			 	g_vnc_context.event.type = FTK_EVT_MOUSE_UP;
			}
			
			g_vnc_context.old_button=0;
		}
		g_vnc_context.old_x=x; g_vnc_context.old_y=y; g_vnc_context.old_button=buttonMask;

		if(g_vnc_context.event.type != FTK_EVT_NOP)
		{
			ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &g_vnc_context.event);
		}
	}

	rfbDefaultPtrAddEvent(buttonMask, x, y, cl);

	return;
}

static const int s_key_map[256] = 
{
	['1'] = FTK_KEY_1,
	['2'] = FTK_KEY_2,
	['3'] = FTK_KEY_3,
	['4'] = FTK_KEY_4,
	['5'] = FTK_KEY_5,
	['6'] = FTK_KEY_6,
	['7'] = FTK_KEY_7,
	['8'] = FTK_KEY_8,
	['9'] = FTK_KEY_9,
	['0'] = FTK_KEY_0,
	['a'] = FTK_KEY_a,
	['b'] = FTK_KEY_b,
	['c'] = FTK_KEY_c,
	['d'] = FTK_KEY_d,
	['e'] = FTK_KEY_e,
	['f'] = FTK_KEY_f,
	['g'] = FTK_KEY_g,
	['h'] = FTK_KEY_h,
	['i'] = FTK_KEY_i,
	['j'] = FTK_KEY_j,
	['k'] = FTK_KEY_k,
	['l'] = FTK_KEY_l,
	['m'] = FTK_KEY_m,
	['n'] = FTK_KEY_n,
	['o'] = FTK_KEY_o,
	['p'] = FTK_KEY_p,
	['q'] = FTK_KEY_q,
	['r'] = FTK_KEY_r,
	['s'] = FTK_KEY_s,
	['t'] = FTK_KEY_t,
	['u'] = FTK_KEY_u,
	['v'] = FTK_KEY_v,
	['w'] = FTK_KEY_w,
	['x'] = FTK_KEY_x,
	['y'] = FTK_KEY_y,
	['z'] = FTK_KEY_z,
	['A'] = FTK_KEY_A,
	['B'] = FTK_KEY_B,
	['C'] = FTK_KEY_C,
	['D'] = FTK_KEY_D,
	['E'] = FTK_KEY_E,
	['F'] = FTK_KEY_F,
	['G'] = FTK_KEY_G,
	['H'] = FTK_KEY_H,
	['I'] = FTK_KEY_I,
	['J'] = FTK_KEY_J,
	['K'] = FTK_KEY_K,
	['L'] = FTK_KEY_L,
	['M'] = FTK_KEY_M,
	['N'] = FTK_KEY_N,
	['O'] = FTK_KEY_O,
	['P'] = FTK_KEY_P,
	['Q'] = FTK_KEY_Q,
	['R'] = FTK_KEY_R,
	['S'] = FTK_KEY_S,
	['T'] = FTK_KEY_T,
	['U'] = FTK_KEY_U,
	['V'] = FTK_KEY_V,
	['W'] = FTK_KEY_W,
	['X'] = FTK_KEY_X,
	['Y'] = FTK_KEY_Y,
	['Z'] = FTK_KEY_Z,
	[','] = FTK_KEY_COMMA,
	['.'] = FTK_KEY_DOT,
	['/'] = FTK_KEY_SLASH,
	[';'] = FTK_KEY_SEMICOLON,
	[' '] = FTK_KEY_SPACE,
	['-'] = FTK_KEY_MINUS,
	['~'] = FTK_KEY_NOT,
	['!'] = FTK_KEY_EXCLAM,
	['@'] = FTK_KEY_AT,
	['#'] = FTK_KEY_NUMBERSIGN,
	['$'] = FTK_KEY_DOLLAR,
	['%'] = FTK_KEY_PERCENT,
	['^'] = FTK_KEY_ASCIICIRCUM,
	['&'] = FTK_KEY_AMPERSAND,
	['*'] = FTK_KEY_ASTERISK,
	['('] = FTK_KEY_PARENLEFT,
	[')'] = FTK_KEY_PARENRIGHT,
	['['] = FTK_KEY_BRACKETLEFT,
	[']'] = FTK_KEY_BRACKETRIGHT,
	['{'] = FTK_KEY_LEFTBRACE,
	['}'] = FTK_KEY_RIGHTBRACE,
	['|'] = FTK_KEY_OR,
	['+'] = FTK_KEY_PLUS,
	['_'] = FTK_KEY_UNDERSCORE,
	['\\'] = FTK_KEY_SLASH,
	[':'] = FTK_KEY_COLON,
	[';'] = FTK_KEY_SEMICOLON,
	['\''] = FTK_KEY_QUOTERIGHT,
	['\"'] = FTK_KEY_QUOTEDBL,
	['<'] = FTK_KEY_LESS,
	['>'] = FTK_KEY_GREATER,
	['?'] = FTK_KEY_QUESTION,
	[','] = FTK_KEY_COMMA,
	['.'] = FTK_KEY_DOT,
	['/'] = FTK_KEY_BACKSLASH,
	['='] = FTK_KEY_EQUAL
};

static const int s_ctl_key_map[256] = 
{
	[0xbe] = FTK_KEY_F1,
	[0xbf] = FTK_KEY_F2,  
	[0xc0] = FTK_KEY_F3, 
	[0xc1] = FTK_KEY_F4, 
	[0xc2] = FTK_KEY_F5, 
	[0xc3] = FTK_KEY_F6,
	[0x0d] = FTK_KEY_ENTER,
	[0x08] = FTK_KEY_BACKSPACE,
	[0xe1] = FTK_KEY_LEFTSHIFT,
	[0xe5] = FTK_KEY_CAPSLOCK,
	[0x09] = FTK_KEY_TAB,
	[0xe3] = FTK_KEY_LEFTCTRL,
	[0x1b] = FTK_KEY_ESC,
	[0x51] = FTK_KEY_LEFT,
	[0x52] = FTK_KEY_UP,
	[0x53] = FTK_KEY_RIGHT,
	[0x54] = FTK_KEY_DOWN,
	[0x55] = FTK_KEY_PAGEUP,
	[0x56] = FTK_KEY_PAGEDOWN,
	[0x63] = FTK_KEY_INSERT,
	[0xff] = FTK_KEY_DELETE,
	[0x50] = FTK_KEY_HOME,
	[0x57] = FTK_KEY_END,
};

static void on_key_event(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{
	int code = 0;
	if(key < 0x100)
	{
		code = s_key_map[key];
	}
	else
	{
		code = s_ctl_key_map[0xff & key];
	}

	ftk_logd("%s: %02x -- > %02x\n", __func__, key, code);
	g_vnc_context.event.type = down ? FTK_EVT_KEY_DOWN : FTK_EVT_KEY_UP;
	g_vnc_context.event.u.key.code = code;
	ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &g_vnc_context.event);

	return;
}

static void* vnc_thread(void* args)
{
	int i = 0;
	int argc = 1;
	int width = g_vnc_context.width;
	int height = g_vnc_context.height;
	char* argv[5] = {"FTKVNC", NULL};
	rfbScreenInfoPtr rfbScreen = NULL;
	
	ftk_logd("%s: vnc thread start.\n", __func__);

	rfbScreen = rfbGetScreen(&argc, argv, width, height, 8, 4, 4);
	rfbScreen->desktopName   = "FTK VNC";
	rfbScreen->alwaysShared  = 0;
	rfbScreen->frameBuffer   = g_vnc_context.vnc_buffer;
	rfbScreen->ptrAddEvent   = on_pointer_event;
	rfbScreen->kbdAddEvent   = on_key_event;
	rfbScreen->newClientHook = on_new_client;
	rfbScreen->deferUpdateTime = 100;
	rfbInitServer(rfbScreen);

	g_vnc_context.rfbScreen = rfbScreen;
	for(i=0; !g_vnc_context.quit && rfbIsActive(rfbScreen); i++)
	{
		if(g_vnc_context.client_nr > 0 && g_vnc_context.active)
		{
			if((i % 10) == 0)
			{
				rfbMarkRectAsModified(g_vnc_context.rfbScreen, 0, 0, width, height);
			}
		}
		else
		{
			sleep(1);
		}

		rfbProcessEvents(rfbScreen, 200000);
	}

	ftk_logd("%s: vnc thread quit.\n", __func__);

	rfbScreenCleanup(rfbScreen);

	return NULL;
}

static Ret on_display_change(void* ctx, FtkDisplay* display, int before,
    FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	if(!before && g_vnc_context.active)
	{
		ftk_bitmap_copy_to_data_rgba32(bitmap, rect, g_vnc_context.vnc_buffer, xoffset, yoffset, 
			g_vnc_context.width, g_vnc_context.height);
		if(g_vnc_context.client_nr > 0)
		{
			rfbMarkRectAsModified(g_vnc_context.rfbScreen, xoffset, xoffset, rect->width, rect->height);
		}
	}

	return RET_OK;
}

static Ret ftk_display_vnc_init(void)
{
	if(!g_vnc_context.inited)
	{
		memset(&g_vnc_context, 0x00, sizeof(g_vnc_context));
		
		g_vnc_context.inited = 1;
		ftk_display_reg_update_listener(ftk_default_display(), on_display_change, NULL);
		g_vnc_context.width = ftk_display_width(ftk_default_display());
		g_vnc_context.height = ftk_display_height(ftk_default_display());
		g_vnc_context.max_x = g_vnc_context.width;
		g_vnc_context.max_y = g_vnc_context.height;
		g_vnc_context.vnc_buffer = calloc(g_vnc_context.width * g_vnc_context.height * 4, 1);
		pthread_create(&g_vnc_context.vnc_tid, NULL, vnc_thread, NULL);
	}

	return RET_OK;
}

Ret ftk_display_vnc_start(void)
{
	ftk_display_vnc_init();

	if(!g_vnc_context.active)
	{
		g_vnc_context.active = 1;
	}

	return RET_OK;
}

Ret ftk_display_vnc_stop(void)
{
	if(g_vnc_context.active)
	{
		g_vnc_context.active = 0;
	}

	return RET_OK;
}

Ret ftk_display_vnc_quit(void)
{
	if(g_vnc_context.inited)
	{
		g_vnc_context.active = 0;
		g_vnc_context.quit = 1;
		pthread_join(g_vnc_context.vnc_tid, NULL);
	}

	return RET_OK;
}

int ftk_display_vnc_is_active(void)
{
	return g_vnc_context.active;
}

Ret ftk_display_vnc_set_passwd(const char* user, const char* passwd)
{
	/*TODO*/
	return RET_OK;
}

