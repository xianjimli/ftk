/*
 * File:    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   
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

#include "coder_intf.h"

#define STR_LENGTH 127

typedef struct _PrivInfo
{
	GString* func_types;
	GString* func_decls;
	GString* func_inlines;
	gboolean  has_interface;
    char interface[STR_LENGTH+1];
    char interface_lower[STR_LENGTH+1];
    char interface_upper[STR_LENGTH+1];
}PrivInfo;

static gboolean coder_intf_end_interface(Coder* thiz)
{
	FILE* fp = NULL;
	char filename[260] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	if(!priv->has_interface)
	{
		return TRUE;
	}

	mkdir(priv->interface, 0777);
	g_string_append_printf(priv->func_types, "typedef void (*%sDestroy)(%s* thiz);\n", priv->interface, priv->interface);
	g_string_append_printf(priv->func_decls, "\t%sDestroy destroy;\n\n", priv->interface);
	g_string_append_printf(priv->func_decls, "\tchar priv[1];\n");
	g_string_append_printf(priv->func_decls, "};\n");

	g_string_append_printf(priv->func_inlines, "static inline void %s_destroy(%s* thiz)\n{\n",
		priv->interface_lower, priv->interface);
	g_string_append_printf(priv->func_inlines, "\tif(thiz != NULL && thiz->destroy != NULL)\n");
	g_string_append_printf(priv->func_inlines, "\t{\n");
	g_string_append_printf(priv->func_inlines, "\t\tthiz->destroy(thiz);\n");
	g_string_append_printf(priv->func_inlines, "\t}\n\n");
	g_string_append_printf(priv->func_inlines, "\treturn;\n");
	g_string_append_printf(priv->func_inlines, "}\n");
	snprintf(filename, sizeof(filename) - 1, "%s/%s.h", priv->interface, priv->interface_lower);
	fp = fopen(filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#ifndef %s_H\n", priv->interface_upper);
		fprintf(fp, "#define %s_H\n\n", priv->interface_upper);
		fprintf(fp, "#include \"fbus_typedef.h\"\n\n");
		fprintf(fp, "FTK_BEGIN_DECLS\n\n");
		fprintf(fp, "%s\n", priv->func_types->str);
		fprintf(fp, "%s\n", priv->func_decls->str);
		fprintf(fp, "%s\n", priv->func_inlines->str);
		fprintf(fp, "FTK_END_DECLS\n\n");
		fprintf(fp, "#endif/*%s_H*/\n", priv->interface_upper);
		fclose(fp);
	}
	g_string_free(priv->func_types, 1);
	g_string_free(priv->func_decls, 1);
	g_string_free(priv->func_inlines, 1);

	priv->has_interface = FALSE;

	return TRUE;
}

static gboolean coder_intf_on_interface(Coder* thiz, const char* name, const char* parent)
{
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	coder_intf_end_interface(thiz);
	
	priv->has_interface = TRUE;
	strncpy(priv->interface, name, STR_LENGTH);
	coder_name_to_lower(name, priv->interface_lower);
	strcpy(priv->interface_upper, priv->interface_lower);
	coder_to_upper(priv->interface_upper);

	priv->func_types = g_string_sized_new(4096);
	priv->func_decls = g_string_sized_new(4096);
	priv->func_inlines = g_string_sized_new(4096);

	g_string_append_printf(priv->func_types, "struct _%s;\n", name);
	g_string_append_printf(priv->func_types, "typedef struct _%s %s;\n\n", name, name);
	g_string_append_printf(priv->func_decls, "struct _%s\n{\n", name);


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

static gboolean coder_intf_on_function(Coder* thiz, struct _IDL_OP_DCL f)
{
	int i = 0;
	TypeInfo  type = {0};
	const char* param_name = NULL;
	struct _IDL_LIST iter = {0};
	char lower_func_name[STR_LENGTH+1] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	const char* func_name = IDL_IDENT(f.ident).str;
	GString* params_list = g_string_sized_new(1024);
	GString* params_call = g_string_sized_new(1023);

	coder_name_to_lower(func_name, lower_func_name);
	coder_get_type_info(f.op_type_spec, 0, &type);
	
	g_string_append_printf(priv->func_types, "typedef Ret (*%s%s)(%s* thiz", priv->interface, func_name, priv->interface);
	g_string_append_printf(priv->func_decls, "	%s%s %s;\n", priv->interface, func_name, lower_func_name);

	if(f.parameter_dcls != NULL)
	{
		for (i = 0, iter = IDL_LIST(f.parameter_dcls); iter.data != NULL; iter = IDL_LIST(iter.next))
		{
			int attr = IDL_PARAM_DCL(iter.data).attr;
			param_name = IDL_IDENT(IDL_PARAM_DCL(iter.data).simple_declarator).str;
			coder_get_type_info(IDL_PARAM_DCL(iter.data).param_type_spec, attr, &type);

			g_string_append_printf(params_call, ", %s", param_name);
			g_string_append_printf(params_list, ", %s %s", type.name, param_name);

			if(iter.next == NULL)
			{
				break;
			}
		}
	}
	g_string_append_printf(priv->func_inlines, "static inline Ret %s_%s(%s* thiz%s)\n{\n", 
		priv->interface_lower, lower_func_name, priv->interface,  params_list->str);
	g_string_append_printf(priv->func_inlines, "	return_val_if_fail(thiz != NULL && thiz->%s != NULL, RET_FAIL);\n\n",
		lower_func_name);
	g_string_append_printf(priv->func_inlines, "	return thiz->%s(thiz%s);\n", lower_func_name, params_call->str);
	g_string_append_printf(priv->func_inlines, "}\n\n");

	g_string_append_printf(priv->func_types, "%s);\n", params_list->str);

	g_string_free(params_list, 1);
	g_string_free(params_call, 1);

	return TRUE;
}

static gboolean coder_intf_on_const(Coder* thiz, struct _IDL_CONST_DCL c)
{
	return TRUE;
}

static gboolean coder_intf_on_struct(Coder* thiz, struct _IDL_TYPE_STRUCT s)
{
	return TRUE;
}

static gboolean coder_intf_on_enum(Coder* thiz, struct _IDL_TYPE_ENUM e)
{
	return TRUE;
}

static gboolean coder_intf_on_union(Coder* thiz, struct _IDL_TYPE_UNION u)
{
	return TRUE;
}

static void coder_intf_destroy(Coder* thiz)
{
	if(thiz != NULL)
	{
		coder_intf_end_interface(thiz);

		g_free(thiz);
	}

	return;
}

Coder* coder_intf_create(const char* name)
{
	Coder* thiz = g_malloc0(sizeof(Coder) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->on_interface = coder_intf_on_interface;
		thiz->on_function  = coder_intf_on_function;
		thiz->on_const     = coder_intf_on_const;
		thiz->on_enum      = coder_intf_on_enum;
		thiz->on_struct    = coder_intf_on_struct;
		thiz->on_union     = coder_intf_on_union;
		thiz->destroy      = coder_intf_destroy;
	}

	return thiz;
}
