/*
 * File: fitx_convert.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a tool to shrink less use freq words from lookup table.
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
 * 2010-04-09 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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
	int freq = 0;
	FILE* out = NULL;
	int length = 0;
	char* data = NULL;
	char* line = NULL;
	char* real_data = NULL;
	TableHeader* h = NULL;

	if(argc != 4)
	{
		printf("#####################################################\n");
		printf("usage: %s in out freq\n", argv[0]);
		printf("       Remove words whose freq are less than specified freq in lookup table data.\n");
		printf("#####################################################\n");

		return 0;
	}

	freq = atoi(argv[3]);
	data = read_file(argv[1], &length);
	h = (TableHeader*)data;
	real_data = data + h->nr * sizeof(int) + sizeof(h->nr);

	if((out = fopen(argv[2], "wb+")) != NULL)
	{
		int i = 0;
		int f = 0;
		char* str = NULL;
		char* start = real_data;
		char* end = real_data;
		for(i = 0; i < h->nr; i++)
		{
			end = strchr(start, '\n');
			*end = '\0';
			str = strrchr(start, ' ') + 1;
			f = atoi(str);
			*end='\n';
			if(f > freq)
			{
				fwrite(start, 1, end-start+1, out);
			}
			start = end + 1;
//			printf("i=%d start=%p end=%p freq=%d\n", i, start, end, f);
		}
		fflush(out);
		fclose(out);
	}

	free(data);

	return 0;
}

