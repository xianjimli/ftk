/*
 * File:    coder_share.h
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   generate code shared by service and client.
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

#include "coder_share.h"

#define STR_LENGTH 127

typedef struct _PrivInfo
{
	GString*  req_decls;
	gboolean  has_interface;
    char interface[STR_LENGTH+1];
    char interface_lower[STR_LENGTH+1];
    char interface_upper[STR_LENGTH+1];
}PrivInfo;

static gboolean coder_share_end_interface(Coder* thiz)
{
	FILE* fp = NULL;
	char filename[260] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	
	if(!priv->has_interface)
	{
		return TRUE;
	}

	mkdir(priv->interface, 0777);
	g_string_append_printf(priv->req_decls, "	%s_REQ_NR\n", priv->interface_upper);
	g_string_append_printf(priv->req_decls, "}%s_REQ;\n", priv->interface_upper);

	snprintf(filename, sizeof(filename) - 1, "%s/%s_share.h", priv->interface, priv->interface_lower);
	fp = fopen(filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#ifndef %s_SHARE_H\n", priv->interface_upper);
		fprintf(fp, "#define %s_SHARE_H\n\n", priv->interface_upper);
		fprintf(fp, "#include \"fbus_typedef.h\"\n\n");
		fprintf(fp, "FTK_BEGIN_DECLS\n\n");
		fprintf(fp, "%s\n", priv->req_decls->str);
		fprintf(fp, "#define FBUS_SERVICE_%s \"fbus.service.%s\"\n\n", priv->interface_upper, priv->interface_lower);
		fprintf(fp, "FTK_END_DECLS\n\n");
		fprintf(fp, "#endif/*%s_SHARE_H*/\n", priv->interface_upper);
		fclose(fp);
	}
	g_string_free(priv->req_decls, 1);

	priv->has_interface = FALSE;

	return TRUE;
}

static gboolean coder_share_on_interface(Coder* thiz, const char* name, const char* parent)
{
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	coder_share_end_interface(thiz);
	
	priv->has_interface = 1;
	strncpy(priv->interface, name, STR_LENGTH);
	coder_name_to_lower(name, priv->interface_lower);
	strcpy(priv->interface_upper, priv->interface_lower);
	coder_to_upper(priv->interface_upper);

	priv->req_decls = g_string_sized_new(4096);
	g_string_append_printf(priv->req_decls, "typedef enum _%s_REQ\n{", priv->interface_upper);

	return TRUE;
}

typedef struct _TypeInfo
{
	int attr;
	gboolean is_binary;
	char name[STR_LENGTH+1];
}TypeInfo;

static gboolean coder_share_on_function(Coder* thiz, struct _IDL_OP_DCL f)
{
	char req_coder_name[STR_LENGTH+1];
	char lower_func_name[STR_LENGTH+1] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	const char* func_name = IDL_IDENT(f.ident).str;

	coder_name_to_lower(func_name, lower_func_name);
	snprintf(req_coder_name, sizeof(req_coder_name)-1, "%s_%s", priv->interface_lower, lower_func_name);
	coder_to_upper(req_coder_name);
	g_string_append_printf(priv->req_decls, "	%s,\n", req_coder_name);


	return TRUE;
}

static gboolean coder_share_on_const(Coder* thiz, struct _IDL_CONST_DCL c)
{
	return TRUE;
}

static gboolean coder_share_on_struct(Coder* thiz, struct _IDL_TYPE_STRUCT s)
{
	return TRUE;
}

static gboolean coder_share_on_enum(Coder* thiz, struct _IDL_TYPE_ENUM e)
{
	return TRUE;
}

static gboolean coder_share_on_union(Coder* thiz, struct _IDL_TYPE_UNION u)
{
	return TRUE;
}

static void coder_share_destroy(Coder* thiz)
{
	if(thiz != NULL)
	{
		coder_share_end_interface(thiz);

		g_free(thiz);
	}

	return;
}

Coder* coder_share_create(const char* name)
{
	Coder* thiz = g_malloc0(sizeof(Coder) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->on_interface = coder_share_on_interface;
		thiz->on_function  = coder_share_on_function;
		thiz->on_const     = coder_share_on_const;
		thiz->on_enum      = coder_share_on_enum;
		thiz->on_struct    = coder_share_on_struct;
		thiz->on_union     = coder_share_on_union;
		thiz->destroy      = coder_share_destroy;
	}

	return thiz;
}
