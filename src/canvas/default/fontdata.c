/*
 * File: fontdata.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   functions to operate font data. 
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
 * 2009-09-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include "fontdata.h"

#ifdef WITHOUT_FTK 
#define FTK_ALLOC malloc
#define FTK_FREE free
#define FTK_ZALLOC(s) calloc(1, s)
#define FTK_REALLOC realloc
#else
#include "ftk_log.h"
#include "ftk_allocator.h"
#endif

#define FONT_VERSION 0x00000101 /*1.01*/

typedef struct _FontDataHeader
{
	unsigned int   version;
	unsigned int   char_nr;
	unsigned char  width;
	unsigned char  height;
	unsigned short encoding;
	unsigned int   unused;
	char author[64];
	char family[32];
	char style[32];
}FontDataHeader;

typedef struct _FGlyph
{
	signed char x;
	signed char y;
	unsigned char w;
	unsigned char h;
	unsigned short code;
	unsigned short unused;
	unsigned offset;
}FGlyph;

struct _FontData
{
	FontDataHeader header;

	FGlyph* glyphs;
	unsigned data_size;
	unsigned data_buffer_size;
	unsigned char* data;

	void* org_data;
	int   new_created;

	char file_name[260];
	void* current_glyph_data;
	unsigned current_glyph_data_size;
};

static Ret font_data_read_glyph(FontData* thiz, unsigned offset, unsigned size, Glyph* glyph);

FontData* font_data_create(int char_nr, Encoding encoding)
{
	FontData* thiz = (FontData*)FTK_ZALLOC(sizeof(FontData));

	if(thiz != NULL)
	{
		thiz->header.version  = FONT_VERSION;
		thiz->header.char_nr  = char_nr;
		thiz->header.encoding = encoding;
		
		if(char_nr > 0)
		{
			thiz->new_created = 1;
			thiz->glyphs = (FGlyph*)FTK_ZALLOC(char_nr * sizeof(FGlyph));
		}
	}

	return thiz;
}

FontData* font_data_load(char* data, unsigned length)
{
	FontData* thiz = font_data_create(0, 0);

	return_val_if_fail(data != NULL && length > sizeof(FontDataHeader), NULL);

	if(thiz != NULL)
	{
		int glyph_size = 0;
		thiz->header = *(FontDataHeader*)data;
		glyph_size = thiz->header.char_nr * sizeof(FGlyph);

		thiz->glyphs = (FGlyph*)(data + sizeof(FontDataHeader));
		thiz->data = (unsigned char*)(data + sizeof(FontDataHeader) + glyph_size);
		thiz->data_size = length - ((char*)thiz->data - data);
		thiz->data_buffer_size = thiz->data_size;
		thiz->new_created = 0;
		thiz->org_data = data;
	}

	return thiz;
}

#ifdef WITHOUT_FTK
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
char* read_file(const char* file_name, int* length)
{
	struct stat st = {0};
	if(stat(file_name, &st))
	{
		return NULL;
	}
	else
	{
		char* buffer = malloc(st.st_size + 1);
		FILE* fp = fopen(file_name, "rb");
		fread(buffer, 1, st.st_size, fp);
		fclose(fp);
		buffer[st.st_size] = '\0';
		*length = st.st_size;

		return buffer;
	}
}

FontData* font_data_load_file(const char* file_name)
{
	int length = 0;
	char* buffer = read_file(file_name, &length);
	
	return font_data_load(buffer, length);
}

static Ret font_data_read_glyph(FontData* thiz, unsigned offset, unsigned size, Glyph* glyph)
{

	return RET_FAIL;
}
#else
#include "ftk_file_system.h"

FontData* font_data_load_file(const char* file_name)
{
	FontData* thiz = NULL;
	return_val_if_fail(file_name != NULL, NULL);

	if((thiz = font_data_create(0, 0)) != NULL)
	{
		FtkFsHandle fp = ftk_file_open(file_name, "rb");
		if(fp != NULL)
		{
			int ret = 0;
			unsigned glyphs_size = 0;
			ret = ftk_file_read(fp, &thiz->header, sizeof(thiz->header));
			assert(ret == sizeof(thiz->header));
			glyphs_size = thiz->header.char_nr * sizeof(FGlyph);
			thiz->glyphs = (FGlyph*)FTK_ZALLOC(glyphs_size);
			assert(thiz->glyphs != NULL);
			ftk_file_read(fp, thiz->glyphs, glyphs_size);
			thiz->data = NULL;
			thiz->data_size = 0;
			thiz->new_created = 0;
			thiz->org_data = NULL;
			ftk_file_close(fp);
			ftk_strncpy(thiz->file_name, file_name, sizeof(thiz->file_name)-1);
		}
		else
		{
			FTK_FREE(thiz);
		}
	}

	return thiz;
}

static Ret font_data_read_glyph(FontData* thiz, unsigned offset, unsigned size, Glyph* glyph)
{
	int ret = 0;
	FtkFsHandle fp = ftk_file_open(thiz->file_name, "rb");
	unsigned skip = sizeof(thiz->header) + thiz->header.char_nr * sizeof(FGlyph) + offset;
	return_val_if_fail(fp != NULL && glyph != NULL, RET_FAIL);

	ret = ftk_file_seek(fp, skip);
	assert(ret == 0);
	
	if(thiz->current_glyph_data_size < size)
	{
		FTK_FREE(thiz->current_glyph_data);
		thiz->current_glyph_data = FTK_ALLOC(size * 2);
		thiz->current_glyph_data_size = size * 2;
	}

	ret = ftk_file_read(fp, thiz->current_glyph_data, size);
	ftk_logd("%s: offset=%d size=%d ret=%d\n", __func__, offset, size, ret);
	ftk_file_close(fp);
	assert(ret == size);
	glyph->data = thiz->current_glyph_data;

	return RET_OK;
}
#endif

Ret font_data_add_glyph(FontData* thiz, Glyph* glyph)
{
	unsigned i = 0;
	return_val_if_fail(thiz != NULL && glyph != NULL && thiz->new_created, RET_FAIL);
	
	for(i = 0; i < thiz->header.char_nr; i++)
	{
		if(thiz->glyphs[i].code == glyph->code)
		{
			return RET_FOUND;
		}

		if(thiz->glyphs[i].code > glyph->code || thiz->glyphs[i].code == 0)
		{
			unsigned size = glyph->w * glyph->h;
			if(thiz->glyphs[i].code > glyph->code)
			{
				unsigned k = 0;
				for(k = thiz->header.char_nr - 1; k > i; k--)
				{
					thiz->glyphs[k] = thiz->glyphs[k-1];
				}
			}

			if((thiz->data_size + size) >= thiz->data_buffer_size)
			{
				unsigned data_buffer_size = thiz->data_buffer_size + (thiz->data_buffer_size >> 1) + (size << 4);
				unsigned char* data = (unsigned char*)FTK_REALLOC(thiz->data, data_buffer_size);
				if(data != NULL)
				{
					thiz->data = data;
					thiz->data_buffer_size = data_buffer_size;
				}
			}

			if((thiz->data_size + size) < thiz->data_buffer_size)
			{
				memcpy(thiz->data + thiz->data_size, glyph->data, size);
				thiz->glyphs[i].offset = thiz->data_size;
                thiz->glyphs[i].x = glyph->x;
                thiz->glyphs[i].y = glyph->y;
                thiz->glyphs[i].w = glyph->w;
                thiz->glyphs[i].h = glyph->h;
                thiz->glyphs[i].code = glyph->code;
                thiz->glyphs[i].unused = glyph->unused;
				thiz->data_size += size;
			}
            else
                return RET_OUT_OF_SPACE;
			return RET_OK;
		}
	}

	return RET_FAIL;
}

Ret font_data_get_glyph(FontData* thiz, unsigned short code, Glyph* glyph)
{
    int low    = 0;
    int mid    = 0;
    int high   = 0;
    int result = 0;

    return_val_if_fail(thiz != NULL && glyph != NULL, RET_FAIL);

    high = thiz->header.char_nr;
    while(low <= high)
    {
        mid  = low + ((high - low) >> 1);
        result = thiz->glyphs[mid].code - code;

        if(result == 0)
        {
            glyph->x = thiz->glyphs[mid].x;
            glyph->y = thiz->glyphs[mid].y;
            glyph->w = thiz->glyphs[mid].w;
            glyph->h = thiz->glyphs[mid].h;
            glyph->code = thiz->glyphs[mid].code;
            glyph->unused = thiz->glyphs[mid].unused;
			if(thiz->data != NULL)
			{
				glyph->data = (unsigned char*)(thiz->data + (int)(thiz->glyphs[mid].offset));
			}
			else
			{
				font_data_read_glyph(thiz, thiz->glyphs[mid].offset, glyph->w * glyph->h, glyph);
			}
            return RET_OK;
        }
        else if(result < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return RET_FAIL;
}

int  font_data_get_version(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->header.version;
}

int  font_data_get_width(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->header.width;
}

int  font_data_get_height(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, 0);

	return thiz->header.height;
}

const char* font_data_get_author(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->header.author;
}

const char* font_data_get_family(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->header.family;
}

const char* font_data_get_style(FontData* thiz)
{
	return_val_if_fail(thiz != NULL, NULL);

	return thiz->header.style;
}

void font_data_set_size(FontData* thiz, int width, int height)
{
	return_if_fail(thiz != NULL);

	thiz->header.width  = width;
	thiz->header.height = height;
	
	return;
}

void font_data_set_author(FontData* thiz, const char* author)
{
	return_if_fail(thiz != NULL);
	
	ftk_strncpy(thiz->header.author, author, sizeof(thiz->header.author));

	return;
}

void font_data_set_family(FontData* thiz, const char* family)
{
	return_if_fail(thiz != NULL);
	
	ftk_strncpy(thiz->header.family, family, sizeof(thiz->header.family));

	return;
}

void font_data_set_style(FontData* thiz, const char* style)
{
	return_if_fail(thiz != NULL);
	
	ftk_strncpy(thiz->header.style, style, sizeof(thiz->header.style));

	return;
}

void font_data_destroy(FontData* thiz)
{
	return_if_fail(thiz != NULL);

	if(thiz != NULL)
	{
		if(thiz->new_created)
		{
			FTK_FREE(thiz->glyphs);
			FTK_FREE(thiz->data);
			FTK_FREE(thiz->current_glyph_data);
		}
		else
		{
			if((char*)thiz->glyphs != (char*)thiz->data)
			{
				FTK_FREE(thiz->glyphs);
			}
		}

		if(thiz->current_glyph_data != NULL)
		{
			FTK_FREE(thiz->current_glyph_data);
		}

		FTK_FREE(thiz);
	}

	return;
}

#ifdef HAS_FONT_DATA_SAVE
#include <stdio.h>
Ret font_data_save(FontData* thiz, const char* file_name)
{
	FILE* fp = fopen(file_name, "wb+");
	{
		fwrite(&thiz->header, 1, sizeof(FontDataHeader), fp);
		fwrite(thiz->glyphs, 1, sizeof(FGlyph) * thiz->header.char_nr, fp);
		fwrite(thiz->data, 1, thiz->data_size, fp);
		fclose(fp);
	}

	return RET_OK;
}
#endif

#ifdef FONT_DATA_TEST
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int ch = 'a';
	int length = 0;
	char* buffer = NULL;
	Glyph ret_glyph = {0};
	FontData* data = font_data_create(36, 0);
	Glyph* glyph = (Glyph*)FTK_ZALLOC(sizeof(Glyph));

	glyph->w = 7;
	glyph->h = 9;
	glyph->x = 5;
	glyph->y = 5;
	glyph->data = (unsigned char*) FTK_ZALLOC(glyph->w * glyph->h);

	for(; ch <= 'z'; ch++)
	{
		glyph->code = ch;
		assert(font_data_add_glyph(data, glyph) == RET_OK);
	}
	
	for(ch='0'; ch <= '9'; ch++)
	{
		glyph->code = ch;
		assert(font_data_add_glyph(data, glyph) == RET_OK);
	}

	for(ch='a'; ch <= 'z'; ch++)
	{
		glyph->code = ch;
		assert(font_data_get_glyph(data, ch, &ret_glyph) == RET_OK);
		assert(memcmp(glyph, &ret_glyph, sizeof(ret_glyph)-sizeof(void*)) == 0);
		assert(memcmp(glyph->data, ret_glyph.data, ret_glyph.w * ret_glyph.h) == 0);
	}
	
	for(ch='0'; ch <= '9'; ch++)
	{
		glyph->code = ch;
		assert(font_data_get_glyph(data, ch, &ret_glyph) == RET_OK);
		assert(memcmp(glyph, &ret_glyph, sizeof(ret_glyph)-sizeof(void*)) == 0);
		assert(memcmp(glyph->data, ret_glyph.data, ret_glyph.w * ret_glyph.h) == 0);
	}

	font_data_set_size(data, 16, 16);
	assert(font_data_get_width(data) == 16);
	assert(font_data_get_height(data) == 16);
	font_data_set_author(data, "Li XianJing <xianjimli@hotmail.com>");
	font_data_save(data, "test.fnt");
	font_data_destroy(data);

	buffer = read_file("test.fnt", &length);
	data = font_data_load(buffer, length);
	assert(data != NULL);
	for(ch='a'; ch <= 'z'; ch++)
	{
		glyph->code = ch;
		assert(font_data_get_glyph(data, ch, &ret_glyph) == RET_OK);
		assert(memcmp(glyph, &ret_glyph, sizeof(ret_glyph)-sizeof(void*)) == 0);
		assert(memcmp(glyph->data, ret_glyph.data, ret_glyph.w * ret_glyph.h) == 0);
	}
	assert(font_data_get_version(data) == FONT_VERSION);
	assert(strcmp(font_data_get_author(data), "Li XianJing <xianjimli@hotmail.com>") == 0);
	font_data_destroy(data);
	FTK_FREE(glyph->data);
	FTK_FREE(glyph);

	return 0;
}
#endif/*FONT_DATA_TEST*/
