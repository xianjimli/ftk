/*
 * File: ftk_util.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   common used functions.
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

#include "ftk_log.h"
#include "ftk_util.h"

/*UTF8-related functions are copied from glib.*/

#define UTF8_COMPUTE(Char, Mask, Len)					      \
  if (Char < 128)							      \
    {									      \
      Len = 1;								      \
      Mask = 0x7f;							      \
    }									      \
  else if ((Char & 0xe0) == 0xc0)					      \
    {									      \
      Len = 2;								      \
      Mask = 0x1f;							      \
    }									      \
  else if ((Char & 0xf0) == 0xe0)					      \
    {									      \
      Len = 3;								      \
      Mask = 0x0f;							      \
    }									      \
  else if ((Char & 0xf8) == 0xf0)					      \
    {									      \
      Len = 4;								      \
      Mask = 0x07;							      \
    }									      \
  else if ((Char & 0xfc) == 0xf8)					      \
    {									      \
      Len = 5;								      \
      Mask = 0x03;							      \
    }									      \
  else if ((Char & 0xfe) == 0xfc)					      \
    {									      \
      Len = 6;								      \
      Mask = 0x01;							      \
    }									      \
  else									      \
    Len = -1;

#define UTF8_LENGTH(Char)              \
  ((Char) < 0x80 ? 1 :                 \
   ((Char) < 0x800 ? 2 :               \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :          \
      ((Char) < 0x4000000 ? 5 : 6)))))
   

#define UTF8_GET(Result, Chars, Count, Mask, Len)			      \
  (Result) = (Chars)[0] & (Mask);					      \
  for ((Count) = 1; (Count) < (Len); ++(Count))				      \
    {									      \
      if (((Chars)[(Count)] & 0xc0) != 0x80)				      \
	{								      \
	  (Result) = -1;						      \
	  break;							      \
	}								      \
      (Result) <<= 6;							      \
      (Result) |= ((Chars)[(Count)] & 0x3f);				      \
    }

#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
     (((Char) & 0xFFFFF800) != 0xD800) &&     \
     ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&  \
     ((Char) & 0xFFFE) != 0xFFFE)
static const char utf8_skip_data[256] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

const char * const g_utf8_skip = utf8_skip_data;
#define g_utf8_next_char(p) (char *)((p) + g_utf8_skip[*(const unsigned char *)(p)])

unsigned short utf8_get_char (const char *p, const char** next)
{
  int i, mask = 0, len;
  unsigned short result;
  unsigned char c = (unsigned char) *p;

  UTF8_COMPUTE (c, mask, len);
  if (len == -1)
    return (unsigned short)-1;
  UTF8_GET (result, p, i, mask, len);

  if(next != NULL)
  {
    *next = g_utf8_next_char(p);
  }

  return result;
}

unsigned short utf8_get_prev_char (const char *p, const char** prev)
{
	int i = 0;
	for(i = 1; i < 8; i++)
	{
		unsigned char val = p[-i];
		if((val & 0x80) && !(val & 0x40))
		{
			continue;
		}
		else
		{
			if(prev != NULL)
			{
				*prev = p-i;
			}
			return utf8_get_char(p-i, NULL);
		}
	}

	if(prev != NULL)
	{
		*prev = p;
	}

	return 0;
}

int utf8_count_char(const char *str, int length)
{
	int nr = 0;
	const char* iter = str;
	return_val_if_fail(str != NULL, 0);

	while(utf8_get_char(iter, &iter) && (iter - str) <= (int)length)
	{
		nr++;
	}

	return nr;
}

int unichar_to_utf8 (unsigned short c, char* outbuf)
{
  /* If this gets modified, also update the copy in g_string_insert_unichar() */
  size_t len = 0;    
  int first;
  int i;

  if (c < 0x80)
    {
      first = 0;
      len = 1;
    }
  else if (c < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
  else if (c < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
   else if (c < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
  else if (c < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
  else
    {
      first = 0xfc;
      len = 6;
    }

  if (outbuf)
    {
      for (i = len - 1; i > 0; --i)
	{
	  outbuf[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      outbuf[0] = c | first;
    }

  return len;
}

#define SURROGATE_VALUE(h,l) (((h) - 0xd800) * 0x400 + (l) - 0xdc00 + 0x10000)
char* utf16_to_utf8 (const unsigned short  *str, long len, char* utf8, int out_len)
{
  /* This function and g_utf16_to_ucs4 are almost exactly identical - The lines that differ
   * are marked.
   */
  const unsigned short *in;
  char *out;
  char *result = NULL;
  int n_bytes;
  unsigned short high_surrogate;

  return_val_if_fail (str != NULL, NULL);

  n_bytes = 0;
  in = str;
  high_surrogate = 0;
  while ((len < 0 || in - str < len) && *in)
    {
      unsigned short c = *in;
      unsigned short wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  if (high_surrogate)
	    {
	      wc = SURROGATE_VALUE (high_surrogate, c);
	      high_surrogate = 0;
	    }
	  else
	    {
          ftk_loge("Invalid sequence in conversion input");
	      goto err_out;
	    }
	}
      else
	{
	  if (high_surrogate)
	    {
          ftk_loge("Invalid sequence in conversion input");
	      goto err_out;
	    }

	  if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	    {
	      high_surrogate = c;
	      goto next1;
	    }
	  else
	    wc = c;
	}

      /********** DIFFERENT for UTF8/UCS4 **********/
      n_bytes += UTF8_LENGTH (wc);

    next1:
      in++;
    }

  if (high_surrogate)
    {
      ftk_loge("Partial character sequence at end of input");
      goto err_out;
    }
  
  /* At this point, everything is valid, and we just need to convert
   */
  /********** DIFFERENT for UTF8/UCS4 **********/
  //result = g_malloc (n_bytes + 1);
  result = utf8;
  assert(out_len > n_bytes);

  high_surrogate = 0;
  out = result;
  in = str;
  while (out < result + n_bytes)
    {
      unsigned short c = *in;
      unsigned short wc;

      if (c >= 0xdc00 && c < 0xe000) /* low surrogate */
	{
	  wc = SURROGATE_VALUE (high_surrogate, c);
	  high_surrogate = 0;
	}
      else if (c >= 0xd800 && c < 0xdc00) /* high surrogate */
	{
	  high_surrogate = c;
	  goto next2;
	}
      else
	wc = c;

      /********** DIFFERENT for UTF8/UCS4 **********/
      out += unichar_to_utf8 (wc, out);

    next2:
      in++;
    }
  
  /********** DIFFERENT for UTF8/UCS4 **********/
  *out = '\0';
  
  return result;
err_out:
  return NULL;
}

static int ftk_hex_to_int(char c)
{
	if(c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if(c >= 'A' && c <= 'F')
	{
		return c - 'A' + 0x0A;
	}
	else if(c >= 'a' && c <= 'f')
	{
		return c - 'a' + 0x0a;
	}

	return 0;
}

static int ftk_parse_color_1(const char* value)
{
	return ftk_hex_to_int(value[0]) * 16 + ftk_hex_to_int(value[1]);	
}

FtkColor ftk_parse_color( const char* value)
{
	FtkColor color = {0};
	return_val_if_fail(value != NULL && strlen(value) >= 8, color);

	color.a = ftk_parse_color_1(value);
	color.r = ftk_parse_color_1(value + 2);
	color.g = ftk_parse_color_1(value + 4);
	color.b = ftk_parse_color_1(value + 6);

	return color;
}

#define IS_CURRENT(path) (((path)[0] == '.') && \
	((path)[1] == '/' || ((path)[1] == '\\') || ((path)[1] == '\0')))
#define IS_HOME(path) (((path)[0] == '~') && \
	((path)[1] == '/' || ((path)[1] == '\\') || ((path)[1] == '\0')))
#define IS_PARENT(path) (((path)[0] == '.') && ((path)[1] == '.') && \
	((path)[2] == '/' || ((path)[2] == '\\') || ((path)[2] == '\0') ))

#define BREAK_IF_LAST(str) if((str)[0] == '\0') break;

char* normalize_path(const char* path_in, char path_out[FTK_MAX_PATH+1])
{
	int i = 0;
	int in_index = 0;
	int out_index = 0;

	return_val_if_fail(path_in != NULL && path_out != NULL, NULL);
	
	path_out[0] = '\0';
	for(in_index = 0; path_in[in_index] != '\0'; in_index++)
	{
		if(in_index == 0)
		{
			if(IS_CURRENT(path_in)) 
			{
				ftk_getcwd(path_out, FTK_MAX_PATH);
				out_index = strlen(path_out);
				continue;
			}
#ifdef LINUX			
			else if(IS_HOME(path_in))
			{
				const char* home = getenv("HOME");
				if(home != NULL)
				{
					ftk_strcpy(path_out, home);
					out_index = strlen(path_out);
				}
				continue;
			}	
			else if(path_in[0] != '/')
			{
				ftk_getcwd(path_out, FTK_MAX_PATH);
				out_index = strlen(path_out);
				path_out[out_index++] = '/';
				path_out[out_index++] = path_in[in_index];
				continue;
			}
#endif		
		}

		if(path_in[in_index] == '\\' || path_in[in_index] == '/')
		{
			if(out_index == 0 || path_out[out_index - 1] != '/')
			{
				path_out[out_index++] = '/';
			}
		}
		else if(IS_CURRENT(path_in+in_index) || IS_HOME(path_in+in_index))
		{
			in_index++;
			BREAK_IF_LAST(path_in+in_index);
		}
		else if(IS_PARENT(path_in+in_index))
		{
			if(out_index > 1)
			{
				if(path_out[out_index - 1] == '/')
				{
					for(--out_index; path_out[out_index - 1] != '/'; out_index--);
				}
				else
				{
					ftk_logd("%s:%d %s is invalid path\n", __FILE__, __LINE__, path_in);
					in_index += 2;
				}
			}
			else
			{
				ftk_logd("%s:%d %s is invalid path\n", __FILE__, __LINE__, path_in);
				in_index += 2;
			}
			BREAK_IF_LAST(path_in+in_index);
		}
		else 
		{
			path_out[out_index++] = path_in[in_index];
		}

		if(out_index >= FTK_MAX_PATH)
		{
			break;
		}
	}

	path_out[out_index] = '\0';

	for(i = 0; i < out_index; i++)
	{
		if(path_out[i] == '\\' || path_out[i] == '/')
		{
			path_out[i] = FTK_PATH_DELIM;
		}
	}

	return path_out;
}

const char* ftk_normalize_path(char path[FTK_MAX_PATH+1])
{
	char path_out[FTK_MAX_PATH+1] = {0};
	return_val_if_fail(path != NULL, NULL);

	normalize_path(path, path_out);
	ftk_strncpy(path, path_out, FTK_MAX_PATH);

	return path;
}

const char* utf8_move_forward(const char* str, int nr)
{
	int i = 0;
	const char* next = str;

	for(i = 0; i < nr; i++)
	{
		utf8_get_char(next, &next);
	}

	return next;
}

#ifdef USE_LINEBREAK
#include "linebreak/linebreak.h"
const char* ftk_line_break(const char* start, const char* end)
{
	const char* p = end;
	const char* next = NULL;
	unsigned short c1 = 0;
	unsigned short c2 = 0;
	static int linebreak_inited = 0;

	if(linebreak_inited == 0)
	{
		init_linebreak();
		linebreak_inited = 1;
	}

	c2 = utf8_get_char(p, &next);
	c1 = utf8_get_prev_char(p, NULL);

	if(c1 != '\n' && c1 != '\r' && c2 != '\0' && c2 != '\n' && c2 != '\r')
	{
		size_t i = 0;
		char brks[256] = {0};
		size_t len = end - start + 1;
		assert(len < sizeof(brks));
	
		set_linebreaks_utf8((const utf8_t*)start, len, "zh", brks);
		
		i = len - 2;
		for(; i > 0; i--)
		{
			if(brks[i] == LINEBREAK_ALLOWBREAK || brks[i] == LINEBREAK_MUSTBREAK)
			{
				end = start + i + 1;
				break;
			}
		}

//		while((unsigned char)(*end) >= 0x80) end--;
	}

	return end;
}
#else
int ftk_can_break(unsigned short c1, unsigned short c2)
{
	if(c1 > 0x80 || c2 > 0x80)
	{
		return 1;
	}

	if(isdigit(c1) && isdigit(c2))
	{
		return 0;
	}
	
	if(isalpha(c1) && isalpha(c2))
	{
		return 0;
	}

	return 1;
}

const char* ftk_line_break(const char* start, const char* end)
{
	const char* p = end;
	const char* next = NULL;
	unsigned short c1 = 0;
	unsigned short c2 = 0;
	c2 = utf8_get_char(p, &next);
	c1 = utf8_get_prev_char(p, NULL);

	if(c1 != '\n' && c1 != '\r' && c2 != '\0' && c2 != '\n' && c2 != '\r')
	{
		while(!ftk_can_break(c1, c2) && p > start)
		{
			next = p;
			c2 = c1;
			c1 = utf8_get_prev_char(next, &p);
		}
	
		end = p;
	}

	return end;
}
#endif

int ftk_str2bool(const char* str)
{
	if(str == NULL || str[0] == '0' || strcmp(str, "false") == 0 || strcmp(str, "no") == 0)
	{
		return 0;
	}

	return 1;
}

char* ftk_strs_cat(char* str, int len, const char* first, ...)
{
	va_list arg;
	size_t dst = 0;
	const char* iter = first;
	return_val_if_fail(str != NULL && len > 0, NULL);

	va_start(arg, first); 
	while(iter != NULL && dst < len)
	{
		for(; dst < len && *iter; iter++, dst++)
		{
			str[dst] = *iter;
		}

		iter = va_arg(arg, char*);
	}
	va_end(arg); 

	if(dst < len)
	{	
		str[dst] = '\0';
	}
	else
	{
		str[len-1] = '\0';
	}

	return str;
}

static long  ftk_strtol_internal(const char* str, const char **end, int base)
{
	int i = 0;
	long n = 0;
	char c  = 0;
	return_val_if_fail(str != NULL && (base == 10 || base == 8 || base == 16), 0);

	if(base == 10)
	{
		for(i = 0; str[i] && i < 10; i++)
		{
			c = str[i];

			if(c < '0' || c > '9')
			{
				break;
			}

			n = n * base + c - '0';
		}
	}
	else if(base == 8)
	{
		for(i = 0; str[i] && i < 10; i++)
		{
			c = str[i];

			if(c < '0' || c > '7')
			{
				break;
			}

			n = n * base + c - '0';
		}
	}
	else if(base == 16)
	{
		for(i = 0; str[i] && i < 10; i++)
		{
			c = str[i];

			if((c >= '0' && c <= '9'))
			{
				c -= '0';
			}
			else if(c >= 'a' && c <= 'f')
			{
				c = c - 'a' + 10;
			}
			else if(c >= 'A' && c <= 'F')
			{
				c = c - 'A' + 10;
			}
			else
			{
				break;
			}

			n = n * base + c;
		}
	}

	if(end != NULL)
	{
		*end = str+i;
	}

	return n;
}

long  ftk_strtol(const char* str, const char **end, int base)
{
	long n = 0;
	int neg = 0;
	return_val_if_fail(str != NULL, 0);

	while(*str == ' ' || *str == '\t') str++;

	if(*str == '+' || *str == '-')
	{
		neg = *str == '-';
		str++;
	}

	n = ftk_strtol_internal(str, end, base);

	return neg ? -n : n;
}

int   ftk_atoi(const char* str)
{
	return  ftk_strtol(str, NULL, 10);
}

double ftk_atof(const char* str)
{
	int n = 0;
	int f = 0;
	int neg = 0;
	double result = 0;
	const char* p = NULL;
	return_val_if_fail(str != NULL, 0);

	if(str[0] == '+' || str[0] == '-')
	{
		neg = str[0] == '-';
		str++;
	}

	n = ftk_strtol_internal(str, &p, 10);

	if(p != NULL && *p == '.')
	{
		f = ftk_strtol_internal(p+1, NULL, 10);
	}

	result = f;
	while(result >= 1)
	{
		result = result / 10;
	}

	result = n + result;

	return neg ? -result : result;
}

static const char* ftk_itoa_simple(char* str, int len, int n, const char** end)
{
	int i = 0;
	int value = n;
	int need_len = 0;

	return_val_if_fail(str != NULL && len > 2, NULL);

	if(n == 0)
	{
		str[0] = '0';
		str[1] = '\0';
		
		if(end != NULL)
		{
			*end = str + 1;
		}

		return str;
	}
	
	if(n < 0)
	{
		n = -n;
		str[0] = '-';
		need_len++;
	}

	value = n;
	while(value > 0)
	{
		value = value / 10;
		need_len++;
	}

	need_len++; /*for null char*/
	return_val_if_fail(len > (need_len), NULL);
	
	i = need_len - 2;
	while(n > 0)
	{
		str[i--] = (n % 10) + '0';
		n = n / 10;
	}
	str[need_len - 1] = '\0';

	if(end != NULL)
	{
		*end = str + need_len - 1;
	}

	return str;
}

const char* ftk_itoa(char* str, int len, int n)
{
	return ftk_itoa_simple(str, len, n, NULL);
}

const char* ftk_ftoa(char* str, int len, double value)
{
	int i = 0;
	char str_n[32] = {0};
	char str_f[32] = {0};
	int n = (int)value;
	int f = (int)((value - n) * 1000000000);

	ftk_itoa(str_n, sizeof(str_n), n);
	ftk_itoa(str_f, sizeof(str_f), f > 0 ? f : -f);

	if(f == 0)
	{
		strncpy(str, str_n, len);

		return str;
	}

	i = strlen(str_f) - 1;
	i = i > 6 ? 6 : i;
	str_f[i] = '\0';

	while(i > 0)
	{
		if(str_f[i] == '0') 
		{
			str_f[i] = '\0';
		}
		i--;
	}
	return_val_if_fail(len > (strlen(str_n) + 1 + i), NULL);
	
	return ftk_strs_cat(str, len, str_n, ".", str_f, NULL);
}

char* ftk_strcpy(char* dst, const char* src)
{
	return strcpy(dst, src);
}
