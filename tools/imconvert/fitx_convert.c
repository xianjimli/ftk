/*
 * File: fitx_convert.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a tool converting fitx data to ftk lookup table data.
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
 * 2010-02-12 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_LINES 400 * 1024
typedef struct _TableHeader
{
	int nr;
	unsigned int offset[1];
}TableHeader;

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
		size_t fread_ret = 0;
		fread_ret = fread(buffer, 1, st.st_size, fp);
		fclose(fp);
		buffer[st.st_size] = '\0';
		*length = st.st_size;

		return buffer;
	}
}

int main(int argc, char* argv[])
{
	FILE* out = NULL;
	int length = 0;
	char* data = NULL;
	char* line = NULL;
	char* end  = NULL;
	TableHeader* h = NULL;

	if(argc != 3)
	{
		printf("#####################################################\n");
		printf("usage: %s in out\n", argv[0]);
		printf("       Converting fitx data to ftk lookup table data.\n");
		printf("#####################################################\n");

		return 0;
	}

	h = calloc(1, sizeof(TableHeader) + MAX_LINES * sizeof(int));
	data = read_file(argv[1], &length);

	line = data;
	end = data + length;

	while(line < end)
	{
		h->offset[h->nr] = line - data;
		h->nr++; 
		while(line != end && *line != '\n') line++;
		
		if(*line == '\n')
		{
			line++;
		}

		if(line >= end)
		{
			break;
		}
	}

	if((out = fopen(argv[2], "wb+")) != NULL)
	{
		fwrite(h, 1, sizeof(h->nr) + h->nr * sizeof(int), out);
		fwrite(data, 1, length, out);
		fflush(out);
		fclose(out);
	}

	free(h);

	return 0;
}

