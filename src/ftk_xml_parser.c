/*
 * File:    ftk_xml_parser.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   xml parser
 *
 * Copyright (c) Li XianJing
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
 * 2009-05-30 Li XianJing <xianjimli@hotmail.com> created.
 *
 */

#include "ftk_allocator.h"
#include "ftk_xml_parser.h"

struct _FtkXmlParser
{
	const char* read_ptr;

	int   attrs_nr;
	char* attrs[2*MAX_ATTR_NR+1];

	char* buffer;
	int buffer_used;
	int buffer_total;

	FtkXmlBuilder* builder;
};

static const char* strtrim(char* str);
static void ftk_xml_parser_parse_entity(FtkXmlParser* thiz);
static void ftk_xml_parser_parse_start_tag(FtkXmlParser* thiz);
static void ftk_xml_parser_parse_end_tag(FtkXmlParser* thiz);
static void ftk_xml_parser_parse_comment(FtkXmlParser* thiz);
static void ftk_xml_parser_parse_pi(FtkXmlParser* thiz);
static void ftk_xml_parser_parse_text(FtkXmlParser* thiz);
static void ftk_xml_parser_reset_buffer(FtkXmlParser* thiz);

FtkXmlParser* ftk_xml_parser_create(void)
{
	return (FtkXmlParser*)FTK_ZALLOC(sizeof(FtkXmlParser));
}

void ftk_xml_parser_set_builder(FtkXmlParser* thiz, FtkXmlBuilder* builder)
{
	thiz->builder = builder;

	return;
}

void ftk_xml_parser_parse(FtkXmlParser* thiz, const char* xml, int length)
{
	int i = 0;
	enum _State
	{
		STAT_NONE,
		STAT_AFTER_LT,
		STAT_START_TAG,
		STAT_END_TAG,
		STAT_TEXT,
		STAT_PRE_COMMENT1,
		STAT_PRE_COMMENT2,
		STAT_COMMENT,
		STAT_PROCESS_INSTRUCTION,
	}state = STAT_NONE;

	thiz->read_ptr = xml;

	for(; *thiz->read_ptr != '\0' && (thiz->read_ptr - xml) < length; thiz->read_ptr++, i++)
	{
		char c = thiz->read_ptr[0];

		switch(state)
		{
			case STAT_NONE:
			{

				if(c == '<')
				{
					ftk_xml_parser_reset_buffer(thiz);
					state = STAT_AFTER_LT;
				}
				else if(!isspace(c))
				{
					state = STAT_TEXT;
				}
				break;
			}
			case STAT_AFTER_LT:
			{
				if(c == '?')
				{
					state = STAT_PROCESS_INSTRUCTION;
				}
				else if(c == '/')
				{
					state = STAT_END_TAG;
				}
				else if(c == '!')
				{
					state = STAT_PRE_COMMENT1;
				}
				else if(isalpha(c) || c == '_')
				{
					state = STAT_START_TAG;
				}
				else
				{
					ftk_xml_builder_on_error(thiz->builder, 0, 0, "unexpected char");
				}
				break;
			}
			case STAT_START_TAG:
			{
				ftk_xml_parser_parse_start_tag(thiz);
				state = STAT_NONE;
				break;
			}
			case STAT_END_TAG:
			{
				ftk_xml_parser_parse_end_tag(thiz);
				state = STAT_NONE;
				break;
			}
			case STAT_PROCESS_INSTRUCTION:
			{
				ftk_xml_parser_parse_pi(thiz);
				state = STAT_NONE;
				break;
			}
			case STAT_TEXT:
			{
				ftk_xml_parser_parse_text(thiz);
				state = STAT_NONE;
				break;
			}
			case STAT_PRE_COMMENT1:
			{
				if(c == '-')
				{
					state = STAT_PRE_COMMENT2;
				}
				else
				{
					ftk_xml_builder_on_error(thiz->builder, 0, 0, "expected \'-\'");
				}
				break;
			}
			case STAT_PRE_COMMENT2:
			{
				if(c == '-')
				{
					state = STAT_COMMENT;
				}
				else
				{
					ftk_xml_builder_on_error(thiz->builder, 0, 0, "expected \'-\'");
				}
			}
			case STAT_COMMENT:
			{
				ftk_xml_parser_parse_comment(thiz);	
				state = STAT_NONE;
				break;
			}
			default:break;
		}

		if(*thiz->read_ptr == '\0')
		{
			break;
		}
	}

	return;
}

static void ftk_xml_parser_reset_buffer(FtkXmlParser* thiz)
{
	thiz->buffer_used = 0;
	thiz->attrs_nr = 0;
	thiz->attrs[0] = NULL;

	return;
}

static int ftk_xml_parser_strdup(FtkXmlParser* thiz, const char* start, int length)
{
	int offset = -1;

	if((thiz->buffer_used + length) >= thiz->buffer_total)
	{
		int length = thiz->buffer_total+(thiz->buffer_total>>1) + 128;
		char* buffer = (char*)FTK_REALLOC(thiz->buffer, length);
		if(buffer != NULL)
		{
			thiz->buffer = buffer;
			thiz->buffer_total = length;
		}
	}

	if((thiz->buffer_used + length) >= thiz->buffer_total)
	{
		return offset;
	}

	offset = thiz->buffer_used;
	ftk_strncpy(thiz->buffer + offset, start, length);
	thiz->buffer[offset + length] = '\0';
	strtrim(thiz->buffer+offset);
	thiz->buffer_used += length + 1;

	return offset;
}

static void ftk_xml_parser_parse_attrs(FtkXmlParser* thiz, char end_char)
{
	int i = 0;
	enum _State
	{
		STAT_PRE_KEY,
		STAT_KEY,
		STAT_PRE_VALUE,
		STAT_VALUE,
		STAT_END,
	}state = STAT_PRE_KEY;

	char value_end = '\"';
	const char* start = thiz->read_ptr;

	thiz->attrs_nr = 0;
	for(; *thiz->read_ptr != '\0' && thiz->attrs_nr < MAX_ATTR_NR; thiz->read_ptr++)
	{
		char c = *thiz->read_ptr;
	
		switch(state)
		{
			case STAT_PRE_KEY:
			{
				if(c == end_char || c == '>')
				{
					state = STAT_END;
				}
				else if(!isspace(c))
				{
					state = STAT_KEY;
					start = thiz->read_ptr;
				}
			}
			case STAT_KEY:
			{
				if(c == '=')
				{
					thiz->attrs[thiz->attrs_nr++] = (char*)ftk_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
					state = STAT_PRE_VALUE;
				}

				break;
			}
			case STAT_PRE_VALUE:
			{
				if(c == '\"' || c == '\'')
				{
					state = STAT_VALUE;
					value_end = c;
					start = thiz->read_ptr + 1;
				}
				break;
			}
			case STAT_VALUE:
			{
				if(c == value_end)
				{
					thiz->attrs[thiz->attrs_nr++] = (char*)ftk_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
					state = STAT_PRE_KEY;
				}
			}
			default:break;
		}

		if(state == STAT_END)
		{
			break;
		}
	}
	
	for(i = 0; i < thiz->attrs_nr; i++)
	{
		thiz->attrs[i] = thiz->buffer + (int)(thiz->attrs[i]);
	}
	thiz->attrs[thiz->attrs_nr] = NULL;

	return;
}

static void ftk_xml_parser_parse_start_tag(FtkXmlParser* thiz)
{
	enum _State
	{
		STAT_NAME,
		STAT_ATTR,
		STAT_END,
	}state = STAT_NAME;

	char* tag_name = NULL;
	const char* start = thiz->read_ptr - 1;

	for(; *thiz->read_ptr != '\0'; thiz->read_ptr++)
	{
		char c = *thiz->read_ptr;
	
		switch(state)
		{
			case STAT_NAME:
			{
				if(isspace(c) || c == '>' || c == '/')
				{
					tag_name = (char*)ftk_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
					state = (c != '>' && c != '/') ? STAT_ATTR : STAT_END;
				}
				break;
			}
			case STAT_ATTR:
			{
				ftk_xml_parser_parse_attrs(thiz, '/');
				state = STAT_END;

				break;
			}
			default:break;
		}

		if(state == STAT_END)
		{
			break;
		}
	}
	
	tag_name = thiz->buffer + (int)tag_name;
	ftk_xml_builder_on_start_element(thiz->builder, tag_name, (const char**)thiz->attrs);
	
	if(thiz->read_ptr[0] == '/')
	{
		ftk_xml_builder_on_end_element(thiz->builder, tag_name);
	}

	for(; *thiz->read_ptr != '>' && *thiz->read_ptr != '\0'; thiz->read_ptr++);

	return;
}

static void ftk_xml_parser_parse_end_tag(FtkXmlParser* thiz)
{
	char* tag_name = NULL;
	const char* start = thiz->read_ptr;
	for(; *thiz->read_ptr != '\0'; thiz->read_ptr++)
	{
		if(*thiz->read_ptr == '>')
		{
			tag_name = thiz->buffer + ftk_xml_parser_strdup(thiz, start, thiz->read_ptr-start);
			ftk_xml_builder_on_end_element(thiz->builder, tag_name);

			break;
		}
	}
	
	return;
}

static void ftk_xml_parser_parse_comment(FtkXmlParser* thiz)
{
	enum _State
	{
		STAT_COMMENT,
		STAT_MINUS1,
		STAT_MINUS2,
	}state = STAT_COMMENT;

	const char* start = ++thiz->read_ptr;
	for(; *thiz->read_ptr != '\0'; thiz->read_ptr++)
	{
		char c = *thiz->read_ptr;

		switch(state)
		{
			case STAT_COMMENT:
			{
				if(c == '-')
				{
					state = STAT_MINUS1;
				}
				break;
			}
			case STAT_MINUS1:
			{
				if(c == '-')
				{
					state = STAT_MINUS2;
				}
				else
				{
					state = STAT_COMMENT;
				}
				break;
			}
			case STAT_MINUS2:
			{
				if(c == '>')
				{
					ftk_xml_builder_on_comment(thiz->builder, start, thiz->read_ptr-start-2);
					return;
				}
			}
			default:break;
		}
	}

	return;
}

static void ftk_xml_parser_parse_pi(FtkXmlParser* thiz)
{
	enum _State
	{
		STAT_NAME,
		STAT_ATTR,
		STAT_END
	}state = STAT_NAME;

	char* tag_name = NULL;
	const char* start = thiz->read_ptr;

	for(; *thiz->read_ptr != '\0'; thiz->read_ptr++)
	{
		char c = *thiz->read_ptr;
	
		switch(state)
		{
			case STAT_NAME:
			{
				if(isspace(c) || c == '>')
				{
					tag_name = (char*)ftk_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
					state = c != '>' ? STAT_ATTR : STAT_END;
				}

				break;
			}
			case STAT_ATTR:
			{
				ftk_xml_parser_parse_attrs(thiz, '?');
				state = STAT_END;
				break;
			}
			default:break;
		}

		if(state == STAT_END)
		{
			break;
		}
	}
	
	tag_name = thiz->buffer + (int)tag_name;
	ftk_xml_builder_on_pi_element(thiz->builder, tag_name, (const char**)thiz->attrs);	

	for(; *thiz->read_ptr != '>' && *thiz->read_ptr != '\0'; thiz->read_ptr++);

	return;
}

static void ftk_xml_parser_parse_text(FtkXmlParser* thiz)
{
	const char* start = thiz->read_ptr - 1;
	for(; *thiz->read_ptr != '\0'; thiz->read_ptr++)
	{
		char c = *thiz->read_ptr;

		if(c == '<')
		{
			if(thiz->read_ptr > start)
			{
				ftk_xml_builder_on_text(thiz->builder, start, thiz->read_ptr-start);
			}
			thiz->read_ptr--;
			return;
		}
		else if(c == '&')
		{
			ftk_xml_parser_parse_entity(thiz);
		}
	}

	return;
}

static void ftk_xml_parser_parse_entity(FtkXmlParser* thiz)
{
	/*TODO*/

	return;
}

void ftk_xml_parser_destroy(FtkXmlParser* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz->buffer);
		FTK_FREE(thiz);
	}

	return;
}

static const char* strtrim(char* str)
{
	char* p = NULL;

	p = str + strlen(str) - 1;

	while(p != str && isspace(*p)) 
	{
		*p = '\0';
		p--;
	}

	p = str;
	while(*p != '\0' && isspace(*p)) p++;

	if(p != str)
	{
		char* s = p;
		char* d = str;
		while(*s != '\0')
		{
			*d = *s;
			d++;
			s++;
		}
		*d = '\0';
	}

	return str;
}
