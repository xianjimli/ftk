/*
 * File:    coder_xxxx.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   coder to generate frameworks of interface implementation.
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

#include <sys/stat.h>
#include <sys/types.h>
#include "coder_intf.h"

#define STR_LENGTH 127

typedef struct _PrivInfo
{
	char*    name;
	char*    lower_name;
	GString* func_create;
	GString* func_empty;
	gboolean  has_interface;
    char interface[STR_LENGTH+1];
    char interface_lower[STR_LENGTH+1];
    char interface_upper[STR_LENGTH+1];
}PrivInfo;

static gboolean coder_xxxx_end_interface(Coder* thiz)
{
	FILE* fp = NULL;
	char h_filename[260] = {0};
	char c_filename[260] = {0};
	char create_func[STR_LENGTH+1] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	if(!priv->has_interface)
	{
		return TRUE;
	}
	
	mkdir(priv->interface, 0777);
	snprintf(create_func, STR_LENGTH, "%s_%s_create(void)", priv->interface_lower, priv->lower_name);
	
	snprintf(h_filename, sizeof(h_filename) - 1, "%s/%s_%s.h", priv->interface, priv->interface_lower, priv->lower_name);
	fp = fopen(h_filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#ifndef %s_IMPL_H\n", priv->interface_upper);
		fprintf(fp, "#define %s_IMPL_H\n\n", priv->interface_upper);
		fprintf(fp, "#include \"fbus_typedef.h\"\n\n");
		fprintf(fp, "#include \"%s.h\"\n\n", priv->interface_lower);
		fprintf(fp, "FTK_BEGIN_DECLS\n\n");
		fprintf(fp, "%s* %s;\n\n", priv->interface, create_func);
		fprintf(fp, "FTK_END_DECLS\n\n");
		fprintf(fp, "#endif/*%s_IMPL_H*/\n", priv->interface_upper);
		fclose(fp);
	}
	
	snprintf(h_filename, sizeof(h_filename) - 1, "%s_%s.h", priv->interface_lower, priv->lower_name);
	snprintf(c_filename, sizeof(c_filename) - 1, "%s/%s_%s.c", priv->interface, priv->interface_lower, priv->name);
	fp = fopen(c_filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#include \"%s\"\n\n", h_filename);
		fprintf(fp, "typedef struct _PrivInfo\n");
		fprintf(fp, "{\n");
		fprintf(fp, "\tint dummy;\n");
		fprintf(fp, "}PrivInfo;\n\n");
		fprintf(fp, "%s\n", priv->func_empty->str);
		fprintf(fp, "static void %s_%s_destroy(%s* thiz)\n", priv->interface_lower, priv->lower_name, priv->interface);
		fprintf(fp, "{\n");
		fprintf(fp, "	if(thiz != NULL)\n");
		fprintf(fp, "	{\n");
		fprintf(fp, "		FTK_FREE(thiz);\n");
		fprintf(fp, "	}\n\n");
		fprintf(fp, "	return;\n");
		fprintf(fp, "}\n\n");

		fprintf(fp, "%s* %s\n", priv->interface, create_func);
		fprintf(fp, "{\n");
		fprintf(fp, "	%s* thiz = FTK_ZALLOC(sizeof(%s) + sizeof(PrivInfo));\n\n", priv->interface, priv->interface);
		fprintf(fp, "	if(thiz != NULL)\n");
		fprintf(fp, "	{\n");
		fprintf(fp, "%s", priv->func_create->str);
		fprintf(fp, "		thiz->destroy = %s_%s_destroy;\n", priv->interface_lower, priv->lower_name);
		fprintf(fp, "	}\n\n");
		fprintf(fp, "	return thiz;\n");
		fprintf(fp, "}\n");
		fclose(fp);
	}

	g_string_free(priv->func_empty, 1);
	g_string_free(priv->func_create, 1);

	priv->has_interface = FALSE;

	return TRUE;
}

static gboolean coder_xxxx_on_interface(Coder* thiz, const char* name, const char* parent)
{
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	coder_xxxx_end_interface(thiz);
	
	priv->has_interface = TRUE;
	strncpy(priv->interface, name, STR_LENGTH);
	coder_name_to_lower(name, priv->interface_lower);
	strcpy(priv->interface_upper, priv->interface_lower);
	coder_to_upper(priv->interface_upper);

	priv->func_empty = g_string_sized_new(4096);
	priv->func_create = g_string_sized_new(4096);

	return TRUE;
}

typedef struct _TypeInfo
{
	int attr;
	gboolean is_binary;
	char name[STR_LENGTH+1];
	char org_name[STR_LENGTH+1];
}TypeInfo;

static gboolean coder_get_type_info(IDL_tree tree, int attr, TypeInfo* type)
{
	const char* name = "";
	type->attr = attr;
	if(attr == IDL_PARAM_INOUT)
	{
		assert(!"Not supported.");
	}

	memset(type, 0x00, sizeof(TypeInfo));
	if(IDL_NODE_IS_TYPE(tree))
	{
		switch(IDL_NODE_TYPE(tree))
		{
			case IDLN_TYPE_INTEGER:
			{
				name = "int";
				break;
			}
			case IDLN_TYPE_STRING:
			{
				name = "String";
				break;
			}
			default:
			{
				assert(!"Not supported");
				break;
			}

		}
	}
	else if(IDL_NODE_TYPE(tree) == IDLN_IDENT)
	{
		name = IDL_IDENT(tree).str;
	}
	
	strcat(type->org_name, name);
	if(strcmp(name, "int") == 0)
	{
		strcat(type->name, "int");
		strcat(type->name, attr == IDL_PARAM_OUT ? "*" : "");
	}
	else if(strcmp(name, "String") == 0)
	{
		strcat(type->name, attr == IDL_PARAM_OUT ? "" : "const ");
		strcat(type->name, "char*");
		strcat(type->name, attr == IDL_PARAM_OUT ? "*" : "");
	}
	else if(strcmp(name, "FBusBinary") == 0)
	{
		type->is_binary = TRUE;
	}
	else
	{
		strcat(type->name, attr == IDL_PARAM_OUT ? "" : "const ");
		strcat(type->name, name);
		strcat(type->name, "*");
	}

	return TRUE;
}

static gboolean coder_xxxx_on_function(Coder* thiz, struct _IDL_OP_DCL f)
{
	int i = 0;
	TypeInfo  type = {0};
	struct _IDL_LIST iter = {0};
	const char* param_name = NULL;
	char lower_func_name[STR_LENGTH+1] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	const char* func_name = IDL_IDENT(f.ident).str;
	GString* params_list = g_string_sized_new(1024);
	coder_name_to_lower(func_name, lower_func_name);
	coder_get_type_info(f.op_type_spec, 0, &type);

	if(f.parameter_dcls != NULL)
	{
		for (i = 0, iter = IDL_LIST(f.parameter_dcls); iter.data != NULL; iter = IDL_LIST(iter.next))
		{
			int attr = IDL_PARAM_DCL(iter.data).attr;
			param_name = IDL_IDENT(IDL_PARAM_DCL(iter.data).simple_declarator).str;
			coder_get_type_info(IDL_PARAM_DCL(iter.data).param_type_spec, attr, &type);
			g_string_append_printf(params_list, ", %s %s", type.name, param_name);

			if(iter.next == NULL)
			{
				break;
			}
		}
	}
	g_string_append_printf(priv->func_empty, "static Ret %s_%s_%s(%s* thiz%s)\n{\n", 
		priv->interface_lower, priv->lower_name, lower_func_name, priv->interface,  params_list->str);
	g_string_append_printf(priv->func_empty, "	return RET_OK;\n}\n\n");
	
	g_string_append_printf(priv->func_create, "		thiz->%s = %s_%s_%s;\n",
		lower_func_name, priv->interface_lower, priv->lower_name, lower_func_name);

	g_string_free(params_list, 1);

	return TRUE;
}

static gboolean coder_xxxx_on_const(Coder* thiz, struct _IDL_CONST_DCL c)
{
	return TRUE;
}

static gboolean coder_xxxx_on_struct(Coder* thiz, struct _IDL_TYPE_STRUCT s)
{
	return TRUE;
}

static gboolean coder_xxxx_on_enum(Coder* thiz, struct _IDL_TYPE_ENUM e)
{
	return TRUE;
}

static gboolean coder_xxxx_on_union(Coder* thiz, struct _IDL_TYPE_UNION u)
{
	return TRUE;
}

static void coder_xxxx_destroy(Coder* thiz)
{
	if(thiz != NULL)
	{
		PrivInfo* priv = (PrivInfo*)thiz->priv;
		
		coder_xxxx_end_interface(thiz);
		g_free(priv->name);
		g_free(priv->lower_name);
		g_free(thiz);
	}

	return;
}

Coder* coder_xxxx_create(const char* name)
{
	Coder* thiz = g_malloc0(sizeof(Coder) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		PrivInfo* priv = (PrivInfo*)thiz->priv;
		
		priv->name = g_strdup(name);
		priv->lower_name = g_strdup(name);
		coder_to_lower(priv->lower_name);
		thiz->on_interface = coder_xxxx_on_interface;
		thiz->on_function  = coder_xxxx_on_function;
		thiz->on_const     = coder_xxxx_on_const;
		thiz->on_enum      = coder_xxxx_on_enum;
		thiz->on_struct    = coder_xxxx_on_struct;
		thiz->on_union     = coder_xxxx_on_union;
		thiz->destroy      = coder_xxxx_destroy;
	}

	return thiz;
}
