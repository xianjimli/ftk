/*
 * File: ftk_input_pattern.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   input pattern to format some special input, such as date, 
 *  time and ip address.
 *
 * Copyright (c) 2009 - 2011  Li XianJing <xianjimli@hotmail.com>
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
 * 2011-07-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_allocator.h"
#include "ftk_input_pattern.h"

typedef enum _ValidType
{
	VT_DIGIT  = 'D',
	VT_XDIGIT = 'X',
	VT_ALPHA  = 'A',
	VT_ID     = 'I'
}ValidType;

typedef int (*IsValidChar)(unsigned char c);

typedef struct _InputPattern
{
	size_t offset;
	size_t size;
	size_t min_size;
	size_t max_size;
	unsigned char default_char;
	unsigned char delim_char;
	struct _InputPattern* next;
	IsValidChar is_valid_char;
}InputPattern;

struct _FtkInputPattern
{
	char* text;
	int caret;
	size_t max_length;

	InputPattern* pattern;
};

static void input_pattern_clear(InputPattern* p)
{
	p->offset = p->size = p->min_size = p->max_size = 0;
	p->default_char = p->delim_char = '\0';
	p->next = NULL;
	p->is_valid_char = NULL;

	return;
}

static int is_digit(unsigned char c)
{
	return c >= '0' && c <= '9';
}

static int is_xdigit(unsigned char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static int is_alpha(unsigned char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_id(unsigned char c)
{
	return is_alpha(c) || is_digit(c) || c == '_';
}

static int is_any(unsigned char c)
{
	return 1;
}

static IsValidChar find_valid_function(unsigned char d)
{
	switch(d)
	{
		case 'D': return is_digit;
		case 'X': return is_xdigit;
		case 'A': return is_alpha;
		case 'I': return is_id;
		default: break;
	}

	return is_any;
}

static Ret ftk_input_pattern_append(FtkInputPattern* thiz, InputPattern* p)
{
	InputPattern* iter = NULL;
	InputPattern* pattern = NULL;
	return_val_if_fail(p->is_valid_char != NULL, RET_FAIL);

	if(p->max_size == 0)
	{
		p->max_size = p->min_size;
	}

	pattern = (InputPattern*)FTK_ZALLOC(sizeof(InputPattern));
	memcpy(pattern, p, sizeof(InputPattern));

	if(thiz->pattern == NULL)
	{
		thiz->pattern = pattern;
	}
	else
	{
		for(iter = thiz->pattern; iter->next != NULL; iter = iter->next);
		iter->next = pattern;
	}

	thiz->max_length += p->max_size + 1;

	return RET_OK;
}

/*D[2]0:D[2]0:D[2]0*/
/*type[min-max]default_char delim_char*/
static Ret ftk_input_pattern_parse(FtkInputPattern* thiz, const char* pattern)
{
	int state = 0;
	size_t i = 0;
	unsigned char c = 0;
	InputPattern p = {0};
	enum 
	{
		ST_TYPE,
		ST_MIN,
		ST_MAX,
		ST_DEFAULT,
		ST_DELIM
	};

	state = ST_TYPE;
	input_pattern_clear(&p);
	for(i = 0; pattern[i]; i++)
	{
		c = pattern[i];

		switch(state)
		{
			case ST_TYPE:
			{
				if(c == '[')
				{
					state = ST_MIN;
				}
				else
				{
					p.is_valid_char = find_valid_function(c);
				}
				break;
			}
			case ST_MIN:
			{
				if(c == ']')
				{
					state = ST_DEFAULT;
				}
				else if(c == '-')
				{
					state = ST_MAX;
				}
				else
				{
					p.min_size = p.min_size * 10 + c - '0';
				}
				break;
			}
			case ST_MAX:
			{
				if(c == ']')
				{
					state = ST_DEFAULT;
				}
				else
				{
					p.max_size = p.max_size * 10 + c - '0';
				}
				
				break;
			}
			case ST_DEFAULT:
			{
				if(p.is_valid_char(c))
				{
					p.default_char = c;
					state = ST_DELIM;
					break;
				}
				else
				{
					/*fall down*/
				}
			}
			case ST_DELIM:
			{
				p.delim_char = c;
				state = ST_TYPE;
				ftk_input_pattern_append(thiz, &p);
				input_pattern_clear(&p);
				break;
			}
			default:break;
		}
	}
	
	ftk_input_pattern_append(thiz, &p);

	return RET_OK;
}

FtkInputPattern* ftk_input_pattern_create(const char* pattern, const char* init)
{
	FtkInputPattern* thiz = NULL;

	return_val_if_fail(pattern != NULL && init != NULL, NULL);
	
	thiz = (FtkInputPattern*)FTK_ZALLOC(sizeof(FtkInputPattern));

	if(thiz != NULL)
	{
		ftk_input_pattern_parse(thiz, pattern);
		thiz->max_length += 1;
		ftk_input_pattern_set_text(thiz, init);
	}

	return thiz;
}

static InputPattern* ftk_input_pattern_get_pattern_of_caret(FtkInputPattern* thiz)
{
	size_t i = 0;
	size_t start = 0;
	size_t end = 0;
	unsigned char c = 0;
	InputPattern* iter = thiz->pattern;

	for(i = 0; thiz->text[i] && i < thiz->caret && iter != NULL; i++)
	{
		c = thiz->text[i];

		if(c == iter->delim_char)
		{
			iter = iter->next;
			start = i + 1;
		}
	}

	if(iter != NULL)
	{
		for(; thiz->text[i]; i++)
		{
			c = thiz->text[i];

			if(c == iter->delim_char)
			{
				break;
			}	
		}
		end = i;
		iter->offset = start;
		iter->size = end - start;
	}

	return iter;
}

Ret    ftk_input_pattern_input(FtkInputPattern* thiz, FtkKey key)
{
	size_t i = 0;
	Ret ret = RET_OK;
	InputPattern* pattern = NULL;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	pattern = ftk_input_pattern_get_pattern_of_caret(thiz);
	return_val_if_fail(pattern != NULL, RET_FAIL);

	switch(key)
	{
		case FTK_KEY_DELETE:
		{
			if(pattern->size <= pattern->min_size)
			{
				if(thiz->text[thiz->caret] != pattern->delim_char)
				{
					thiz->text[thiz->caret] = pattern->default_char;
				}
				break;
			}

			if(thiz->text[thiz->caret] != pattern->delim_char)
			{
				if(pattern->size > 1)
				{
					for(i = thiz->caret + 1; thiz->text[i]; i++)
					{
						thiz->text[i] = thiz->text[i+1];
					}
				}
				else if(pattern->size == 1)
				{
					thiz->text[thiz->caret] = pattern->default_char;
				}
			}
			break;
		}
		case FTK_KEY_BACKSPACE:
		{
			thiz->caret--;
			if(thiz->caret < pattern->offset) break;

			if(pattern->size <= pattern->min_size)
			{
				if(thiz->text[thiz->caret] != pattern->delim_char)
				{
					thiz->text[thiz->caret] = pattern->default_char;
				}
			}
			else
			{
				if(pattern->size > 1)
				{
					for(i = thiz->caret; thiz->text[i]; i++)
					{
						thiz->text[i] = thiz->text[i+1];
					}
				}
				else if(pattern->size == 1)
				{
					thiz->text[thiz->caret] = pattern->default_char;
				}
			}
			break;
		}
		case FTK_KEY_LEFT:
		{
			if(thiz->caret > 0)
			{
				thiz->caret--;
			}
			break;
		}
		case FTK_KEY_RIGHT:
		{
			if(thiz->caret < strlen(thiz->text))
			{
				thiz->caret++;
			}
			break;
		}
		default:
		{
			unsigned char c = (unsigned char)key;
			if(pattern->is_valid_char(c))
			{
				if(pattern->size == pattern->max_size)
				{
					/*replace*/
					if(thiz->text[thiz->caret] != pattern->delim_char)
					{
						thiz->text[thiz->caret] = c;
					}
				}
				else
				{
					/*insert*/
					thiz->text[strlen(thiz->text) + 1] = '\0';
					for(i = strlen(thiz->text); i > thiz->caret; i--)
					{
						thiz->text[i] = thiz->text[i-1];
					}
					thiz->text[i] = c;
				}
				thiz->caret++;
			}
			else
			{
				ret = RET_CONTINUE;
			}
		}
	}

	if(thiz->caret < 0)
	{
		thiz->caret = 0;
	}

	return ret;
}

Ret    ftk_input_pattern_set_caret(FtkInputPattern* thiz, size_t caret)
{
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(caret <= strlen(thiz->text))
	{
		thiz->caret = caret;
	}

	return RET_OK;
}

Ret    ftk_input_pattern_set_text(FtkInputPattern* thiz, const char* text)
{
	return_val_if_fail(thiz != NULL && text != NULL, RET_FAIL);

	if(thiz->text == NULL)
	{
		thiz->text = (char*)FTK_ZALLOC(thiz->max_length + 1);
	}

	strncpy(thiz->text, text, thiz->max_length);

	return RET_OK;
}

size_t ftk_input_pattern_get_caret(FtkInputPattern* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->caret;
}


const char* ftk_input_pattern_get_text(FtkInputPattern* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->text;
}

void ftk_input_pattern_destroy(FtkInputPattern* thiz)
{
	InputPattern* save = NULL;
	InputPattern* iter = NULL;

	if(thiz != NULL)
	{
		iter = thiz->pattern;

		while(iter != NULL)
		{
			save = iter->next;
			FTK_FREE(iter);
			iter = save;
		}

		FTK_FREE(thiz);
	}

	return;
}

#ifdef _TEST
int main(int argc, char* argv[])
{
	InputPattern* p = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	FtkInputPattern* thiz = ftk_input_pattern_create("D[2]0:A[2-4]a:X[0-4]b", "12:abc:4");
	assert(thiz->pattern->is_valid_char == is_digit);
	assert(thiz->pattern->min_size == 2);
	assert(thiz->pattern->max_size == 2);
	assert(thiz->pattern->default_char == '0');
	
	assert(thiz->pattern->next->is_valid_char == is_alpha);
	assert(thiz->pattern->next->min_size == 2);
	assert(thiz->pattern->next->max_size == 4);
	assert(thiz->pattern->next->default_char == 'a');
	
	assert(thiz->pattern->next->next->is_valid_char == is_xdigit);
	assert(thiz->pattern->next->next->min_size == 0);
	assert(thiz->pattern->next->next->max_size == 4);
	assert(thiz->pattern->next->next->default_char == 'b');

	ftk_input_pattern_destroy(thiz);
	
	thiz = ftk_input_pattern_create("D[2-2]0:A[2-4]a:X[0-4]b", "12:abc:4");
	assert(thiz->pattern->is_valid_char == is_digit);
	assert(thiz->pattern->min_size == 2);
	assert(thiz->pattern->max_size == 2);
	assert(thiz->pattern->default_char == '0');
	
	assert(thiz->pattern->next->is_valid_char == is_alpha);
	assert(thiz->pattern->next->min_size == 2);
	assert(thiz->pattern->next->max_size == 4);
	assert(thiz->pattern->next->default_char == 'a');
	
	assert(thiz->pattern->next->next->is_valid_char == is_xdigit);
	assert(thiz->pattern->next->next->min_size == 0);
	assert(thiz->pattern->next->next->max_size == 4);
	assert(thiz->pattern->next->next->default_char == 'b');

	ftk_input_pattern_set_caret(thiz, 0);
	p = ftk_input_pattern_get_pattern_of_caret(thiz);
	assert(p->offset == 0);
	assert(p->size == 2);
	
	ftk_input_pattern_set_caret(thiz, 1);
	p = ftk_input_pattern_get_pattern_of_caret(thiz);
	assert(p->is_valid_char == is_digit);
	assert(p->offset == 0);
	assert(p->size == 2);
	
	ftk_input_pattern_set_caret(thiz, 3);
	p = ftk_input_pattern_get_pattern_of_caret(thiz);
	assert(p->is_valid_char == is_alpha);
	assert(p->offset == 3);
	assert(p->size == 3);
	
	ftk_input_pattern_set_caret(thiz, 7);
	p = ftk_input_pattern_get_pattern_of_caret(thiz);
	assert(p->is_valid_char == is_xdigit);
	assert(p->offset == 7);
	assert(p->size == 1);

	assert(strcmp(thiz->text, "12:abc:4") == 0);
	ftk_input_pattern_set_caret(thiz, 0);
	ftk_input_pattern_input(thiz, FTK_KEY_DELETE);
	assert(strcmp(thiz->text, "1:abc:4") == 0);
	ftk_input_pattern_input(thiz, FTK_KEY_DELETE);
	assert(strcmp(thiz->text, "0:abc:4") == 0);

	ftk_input_pattern_destroy(thiz);


	return 0;
}
#endif
