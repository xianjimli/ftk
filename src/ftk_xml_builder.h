/*
 * File:    ftk_xml_builder.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   xml builder interface.
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

#include "ftk_log.h"

#ifndef XML_BUILDER_H
#define XML_BUILDER_H

FTK_BEGIN_DECLS

struct _FtkXmlBuilder;
typedef struct _FtkXmlBuilder FtkXmlBuilder;

typedef void (*FtkXmlBuilderOnStartElementFunc)(FtkXmlBuilder* thiz, const char* tag, const char** attrs);
typedef void (*FtkXmlBuilderOnEndElementFunc)(FtkXmlBuilder* thiz, const char* tag);
typedef void (*FtkXmlBuilderOnTextFunc)(FtkXmlBuilder* thiz, const char* text, size_t length);
typedef void (*FtkXmlBuilderOnCommentFunc)(FtkXmlBuilder* thiz, const char* text, size_t length);
typedef void (*FtkXmlBuilderOnPiElementFunc)(FtkXmlBuilder* thiz, const char* tag, const char** attrs);
typedef void (*FtkXmlBuilderOnErrorFunc)(FtkXmlBuilder* thiz, int line, int row, const char* message);
typedef void (*FtkXmlBuilderDestroyFunc)(FtkXmlBuilder* thiz);

struct _FtkXmlBuilder
{
	FtkXmlBuilderOnStartElementFunc on_start_element;
	FtkXmlBuilderOnEndElementFunc   on_end_element;
	FtkXmlBuilderOnTextFunc         on_text;
	FtkXmlBuilderOnCommentFunc      on_comment;
	FtkXmlBuilderOnPiElementFunc    on_pi_element;
	FtkXmlBuilderOnErrorFunc        on_error;
	FtkXmlBuilderDestroyFunc        destroy;

	char priv[ZERO_LEN_ARRAY];
};

static inline void ftk_xml_builder_on_start_element(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	return_if_fail(thiz != NULL && thiz->on_start_element != NULL);

	thiz->on_start_element(thiz, tag, attrs);

	return;
}

static inline void ftk_xml_builder_on_end_element(FtkXmlBuilder* thiz, const char* tag)
{
	return_if_fail(thiz != NULL);
	if(thiz->on_end_element != NULL)
	{
		thiz->on_end_element(thiz, tag);
	}

	return;
}

static inline void ftk_xml_builder_on_text(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return_if_fail(thiz != NULL);
	if(thiz->on_text != NULL)
	{
		thiz->on_text(thiz, text, length);
	}

	return;
}

static inline void ftk_xml_builder_on_comment(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return_if_fail(thiz != NULL);
	if(thiz->on_comment != NULL)
	{
		thiz->on_comment(thiz, text, length);
	}

	return;
}

static inline void ftk_xml_builder_on_pi_element(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	return_if_fail(thiz != NULL);
	if(thiz->on_pi_element != NULL)
	{
		thiz->on_pi_element(thiz, tag, attrs);
	}

	return;
}

static inline void ftk_xml_builder_on_error(FtkXmlBuilder* thiz, int line, int row, const char* message)
{
	return_if_fail(thiz != NULL);
	if(thiz->on_error != NULL)
	{
		thiz->on_error(thiz, line, row, message);
	}

	return;
}

static inline void ftk_xml_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL && thiz->destroy != NULL)
	{
		thiz->destroy(thiz);
	}

	return;
}

FTK_END_DECLS

#endif/*XML_BUILDER_H*/

