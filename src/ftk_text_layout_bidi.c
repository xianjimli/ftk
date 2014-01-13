/*
 * File: ftk_text_layout_bidi.c
 *
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: interface for text layout(bidi,line break,shape join).
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
 * 2010-07-18 Li XianJing <xianjimli@hotmail.com> created
 *
 */

/*
 * XXX: Part of the code in this file come from microwindow, the author has its copyright.
 */

/*
 * Copyright (c) 2000, 2002, 2003, 2005 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ftk_text_layout.h"

/**
 * UTF-8 to UTF-16 conversion.  Surrogates are handeled properly, e.g.
 * a single 4-byte UTF-8 character is encoded into a surrogate pair.
 * On the other hand, if the UTF-8 string contains surrogate values, this
 * is considered an error and returned as such.
 *
 * The destination array must be able to hold as many Unicode-16 characters
 * as there are ASCII characters in the UTF-8 string.  This in case all UTF-8
 * characters are ASCII characters.  No more will be needed.
 *
 * This function will also accept Java's variant of UTF-8.  This encodes
 * U+0000 as two characters rather than one, so the UTF-8 does not contain
 * any zeroes.
 *
 * @author Copyright (c) 2000 Morten Rolland, Screen Media
 *
 * @param utf8      Input string in UTF8 format.
 * @param cc        Number of bytes to convert.
 * @param unicode16 Destination buffer.
 * @return          Number of characters converted, or -1 if input is not
 *                  valid UTF8.
 */
static int
utf8_to_utf16(const unsigned char *utf8, int cc, unsigned short *unicode16)
{
	int count = 0;
	unsigned char c0, c1;
	unsigned long scalar;

	while(--cc >= 0) {
		c0 = *utf8++;
		/*DPRINTF("Trying: %02x\n",c0);*/

		if ( c0 < 0x80 ) {
			/* Plain ASCII character, simple translation :-) */
			*unicode16++ = c0;
			count++;
			continue;
		}

		if ( (c0 & 0xc0) == 0x80 )
			/* Illegal; starts with 10xxxxxx */
			return -1;

		/* c0 must be 11xxxxxx if we get here => at least 2 bytes */
		scalar = c0;
		if(--cc < 0)
			return -1;
		c1 = *utf8++;
		/*DPRINTF("c1=%02x\n",c1);*/
		if ( (c1 & 0xc0) != 0x80 )
			/* Bad byte */
			return -1;
		scalar <<= 6;
		scalar |= (c1 & 0x3f);

		if ( !(c0 & 0x20) ) {
			/* Two bytes UTF-8 */
			if ( (scalar != 0) && (scalar < 0x80) )
				return -1;	/* Overlong encoding */
			*unicode16++ = scalar & 0x7ff;
			count++;
			continue;
		}

		/* c0 must be 111xxxxx if we get here => at least 3 bytes */
		if(--cc < 0)
			return -1;
		c1 = *utf8++;
		/*DPRINTF("c1=%02x\n",c1);*/
		if ( (c1 & 0xc0) != 0x80 )
			/* Bad byte */
			return -1;
		scalar <<= 6;
		scalar |= (c1 & 0x3f);

		if ( !(c0 & 0x10) ) {
			/*DPRINTF("####\n");*/
			/* Three bytes UTF-8 */
			if ( scalar < 0x800 )
				return -1;	/* Overlong encoding */
			if ( scalar >= 0xd800 && scalar < 0xe000 )
				return -1;	/* UTF-16 high/low halfs */
			*unicode16++ = scalar & 0xffff;
			count++;
			continue;
		}

		/* c0 must be 1111xxxx if we get here => at least 4 bytes */
		c1 = *utf8++;
		if(--cc < 0)
			return -1;
		/*DPRINTF("c1=%02x\n",c1);*/
		if ( (c1 & 0xc0) != 0x80 )
			/* Bad byte */
			return -1;
		scalar <<= 6;
		scalar |= (c1 & 0x3f);

		if ( !(c0 & 0x08) ) {
			/* Four bytes UTF-8, needs encoding as surrogates */
			if ( scalar < 0x10000 )
				return -1;	/* Overlong encoding */
			scalar -= 0x10000;
			*unicode16++ = ((scalar >> 10) & 0x3ff) + 0xd800;
			*unicode16++ = (scalar & 0x3ff) + 0xdc00;
			count += 2;
			continue;
		}

		return -1;	/* No support for more than four byte UTF-8 */
	}
	return count;
}

/* 
 * warning: the length of output string may exceed six x the length of the input 
 */ 
static int
uc16_to_utf8(const unsigned short *us, int cc, unsigned char *s)
{
	int i;
	unsigned char *t = s;
	unsigned short uc16;
	
	for (i = 0; i < cc; i++) {
		uc16 = us[i];
		if (uc16 <= 0x7F) { 
			*t++ = (char) uc16;
		} else if (uc16 <= 0x7FF) {
			*t++ = 0xC0 | (unsigned char) ((uc16 >> 6) & 0x1F); /* upper 5 bits */
			*t++ = 0x80 | (unsigned char) (uc16 & 0x3F);        /* lower 6 bits */
		} else {
			*t++ = 0xE0 | (unsigned char) ((uc16 >> 12) & 0x0F);/* upper 4 bits */
			*t++ = 0x80 | (unsigned char) ((uc16 >> 6) & 0x3F); /* next 6 bits */
			*t++ = 0x80 | (unsigned char) (uc16 & 0x3F);        /* lowest 6 bits */
		}
	}
	*t = 0;
	return (t - s);
}

/* 
   UTF8 utility: 
   This map return the expected count of bytes based on the first char 
 */
const char utf8_len_map[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

#ifdef DEBUG_TEXT_SHAPING
/*
 *  Return the number of character (not byte) of UTF-8 string
 */
int utf8_nchar ( const char *str )
{
	int n = 0;
	int al = strlen ( str );

	while ( n < al )
		n += utf8_len_map[(unsigned char)str[n]];
	return (n < al) ? n : al;
}

#endif	

#define HAVE_SHAPEJOINING_SUPPORT 1

#if HAVE_SHAPEJOINING_SUPPORT
typedef struct char_shaped {
	unsigned short isolated;
	unsigned short initial;
	unsigned short medial;
	unsigned short final;
} chr_shpjoin_t;

/* This table start from a base of 0x0621, up to 0x06D3 */

#define SHAPED_TABLE_START	0x0621
#define SHAPED_TABLE_TOP	0x06D3

static const chr_shpjoin_t shaped_table[] =
{
	/*  base       s       i       m       f */
	{ /*0x0621*/ 0xFE80, 0x0000, 0x0000, 0x0000, },  /* HAMZA */
	{ /*0x0622*/ 0xFE81, 0x0000, 0x0000, 0xFE82, },  /* ALEF_MADDA */
	{ /*0x0623*/ 0xFE83, 0x0000, 0x0000, 0xFE84, },  /* ALEF_HAMZA_ABOVE */
	{ /*0x0624*/ 0xFE85, 0x0000, 0x0000, 0xFE86, },  /* WAW_HAMZA */
	{ /*0x0625*/ 0xFE87, 0x0000, 0x0000, 0xFE88, },  /* ALEF_HAMZA_BELOW */
	{ /*0x0626*/ 0xFE89, 0xFE8B, 0xFE8C, 0xFE8A, },  /* YEH_HAMZA */
	{ /*0x0627*/ 0xFE8D, 0x0000, 0x0000, 0xFE8E, },  /* ALEF */
	{ /*0x0628*/ 0xFE8F, 0xFE91, 0xFE92, 0xFE90, },  /* BEH */
	{ /*0x0629*/ 0xFE93, 0x0000, 0x0000, 0xFE94, },  /* TEH_MARBUTA */
	{ /*0x062A*/ 0xFE95, 0xFE97, 0xFE98, 0xFE96, },  /* TEH */
	{ /*0x062B*/ 0xFE99, 0xFE9B, 0xFE9C, 0xFE9A, },  /* THEH */
	{ /*0x062C*/ 0xFE9D, 0xFE9F, 0xFEA0, 0xFE9E, },  /* JEEM */
	{ /*0x062D*/ 0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2, },  /* HAH */
	{ /*0x062E*/ 0xFEA5, 0xFEA7, 0xFEA8, 0xFEA6, },  /* KHAH */
	{ /*0x062F*/ 0xFEA9, 0x0000, 0x0000, 0xFEAA, },  /* DAL */
	{ /*0x0630*/ 0xFEAB, 0x0000, 0x0000, 0xFEAC, },  /* THAL */
	{ /*0x0631*/ 0xFEAD, 0x0000, 0x0000, 0xFEAE, },  /* REH */
	{ /*0x0632*/ 0xFEAF, 0x0000, 0x0000, 0xFEB0, },  /* ZAIN */
	{ /*0x0633*/ 0xFEB1, 0xFEB3, 0xFEB4, 0xFEB2, },  /* SEEN */
	{ /*0x0634*/ 0xFEB5, 0xFEB7, 0xFEB8, 0xFEB6, },  /* SHEEN */
	{ /*0x0635*/ 0xFEB9, 0xFEBB, 0xFEBC, 0xFEBA, },  /* SAD */
	{ /*0x0636*/ 0xFEBD, 0xFEBF, 0xFEC0, 0xFEBE, },  /* DAD */
	{ /*0x0637*/ 0xFEC1, 0xFEC3, 0xFEC4, 0xFEC2, },  /* TAH */
	{ /*0x0638*/ 0xFEC5, 0xFEC7, 0xFEC8, 0xFEC6, },  /* ZAH */
	{ /*0x0639*/ 0xFEC9, 0xFECB, 0xFECC, 0xFECA, },  /* AIN */
	{ /*0x063A*/ 0xFECD, 0xFECF, 0xFED0, 0xFECE, },  /* GHAIN */
	{ /*0x063B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x063C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x063D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x063E*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x063F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0640*/ 0x0640, 0x0640, 0x0640, 0x0640, },  /* TATWEEL */
	{ /*0x0641*/ 0xFED1, 0xFED3, 0xFED4, 0xFED2, },  /* FEH */
	{ /*0x0642*/ 0xFED5, 0xFED7, 0xFED8, 0xFED6, },  /* QAF */
	{ /*0x0643*/ 0xFED9, 0xFEDB, 0xFEDC, 0xFEDA, },  /* KAF */
	{ /*0x0644*/ 0xFEDD, 0xFEDF, 0xFEE0, 0xFEDE, },  /* LAM */
	{ /*0x0645*/ 0xFEE1, 0xFEE3, 0xFEE4, 0xFEE2, },  /* MEEM */
	{ /*0x0646*/ 0xFEE5, 0xFEE7, 0xFEE8, 0xFEE6, },  /* NOON */
	{ /*0x0647*/ 0xFEE9, 0xFEEB, 0xFEEC, 0xFEEA, },  /* HEH */
	{ /*0x0648*/ 0xFEED, 0x0000, 0x0000, 0xFEEE, },  /* WAW */
	{ /*0x0649*/ 0xFEEF, 0xFBE8, 0xFBE9, 0xFEF0, },  /* ALEF_MAKSURA */
	{ /*0x064A*/ 0xFEF1, 0xFEF3, 0xFEF4, 0xFEF2, },  /* YEH */
	{ /*0x064B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x064C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x064D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x064E*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x064F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0650*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0651*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0652*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0653*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0654*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0655*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0656*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0657*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0658*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0659*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065A*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065E*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x065F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0660*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0661*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0662*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0663*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0664*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0665*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0666*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0667*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0668*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0669*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066A*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066E*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x066F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0670*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0671*/ 0xFB50, 0x0000, 0x0000, 0xFB51, },
	{ /*0x0672*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0673*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0674*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0675*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0676*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0677*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0678*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0679*/ 0xFB66, 0xFB68, 0xFB69, 0xFB67, },
	{ /*0x067A*/ 0xFB5E, 0xFB60, 0xFB61, 0xFB5F, },
	{ /*0x067B*/ 0xFB52, 0xFB54, 0xFB55, 0xFB53, },
	{ /*0x067C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x067D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x067E*/ 0xFB56, 0xFB58, 0xFB59, 0xFB57, },
	{ /*0x067F*/ 0xFB62, 0xFB64, 0xFB65, 0xFB63, },
	{ /*0x0680*/ 0xFB5A, 0xFB5C, 0xFB5D, 0xFB5B, },
	{ /*0x0681*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0682*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0683*/ 0xFB76, 0xFB78, 0xFB79, 0xFB77, },
	{ /*0x0684*/ 0xFB72, 0xFB74, 0xFB75, 0xFB73, },
	{ /*0x0685*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0686*/ 0xFB7A, 0xFB7C, 0xFB7D, 0xFB7B, },
	{ /*0x0687*/ 0xFB7E, 0xFB80, 0xFB81, 0xFB7F, },
	{ /*0x0688*/ 0xFB88, 0x0000, 0x0000, 0xFB89, },
	{ /*0x0689*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x068A*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x068B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x068C*/ 0xFB84, 0x0000, 0x0000, 0xFB85, },
	{ /*0x068D*/ 0xFB82, 0x0000, 0x0000, 0xFB83, },
	{ /*0x068E*/ 0xFB86, 0x0000, 0x0000, 0xFB87, },
	{ /*0x068F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0690*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0691*/ 0xFB8C, 0x0000, 0x0000, 0xFB8D, },
	{ /*0x0692*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0693*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0694*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0695*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0696*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0697*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x0698*/ 0xFB8A, 0x0000, 0x0000, 0xFB8B, },
	{ /*0x0699*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069A*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069B*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069C*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069D*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069E*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x069F*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A0*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A1*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A2*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A3*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A4*/ 0xFB6A, 0xFB6C, 0xFB6D, 0xFB6B, },
	{ /*0x06A5*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A6*/ 0xFB6E, 0xFB70, 0xFB71, 0xFB6F, },
	{ /*0x06A7*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A8*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06A9*/ 0xFB8E, 0xFB90, 0xFB91, 0xFB8F, },
	{ /*0x06AA*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06AB*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06AC*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06AD*/ 0xFBD3, 0xFBD5, 0xFBD6, 0xFBD4, },
	{ /*0x06AE*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06AF*/ 0xFB92, 0xFB94, 0xFB95, 0xFB93, },
	{ /*0x06B0*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B1*/ 0xFB9A, 0xFB9C, 0xFB9D, 0xFB9B, },
	{ /*0x06B2*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B3*/ 0xFB96, 0xFB98, 0xFB99, 0xFB97, },
	{ /*0x06B4*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B5*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B6*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B7*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B8*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06B9*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06BA*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06BB*/ 0xFBA0, 0xFBA2, 0xFBA3, 0xFBA1, },
	{ /*0x06BC*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06BD*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06BE*/ 0xFBAA, 0xFBAC, 0xFBAD, 0xFBAB, },
	{ /*0x06BF*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06C0*/ 0xFBA4, 0x0000, 0x0000, 0xFBA5, },
	{ /*0x06C1*/ 0xFBA6, 0xFBA8, 0xFBA9, 0xFBA7, },
	{ /*0x06C2*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06C3*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06C4*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06C5*/ 0xFBE0, 0x0000, 0x0000, 0xFBE1, },
	{ /*0x06C6*/ 0xFBD9, 0x0000, 0x0000, 0xFBDA, },
	{ /*0x06C7*/ 0xFBD7, 0x0000, 0x0000, 0xFBD8, },
	{ /*0x06C8*/ 0xFBDB, 0x0000, 0x0000, 0xFBDC, },
	{ /*0x06C9*/ 0xFBE2, 0x0000, 0x0000, 0xFBE3, },
	{ /*0x06CA*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06CB*/ 0xFBDE, 0x0000, 0x0000, 0xFBDF, },
	{ /*0x06CC*/ 0xFBFC, 0xFBFE, 0xFBFF, 0xFBFD, },
	{ /*0x06CD*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06CE*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06CF*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06D0*/ 0xFBE4, 0xFBE6, 0xFBE7, 0xFBE5, },
	{ /*0x06D1*/ 0x0000, 0x0000, 0x0000, 0x0000, },  /* dummy filler */
	{ /*0x06D2*/ 0xFBAE, 0x0000, 0x0000, 0xFBAF, },
	{ /*0x06D3*/ 0xFBB0, 0x0000, 0x0000, 0xFBB1, },
};

#define SHAPED_TABLE2_START	0xFEF5
#define SHAPED_TABLE2_TOP	0xFEFB

/*
 * The second table is for special ligatures
 */
static const chr_shpjoin_t shaped_table2[] =
{
	{ /*0xFEF5*/ 0xFEF5, 0x0000, 0x0000, 0xFEF6, }, /* LAM_ALEF_MADDA */
	{ /*0xFEF6*/ 0x0000, 0x0000, 0x0000, 0x0000, }, /* dummy filler */
	{ /*0xFEF7*/ 0xFEF7, 0x0000, 0x0000, 0xFEF8, }, /* LAM_ALEF_HAMZA_ABOVE */
	{ /*0xFEF8*/ 0x0000, 0x0000, 0x0000, 0x0000, }, /* dummy filler */
	{ /*0xFEF9*/ 0xFEF9, 0x0000, 0x0000, 0xFEFA, }, /* LAM_ALEF_HAMZA_BELOW */
	{ /*0xFEFA*/ 0x0000, 0x0000, 0x0000, 0x0000, }, /* dummy filler */
	{ /*0xFEFB*/ 0xFEFB, 0x0000, 0x0000, 0xFEFC, }, /* LAM_ALEF */
};

#define assignShape(chr)	( ((chr) >= SHAPED_TABLE_START  && (chr) <= SHAPED_TABLE_TOP)? \
				    &shaped_table[(chr)-SHAPED_TABLE_START] : \
                                  ((chr) >= SHAPED_TABLE2_START && (chr) <= SHAPED_TABLE2_TOP)? \
				    &shaped_table2[(chr)-SHAPED_TABLE2_START] : NULL) 

#define assignShapeUtf(txt, i) ( (utf8_len_map[(unsigned char)((txt)[(i)])] > 1)? \
					doAssignShapeUtf((txt)+(i)) : NULL)

static const chr_shpjoin_t *
doAssignShapeUtf(const char *txt)
{
	unsigned short fs;

	utf8_to_utf16((const unsigned char *) txt,
		      utf8_len_map[(unsigned char) txt[0]], &fs);
	return assignShape(fs);
}


static void
storeUc_2_Utf8(char *dest, int *psz, unsigned short wch)
{
	int cb = uc16_to_utf8(&wch, 1, dest + (*psz));

	*psz = *psz + cb;
}


static void
store_Utf8(char *dest, int *psz, const char *txt)
{
	int cb = utf8_len_map[(unsigned char) txt[0]];

	memcpy(dest + (*psz), txt, cb);
	*psz = *psz + cb;
}

/*
 * Note that text is currently left to right
 */
static unsigned short *
arabicJoin_UC16(const unsigned short *text, int len, unsigned long *pAttrib)
{
	int i;
	unsigned short *new_str;
	const chr_shpjoin_t *prev = NULL;
	const chr_shpjoin_t *curr = NULL;
	const chr_shpjoin_t *next = NULL;
	unsigned long attrib = 0;

	new_str = (unsigned short *) malloc((1 + len) * sizeof(unsigned short));
	if (new_str == NULL)
		return NULL;

	for (i = 0; i < len; i++) {
		if ((curr = assignShape(text[i])) != NULL) {
			if (i < len - 1)
				next = assignShape(text[i + 1]);
			else
				next = NULL;
			if (next) {
				if (prev) {
					if (!prev->initial || !prev->medial)
						new_str[i] = curr->initial ?
							curr->initial : curr->isolated;
					else
						new_str[i] = curr->medial ?
							curr->medial : curr->final;
				} else {
					new_str[i] = curr->initial ?
						curr->initial : curr->isolated;
				}
			} else {
				if (prev) {
					if (!prev->initial || !prev->medial)
						new_str[i] = curr->isolated;
					else
						new_str[i] = curr->final ?
							curr->final : curr->isolated;
				} else {
					new_str[i] = curr->isolated;
				}
			}
			attrib |= (FTK_TEXT_ATTR_SHAPED | FTK_TEXT_ATTR_EXTENDED);
		} else {
			new_str[i] = text[i];
			if (text[i] <= 0xFF)
				attrib |= FTK_TEXT_ATTR_STANDARD;
			else
				attrib |= FTK_TEXT_ATTR_EXTENDED;
		}

		prev = curr;
	}
	new_str[i] = 0;
	if (pAttrib)
		*pAttrib = attrib;
	return new_str;
}

/*
 * Note that text is currently left to right
 */
char *
arabicJoin_UTF8(const char *text, int len, int *pNewLen,
		unsigned long *pAttrib)
{
	int i, sz;
	char *new_str;
	const chr_shpjoin_t *prev = NULL;
	const chr_shpjoin_t *curr = NULL;
	const chr_shpjoin_t *next = NULL;
	unsigned long attrib = 0;

	/* Note that shaping may result in three UTF-8 bytes, due to 06xx -> FBxx translation*/
	/* two times the original buffer should be enough...*/
	new_str = (char *) malloc((1 + 2 * len) * sizeof(char));
	if (new_str == NULL)
		return NULL;

	sz = 0;

	for (i = 0; i < len;) {
		int b = utf8_len_map[(unsigned char) text[i]];
		if ((curr = assignShapeUtf(text, i)) != NULL) {
			if (i < len - b)
				next = assignShapeUtf(text, i + b);
			else
				next = NULL;
			if (next) {
				if (prev) {
					if (!prev->initial || !prev->medial)
						storeUc_2_Utf8(new_str, &sz,
							       (curr->initial ? curr->initial :
								curr->isolated));
					else
						storeUc_2_Utf8(new_str, &sz,
							       (curr->medial ? curr->medial :
								curr->final));
				} else {
					storeUc_2_Utf8(new_str, &sz, (curr->initial ?
							curr->initial : curr-> isolated));
				}
			} else {
				if (prev) {
					if (!prev->initial || !prev->medial)
						storeUc_2_Utf8(new_str, &sz, curr->isolated);
					else
						storeUc_2_Utf8(new_str, &sz,
							       (curr->final ? curr->final :
							       curr->isolated));
				} else {
					storeUc_2_Utf8(new_str, &sz, curr->isolated);
				}
			}
			attrib |= (FTK_TEXT_ATTR_SHAPED | FTK_TEXT_ATTR_EXTENDED);
		} else {
			store_Utf8(new_str, &sz, text + i);
			if ((unsigned char) text[i] < 0xC0)
				attrib |= FTK_TEXT_ATTR_STANDARD;
			else
				attrib |= FTK_TEXT_ATTR_EXTENDED;
		}

		i += b;
		prev = curr;
	}
	new_str[sz] = 0;
	if (pNewLen)
		*pNewLen = sz;
	if (pAttrib)
		*pAttrib = attrib;
#ifdef DEBUG_TEXT_SHAPING
#endif
	return new_str;
}

unsigned short *
doCharShape_UC16(const unsigned short *text, int len, int *pNewLen,
	unsigned long *pAttrib)
{
	unsigned short *conv = arabicJoin_UC16(text, len, pAttrib);

	if (pNewLen)
		*pNewLen = len;
	return conv;
}

char *
doCharShape_UTF8(const char *text, int len, int *pNewLen, unsigned long *pAttrib)
{
	return arabicJoin_UTF8(text, len, pNewLen, pAttrib);
}

#else /* HAVE_SHAPEJOINING_SUPPORT */
/* DUMMY FUNCTIONS */
unsigned short *
doCharShape_UC16(const unsigned short *text, int len, int *pNewLen,
	unsigned long *pAttrib)
{
	unsigned short *conv = malloc((len + 1) * sizeof(unsigned short));

	if (conv == NULL)
		return NULL;
	memcpy(conv, text, len * sizeof(unsigned short));
	conv[len] = 0;
	if (pNewLen)
		*pNewLen = len;
	if (pAttrib)
		*pAttrib = 0;
	return conv;
}

char *
doCharShape_UTF8(const char *text, int len, int *pNewLen, unsigned long *pAttrib)
{
	char *conv = malloc((len + 1) * sizeof(char));

	if (conv == NULL)
		return NULL;
	memcpy(conv, text, len * sizeof(char));
	conv[len] = 0;
	if (pNewLen)
		*pNewLen = len;
	if (pAttrib)
		*pAttrib = 0;
	return conv;
}
#endif /* HAVE_SHAPEJOINING_SUPPORT */

#define HAVE_FRIBIDI_SUPPORT 1
#if HAVE_FRIBIDI_SUPPORT
#include <fribidi/fribidi.h>

char *
doCharBidi_UTF8(const char *text, int len, int *v2lPos, char *pDirection,
	unsigned long *pAttrib)
{
	FriBidiChar *ftxt, *fvirt;
	FriBidiChar localBuff[128];
	FriBidiCharType basedir;
	int cc;
	int isLocal = 0;
	char *new_str;
	int new_len;

	new_str = (char *) malloc(len + 1);
	if (new_str == NULL)
		return NULL;

	/* len may be greather than real char count, but it's ok.
	   if will fit in localBuff, we use it to improve speed */
	if (len < sizeof(localBuff) / sizeof(localBuff[0]) / 2) {
		ftxt = localBuff;
		fvirt = localBuff +
			sizeof(localBuff) / sizeof(localBuff[0]) / 2;
		isLocal = 1;
	} else {
		ftxt = (FriBidiChar *) malloc((len + 1) * sizeof(FriBidiChar));
		fvirt = (FriBidiChar *) malloc((len + 1) * sizeof(FriBidiChar));
	}

	if (ftxt == NULL)
		return NULL;
	if (fvirt == NULL) {
		free(ftxt);
		return NULL;
	}

	cc = fribidi_utf8_to_unicode((char *) text, len, ftxt);
	basedir = FRIBIDI_TYPE_N;
	fribidi_log2vis(ftxt, cc, &basedir, fvirt, v2lPos, NULL, pDirection);
	new_len = fribidi_unicode_to_utf8(fvirt, cc, new_str);

	if (pAttrib) {
		if (basedir & FRIBIDI_MASK_RTL)
			*pAttrib |= FTK_TEXT_ATTR_RTOL;
	}

	if (!isLocal) {
		free(fvirt);
		free(ftxt);
	}
	new_str[new_len] = 0;
	return new_str;
}


unsigned short *
doCharBidi_UC16(const unsigned short *text, int len, int *v2lPos,
	char *pDirection, unsigned long *pAttrib)
{
	FriBidiChar *ftxt, *fvirt;
	FriBidiChar localBuff[128];
	FriBidiCharType basedir;
	int cc;
	int isLocal = 0;
	unsigned short *new_str;

	new_str = (unsigned short *) malloc((len + 1) * sizeof(unsigned short));
	if (new_str == NULL)
		return NULL;

	/* len may be greather than real char count, but it's ok.
	   if will fit in localBuff, we use it to improve speed */
	if (len < sizeof(localBuff) / sizeof(localBuff[0]) / 2) {
		ftxt = localBuff;
		fvirt = localBuff +
			sizeof(localBuff) / sizeof(localBuff[0]) / 2;
		isLocal = 1;
	} else {
		ftxt = (FriBidiChar *) malloc((len + 1) * sizeof(FriBidiChar));
		fvirt = (FriBidiChar *) malloc((len + 1) * sizeof(FriBidiChar));
	}

	if (ftxt == NULL)
		return NULL;
	if (fvirt == NULL) {
		free(ftxt);
		return NULL;
	}

	for (cc = 0; cc < len; cc++)
		ftxt[cc] = text[cc];
	basedir = FRIBIDI_TYPE_N;
	fribidi_log2vis(ftxt, cc, &basedir, fvirt, v2lPos, NULL, pDirection);
	for (cc = 0; cc < len; cc++)
		new_str[cc] = (unsigned short) fvirt[cc];
	new_str[cc] = 0;

	if (pAttrib) {
		if (basedir & FRIBIDI_MASK_RTL)
			*pAttrib |= FTK_TEXT_ATTR_RTOL;
	}

	if (!isLocal) {
		free(fvirt);
		free(ftxt);
	}
	return new_str;
}

#else
/* DUMMY FUNCTIONS */
char *
doCharBidi_UTF8(const char *text, int len, int *v2lPos, char *pDirection,
	unsigned long *pAttrib)
{
	int i;
	unsigned short *conv = malloc((len + 1) * sizeof(unsigned short));

	if (conv == NULL)
		return NULL;
	memcpy(conv, text, len * sizeof(unsigned short));
	conv[len] = 0;
	if (v2lPos)
		for (i = 0; i < len; i++)
			v2lPos[i] = i;
	if (pDirection)
		memset(pDirection, 0, len * sizeof(pDirection[0]));
	return (char *) conv;
}
unsigned short *
doCharBidi_UC16(const unsigned short *text, int len, int *v2lPos,
	char *pDirection, unsigned long *pAttrib)
{
	int i;
	char *conv = malloc((len + 1) * sizeof(char));

	if (conv == NULL)
		return NULL;
	memcpy(conv, text, len * sizeof(char));
	conv[len] = 0;
	if (v2lPos)
		for (i = 0; i < len; i++)
			v2lPos[i] = i;
	if (pDirection)
		memset(pDirection, 0, len * sizeof(pDirection[0]));
	return (unsigned short *) conv;
}
#endif /* HAVE_FRIBIDI_SUPPORT */

struct _FtkTextLayout
{
	int pos;
	int len;
	size_t width;
	FtkFont* font;
	char* text;
	int line_len;
	int line_pos;
	char* bidi_line;
	unsigned long line_attr;
	FtkWrapMode wrap_mode;
	int v2l_v_line[FTK_LINE_CHAR_NR+1];
	int v2l_l_line[8 * FTK_LINE_CHAR_NR+1];
};

FtkTextLayout* ftk_text_layout_create(void)
{
	FtkTextLayout* thiz = FTK_NEW(FtkTextLayout);

	return thiz;
}

Ret ftk_text_layout_set_font(FtkTextLayout* thiz, FtkFont* font)
{
	return_val_if_fail(thiz != NULL && font != NULL, RET_FAIL);

	thiz->font = font;

	return RET_OK;
}

Ret ftk_text_layout_set_width(FtkTextLayout* thiz, size_t width)
{
	return_val_if_fail(thiz != NULL && width > 0, RET_FAIL);

	thiz->width = width;

	return RET_OK;
}

Ret ftk_text_layout_set_text(FtkTextLayout* thiz, const char* text, int len)
{
	unsigned long attr = 0;
	return_val_if_fail(thiz != NULL && text != NULL, RET_FAIL);

	if(thiz->text != NULL)
	{
		free(thiz->text);
		thiz->text = NULL;
	}

	thiz->pos  = 0;
	thiz->len  = len < 0 ? strlen(text) : len;
	thiz->text = doCharShape_UTF8(text, thiz->len, &thiz->len, &attr);

	return RET_OK;
}

Ret ftk_text_layout_set_wrap_mode(FtkTextLayout* thiz, FtkWrapMode wrap_mode)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	thiz->wrap_mode = wrap_mode;

	return RET_OK;
}

Ret ftk_text_layout_init(FtkTextLayout* thiz, const char* text, int len, FtkFont* font, size_t width)
{
	return_val_if_fail(thiz != NULL && text != NULL && font != NULL && width > 0, RET_FAIL);

	thiz->pos   = 0;
	thiz->font  = font;
	thiz->width = width;
	ftk_text_layout_set_text(thiz, text, len);

	return RET_OK;
}

Ret ftk_text_layout_skip_to(FtkTextLayout* thiz, int pos)
{
	return_val_if_fail(thiz != NULL && pos >= 0 && pos < thiz->len, RET_FAIL);

	thiz->pos = pos;

	return RET_OK;
}

Ret ftk_text_layout_get_visual_line(FtkTextLayout* thiz, FtkTextLine* line)
{
	int i = 0;
	int extent = 0;
	unsigned long attr = 0;
	const char* end = NULL;
	
	return_val_if_fail(thiz != NULL && line != NULL, RET_FAIL);

	if(thiz->pos >= thiz->len && thiz->line_pos >= thiz->line_len)
	{
		return RET_EOF;
	}

	line->pos_v2l = thiz->v2l_v_line;
	line->attr = FTK_TEXT_ATTR_NORMAL;

	if(thiz->line_pos >= thiz->line_len)
	{
		int line_len = 0;
		const char* line_start = thiz->text + thiz->pos;
		const char* line_end = line_start;
	
		thiz->line_pos = 0;
		while((line_end - thiz->text) <= thiz->len)
		{
			if(*line_end == '\n' || *line_end == '\0') break;
			line_end++;
		}
		line_len = line_end - line_start;
		thiz->pos += line_len;
		
		if(thiz->bidi_line != NULL)
		{
			free(thiz->bidi_line);
		}

		thiz->bidi_line = doCharBidi_UTF8(line_start, line_len, thiz->v2l_l_line, NULL, &attr);
		thiz->line_len = strlen(thiz->bidi_line);
		thiz->line_attr = attr;
	}

	line->text = thiz->bidi_line + thiz->line_pos;
	end = ftk_font_calc_str_visible_range(thiz->font, line->text, 0, -1, thiz->width, &extent);
	line->len = end - line->text;
	line->extent = extent;

	line->attr = thiz->line_attr;
	if(thiz->line_attr & FTK_TEXT_ATTR_RTOL)
	{
		line->xoffset = thiz->width - extent;
		for(i = 0; i < line->len && i < FTK_LINE_CHAR_NR; i++)
		{
			line->pos_v2l[i] =  thiz->v2l_v_line[thiz->line_pos + i] + thiz->pos - thiz->line_len;
		}
	}
	else
	{
		line->xoffset = 0;
		for(i = 0; i < line->len && i < FTK_LINE_CHAR_NR; i++)
		{
			line->pos_v2l[i] = thiz->pos - thiz->line_len + 1;
		}
	}

	ftk_logd("%s: line_pos=%d line_len=%d line.len=%d attr=%x\n", __func__, thiz->line_pos, thiz->line_len, line->len, attr);
	thiz->line_pos += line->len;

	for(i = line->len - 1; i >= 0; i--)
	{
		if(line->text[i] == '\r' || line->text[i] == '\n')
			line->len--;
	}

	return RET_OK;
}

void ftk_text_layout_destroy(FtkTextLayout* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}
