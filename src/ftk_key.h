/*
 * File: ftk_key.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   key symbols
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#ifndef _FTK_KEY_H
#define _FTK_KEY_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

typedef enum _FtkKey
{
	FTK_KEY_SPACE         =  0x020,
	FTK_KEY_EXCLAM        =  0x021,
	FTK_KEY_QUOTEDBL      =  0x022,
	FTK_KEY_NUMBERSIGN    =  0x023,
	FTK_KEY_DOLLAR        =  0x024,
	FTK_KEY_PERCENT       =  0x025,
	FTK_KEY_AMPERSAND     =  0x026,
	FTK_KEY_APOSTROPHE    =  0x027,
	FTK_KEY_QUOTERIGHT    =  0x027,
	FTK_KEY_PARENLEFT     =  0x028,
	FTK_KEY_PARENRIGHT    =  0x029,
	FTK_KEY_ASTERISK      =  0x02a,
	FTK_KEY_PLUS          =  0x02b,
	FTK_KEY_COMMA         =  0x02c,
	FTK_KEY_MINUS         =  0x02d,
	FTK_KEY_PERIOD        =  0x02e,
	FTK_KEY_SLASH         =  0x02f,
	FTK_KEY_0             =  0x030,
	FTK_KEY_1             =  0x031,
	FTK_KEY_2             =  0x032,
	FTK_KEY_3             =  0x033,
	FTK_KEY_4             =  0x034,
	FTK_KEY_5             =  0x035,
	FTK_KEY_6             =  0x036,
	FTK_KEY_7             =  0x037,
	FTK_KEY_8             =  0x038,
	FTK_KEY_9             =  0x039,
	FTK_KEY_COLON         =  0x03a,
	FTK_KEY_SEMICOLON     =  0x03b,
	FTK_KEY_LESS          =  0x03c,
	FTK_KEY_EQUAL         =  0x03d,
	FTK_KEY_GREATER       =  0x03e,
	FTK_KEY_QUESTION      =  0x03f,
	FTK_KEY_AT            =  0X040,
	FTK_KEY_A             =  0x041,
	FTK_KEY_B             =  0x042,
	FTK_KEY_C             =  0x043,
	FTK_KEY_D             =  0x044,
	FTK_KEY_E             =  0x045,
	FTK_KEY_F             =  0x046,
	FTK_KEY_G             =  0x047,
	FTK_KEY_H             =  0x048,
	FTK_KEY_I             =  0x049,
	FTK_KEY_J             =  0x04a,
	FTK_KEY_K             =  0x04b,
	FTK_KEY_L             =  0x04c,
	FTK_KEY_M             =  0x04d,
	FTK_KEY_N             =  0x04e,
	FTK_KEY_O             =  0x04f,
	FTK_KEY_P             =  0x050,
	FTK_KEY_Q             =  0x051,
	FTK_KEY_R             =  0x052,
	FTK_KEY_S             =  0x053,
	FTK_KEY_T             =  0x054,
	FTK_KEY_U             =  0x055,
	FTK_KEY_V             =  0x056,
	FTK_KEY_W             =  0x057,
	FTK_KEY_X             =  0x058,
	FTK_KEY_Y             =  0x059,
	FTK_KEY_Z             =  0x05a,
	FTK_KEY_BRACKETLEFT   =  0x05b,
	FTK_KEY_BACKSLASH     =  0x05c,
	FTK_KEY_BRACKETRIGHT  =  0x05d,
	FTK_KEY_ASCIICIRCUM   =  0x05e,
	FTK_KEY_UNDERSCORE    =  0x05f,
	FTK_KEY_QUOTELEFT     =  0x060,
	FTK_KEY_GRAVE         =  0x060,
	FTK_KEY_a             =  0x061,
	FTK_KEY_b             =  0x062,
	FTK_KEY_c             =  0x063,
	FTK_KEY_d             =  0x064,
	FTK_KEY_e             =  0x065,
	FTK_KEY_f             =  0x066,
	FTK_KEY_g             =  0x067,
	FTK_KEY_h             =  0x068,
	FTK_KEY_i             =  0x069,
	FTK_KEY_j             =  0x06a,
	FTK_KEY_k             =  0x06b,
	FTK_KEY_l             =  0x06c,
	FTK_KEY_m             =  0x06d,
	FTK_KEY_n             =  0x06e,
	FTK_KEY_o             =  0x06f,
	FTK_KEY_p             =  0x070,
	FTK_KEY_q             =  0x071,
	FTK_KEY_r             =  0x072,
	FTK_KEY_s             =  0x073,
	FTK_KEY_t             =  0x074,
	FTK_KEY_u             =  0x075,
	FTK_KEY_v             =  0x076,
	FTK_KEY_w             =  0x077,
	FTK_KEY_x             =  0x078,
	FTK_KEY_y             =  0x079,
	FTK_KEY_z             =  0x07a,
	FTK_KEY_LEFTBRACE     =  0x07B,
	FTK_KEY_OR            =  0x07C,
	FTK_KEY_RIGHTBRACE    =  0x07D,
	FTK_KEY_NOT           =  0x07E,
	FTK_KEY_DOT           =  '.',
	FTK_KEY_HOME          =  0xFF50,
	FTK_KEY_LEFT          =  0xFF51,
	FTK_KEY_UP            =  0xFF52,
	FTK_KEY_RIGHT         =  0xFF53,
	FTK_KEY_DOWN          =  0xFF54,
	FTK_KEY_PRIOR         =  0xFF55,
	FTK_KEY_PAGEUP        =  0xFF55,
	FTK_KEY_NEXT          =  0xFF56,
	FTK_KEY_PAGEDOWN      =  0xFF56,
	FTK_KEY_END           =  0xFF57,
	FTK_KEY_BEGIN         =  0xFF58,
	FTK_KEY_INSERT        =  0xFF63,
	FTK_KEY_ENTER         =  0xFF0D,
	FTK_KEY_TAB           =  0xFF09,
	FTK_KEY_ESC           =  0xFF1B,
	FTK_KEY_RIGHTCTRL   =  0xFFE4,
	FTK_KEY_LEFTCTRL    =  0xFFE3,
	FTK_KEY_RIGHTALT    =  0xFFEA,
	FTK_KEY_LEFTALT     =  0xFFE9,
	FTK_KEY_RIGHTSHIFT  =  0xFFE2,
	FTK_KEY_LEFTSHIFT   =  0xFFE1,
	FTK_KEY_DELETE      =  0xFFFF,
	FTK_KEY_F1          =  0xFFBE,
	FTK_KEY_F2          =  0xFFBF,
	FTK_KEY_F3          =  0xFFC0,
	FTK_KEY_RETURN      = FTK_KEY_F3,
	FTK_KEY_F4          =  0xFFC1,
	FTK_KEY_F5          =  0xFFC2,
	FTK_KEY_F6          =  0xFFC3,
	FTK_KEY_F7          =  0xFFC4,
	FTK_KEY_F8          =  0xFFC5,
	FTK_KEY_F9          =  0xFFC6,
	FTK_KEY_F10         =  0xFFC7,
	FTK_KEY_CAPSLOCK    =  0xFFE5,
	FTK_KEY_BACKSPACE   =  0xFF08,
	FTK_KEY_SEND        =  0xFFA1,
	FTK_KEY_REPLY       =  0xFFA2,
	FTK_KEY_SAVE        =  0xFFA3,
	FTK_KEY_BATTERY     =  0xFFA4,
	FTK_KEY_BLUETOOTH   =  0xFFA5,
	FTK_KEY_WLAN        =  0xFFA6,
	FTK_KEY_POWER       =  0xFFA7,
	FTK_KEY_MENU        = FTK_KEY_F2,
	FTK_KEY_CHOOSE_IME  = FTK_KEY_F4,
	FTK_KEY_CLR         =  0xFFA8,
}FtkKey;

#define FTK_IS_DIGIT(key) ((key) >= FTK_KEY_0 && (key) <= FTK_KEY_9)
#define FTK_IS_ALPHA(key) ((key) >= FTK_KEY_A && (key) <= FTK_KEY_Z) && ((key) >= FTK_KEY_a && (key) <= FTK_KEY_z)
#define FTK_IS_ACTIVE_KEY(key) ((key) == FTK_KEY_ENTER || (key) == FTK_KEY_SPACE)

FTK_END_DECLS

#endif/*_FTK_KEY_H*/
