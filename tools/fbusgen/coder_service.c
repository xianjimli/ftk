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
	char* name;
	char* lower_name;
	GString* func_create;
	GString* func_empty;
	GString* func_dispatch;
	gboolean  has_interface;
    char interface[STR_LENGTH+1];
    char interface_lower[STR_LENGTH+1];
    char interface_upper[STR_LENGTH+1];
}PrivInfo;

static gboolean coder_service_gen_service(PrivInfo* priv, FILE* fp)
{
	fprintf(fp, "static const char* %s_service_get_name(FBusService* thiz)\n", priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "	return FBUS_SERVICE_%s;\n", priv->interface_upper);
	fprintf(fp, "}\n\n");
	fprintf(fp, "static Ret %s_service_on_client_connect(FBusService* thiz, int client_id)\n", 
		priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "\treturn RET_OK;\n");
	fprintf(fp, "}\n\n");
	fprintf(fp, "static Ret %s_service_on_client_disconnect(FBusService* thiz, int client_id)\n", 
		priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "\treturn RET_OK;\n");
	fprintf(fp, "}\n\n");
	fprintf(fp, "static Ret %s_service_handle_request(FBusService* thiz, int client_id, FBusParcel* req_resp)\n",
		priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "\tint req_code = fbus_parcel_get_int(req_resp);\n\n");
	fprintf(fp, "\tswitch(req_code)\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "%s", priv->func_dispatch->str);
	fprintf(fp, "\t\tdefault:break;\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn RET_OK;\n");
	fprintf(fp, "}\n\n");
	fprintf(fp, "static void %s_service_destroy(FBusService* thiz)\n", priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "\tif(thiz != NULL)\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t\tDECL_PRIV(thiz, priv);\n");
	fprintf(fp, "\t\t%s_destroy(priv->impl);\n", priv->interface_lower);
	fprintf(fp, "\t\tFTK_FREE(thiz);\n");
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn;\n");
	fprintf(fp, "}\n\n");
	fprintf(fp, "FBusService* %s_service_create(void)\n", priv->interface_lower);
	fprintf(fp, "{\n");
	fprintf(fp, "\tFBusService* thiz = FTK_ZALLOC(sizeof(FBusService)+sizeof(PrivInfo));\n");
	fprintf(fp, "\tif(thiz != NULL)\n");
	fprintf(fp, "\t{\n");
	fprintf(fp, "\t\tDECL_PRIV(thiz, priv);\n");
	fprintf(fp, "\t\tpriv->impl = %s_impl_create();\n", priv->interface_lower);
	fprintf(fp, "\t\tthiz->get_name = %s_service_get_name;\n", priv->interface_lower);
	fprintf(fp, "\t\tthiz->on_client_connect = %s_service_on_client_connect;\n", priv->interface_lower);
	fprintf(fp, "\t\tthiz->on_client_disconnect = %s_service_on_client_disconnect;\n", priv->interface_lower);
	fprintf(fp, "\t\tthiz->handle_request = %s_service_handle_request;\n", priv->interface_lower);
	fprintf(fp, "\t\tthiz->destroy = %s_service_destroy;\n", priv->interface_lower);
	fprintf(fp, "\t\tfbus_service_register(thiz);\n");
	fprintf(fp, "\t}\n\n");
	fprintf(fp, "\treturn thiz;\n");
	fprintf(fp, "}\n\n");
	fprintf(fp, "\n");

	return TRUE;
}

static gboolean coder_service_end_interface(Coder* thiz)
{
	FILE* fp = NULL;
	char h_filename[260] = {0};
	char c_filename[260] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	if(!priv->has_interface)
	{
		return TRUE;
	}
	
	mkdir(priv->interface, 0777);
	
	snprintf(h_filename, sizeof(h_filename) - 1, "%s/%s_service.h", priv->interface, priv->interface_lower);
	fp = fopen(h_filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#ifndef %s_SERVICE_H\n", priv->interface_upper);
		fprintf(fp, "#define %s_SERVICE_H\n\n", priv->interface_upper);
		fprintf(fp, "#include \"fbus_service.h\"\n\n");
		fprintf(fp, "FTK_BEGIN_DECLS\n\n");
		fprintf(fp, "FBusService* %s_service_create(void);\n\n", priv->interface_lower);
		fprintf(fp, "FTK_END_DECLS\n\n");
		fprintf(fp, "#endif/*%s_SERVICE_H*/\n", priv->interface_upper);
		fclose(fp);
	}
	
	snprintf(c_filename, sizeof(c_filename) - 1, "%s/%s_service.c", priv->interface,  priv->interface_lower);
	fp = fopen(c_filename, "w+");
	if(fp != NULL)
	{
		coder_write_header(fp);
		fprintf(fp, "#include \"%s.h\"\n", priv->interface_lower);
		fprintf(fp, "#include \"%s_impl.h\"\n", priv->interface_lower);
		fprintf(fp, "#include \"%s_share.h\"\n", priv->interface_lower);
		fprintf(fp, "#include \"%s_service.h\"\n\n", priv->interface_lower);
		fprintf(fp, "typedef struct _PrivInfo\n");
		fprintf(fp, "{\n");
		fprintf(fp, "	%s* impl;\n", priv->interface);
		fprintf(fp, "}PrivInfo;\n\n");
		fprintf(fp, "%s\n", priv->func_empty->str);
		coder_service_gen_service(priv, fp);
		fclose(fp);
	}

	g_string_free(priv->func_empty, 1);
	g_string_free(priv->func_create, 1);
	g_string_free(priv->func_dispatch, 1);

	priv->has_interface = FALSE;

	return TRUE;
}

static gboolean coder_service_on_interface(Coder* thiz, const char* name, const char* parent)
{
	PrivInfo* priv = (PrivInfo*)thiz->priv;

	coder_service_end_interface(thiz);
	
	priv->has_interface = 1;
	strncpy(priv->interface, name, STR_LENGTH);
	coder_name_to_lower(name, priv->interface_lower);

	strcpy(priv->interface_upper, priv->interface_lower);
	coder_to_upper(priv->interface_upper);

	priv->func_empty = g_string_sized_new(4096);
	priv->func_create = g_string_sized_new(4096);
	priv->func_dispatch = g_string_sized_new(4096);

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
	}
	else if(strcmp(name, "String") == 0)
	{
		strcat(type->name, attr == IDL_PARAM_OUT ? "" : "const ");
		strcat(type->name, "char*");
	}
	else if(strcmp(name, "FBusBinary") == 0)
	{
		type->is_binary = TRUE;
	}
	else
	{
		strcat(type->name, attr == IDL_PARAM_OUT ? "" : "const ");
		strcat(type->name, name);
		strcat(type->name, attr == IDL_PARAM_IN ? "*" : "");
	}

	return TRUE;
}

static gboolean coder_service_on_function(Coder* thiz, struct _IDL_OP_DCL f)
{
	int i = 0;
	TypeInfo  type = {0};
	const char* param_name = NULL;
	struct _IDL_LIST iter = {0};
	char lower_func_name[STR_LENGTH+1] = {0};
	PrivInfo* priv = (PrivInfo*)thiz->priv;
	char req_coder_name[STR_LENGTH+1];
	const char* func_name = IDL_IDENT(f.ident).str;
	GString* params_list = g_string_sized_new(1024);
	GString* params_in = g_string_sized_new(1024);
	GString* params_out = g_string_sized_new(1024);
	GString* params_call = g_string_sized_new(1024);
	GString* params_decls = g_string_sized_new(1024);
	GString* params_free = g_string_sized_new(1024);

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
			g_string_append_printf(params_decls, "\t%s %s = {0};\n", type.name, param_name);
			g_string_append_printf(params_call, ", %s%s", attr == IDL_PARAM_OUT ? "&" : "", param_name);

			if(attr == IDL_PARAM_OUT)
			{
				if(strcmp(type.org_name, "int") == 0)
				{
					g_string_append_printf(params_out, "\t\t\tfbus_parcel_write_int(req_resp, %s);\n", param_name);
				}
				else if(strcmp(type.org_name, "String") == 0)
				{
					g_string_append_printf(params_out, "\t\t\tfbus_parcel_write_string(req_resp, %s);\n", param_name);
					g_string_append_printf(params_free, "\t\t\tFTK_FREE(%s);\n", param_name);
				}
				else
				{
					g_string_append_printf(params_out, "\t\t\tfbus_parcel_write_data(req_resp, &%s, sizeof(%s));\n", 
						param_name, param_name);
				}
			}

			if(attr == IDL_PARAM_IN)
			{
				if(strcmp(type.org_name, "int") == 0)
				{
					g_string_append_printf(params_in, "\t%s = fbus_parcel_get_int(req_resp);\n", param_name);
				}
				else if(strcmp(type.org_name, "String") == 0)
				{
					g_string_append_printf(params_in, "\t%s = fbus_parcel_get_string(req_resp);\n", param_name);
				}
				else
				{
					g_string_append_printf(params_in, "\t%s = (%s)fbus_parcel_get_data(req_resp, sizeof(%s));\n", 
						param_name, type.name, param_name);
				}
			}

			if(iter.next == NULL)
			{
				break;
			}
		}
	}
	coder_name_to_lower(func_name, lower_func_name);
	snprintf(req_coder_name, sizeof(req_coder_name)-1, "%s_%s", priv->interface_lower, lower_func_name);
	coder_to_upper(req_coder_name);
	g_string_append_printf(priv->func_dispatch, "\t\tcase %s:\n", req_coder_name);
	g_string_append_printf(priv->func_dispatch, "\t\t{\n");
	g_string_append_printf(priv->func_dispatch, "\t\t\t%s_marshal_%s(thiz, req_resp);\n",
		priv->interface_lower, lower_func_name);
	g_string_append_printf(priv->func_dispatch, "\t\t\tbreak;\n");
	g_string_append_printf(priv->func_dispatch, "\t\t}\n");
	g_string_append_printf(priv->func_empty, "static Ret %s_marshal_%s(FBusService* thiz, FBusParcel* req_resp)\n{\n", 
		priv->interface_lower, lower_func_name);
	g_string_append_printf(priv->func_empty, "%s\n", params_decls->str);
	g_string_append_printf(priv->func_empty, "\tRet ret = RET_FAIL;\n");
	g_string_append_printf(priv->func_empty, "\tDECL_PRIV(thiz, priv);\n");
	g_string_append_printf(priv->func_empty,  
		"	return_val_if_fail(priv != NULL && priv->impl != NULL && req_resp != NULL, RET_FAIL);\n\n");
	g_string_append_printf(priv->func_empty, "%s\n", params_in->str);
	g_string_append_printf(priv->func_empty, "\tif((ret = %s_%s(priv->impl%s)) == RET_OK)\n",
		priv->interface_lower, lower_func_name, params_call->str);
	g_string_append_printf(priv->func_empty, "\t{\n");
	g_string_append_printf(priv->func_empty, "\t\tfbus_parcel_reset(req_resp);\n");
	g_string_append_printf(priv->func_empty, "\t\tfbus_parcel_write_int(req_resp, ret);\n");
	if(params_out->str != NULL && strlen(params_out->str) > 0)
	{
		g_string_append_printf(priv->func_empty, "\t\tif(ret == RET_OK)\n");
		g_string_append_printf(priv->func_empty, "\t\t{\n");
		g_string_append_printf(priv->func_empty, "%s", params_out->str);
		g_string_append_printf(priv->func_empty, "%s", params_free->str);
		g_string_append_printf(priv->func_empty, "\t\t}\n");
	}
	g_string_append_printf(priv->func_empty, "\t}\n\n");
	g_string_append_printf(priv->func_empty, "\treturn ret;\n");
	g_string_append_printf(priv->func_empty, "}\n\n");
	
	g_string_append_printf(priv->func_create, "		thiz->%s = %s_marshal_%s;\n",
		lower_func_name, priv->interface_lower, lower_func_name);

	g_string_free(params_list, 1);
	g_string_free(params_in, 1);
	g_string_free(params_out, 1);
	g_string_free(params_call, 1);
	g_string_free(params_decls, 1);
	g_string_free(params_free, 1);

	return TRUE;
}

static gboolean coder_service_on_const(Coder* thiz, struct _IDL_CONST_DCL c)
{
	return TRUE;
}

static gboolean coder_service_on_struct(Coder* thiz, struct _IDL_TYPE_STRUCT s)
{
	return TRUE;
}

static gboolean coder_service_on_enum(Coder* thiz, struct _IDL_TYPE_ENUM e)
{
	return TRUE;
}

static gboolean coder_service_on_union(Coder* thiz, struct _IDL_TYPE_UNION u)
{
	return TRUE;
}

static void coder_service_destroy(Coder* thiz)
{
	if(thiz != NULL)
	{
		PrivInfo* priv = (PrivInfo*)thiz->priv;
		
		coder_service_end_interface(thiz);
		g_free(priv->name);
		g_free(priv->lower_name);
		g_free(thiz);
	}

	return;
}

Coder* coder_service_create(const char* name)
{
	Coder* thiz = g_malloc0(sizeof(Coder) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		PrivInfo* priv = (PrivInfo*)thiz->priv;
		
		priv->name = g_strdup(name);
		priv->lower_name = g_strdup(name);
		coder_to_lower(priv->lower_name);
		thiz->on_interface = coder_service_on_interface;
		thiz->on_function  = coder_service_on_function;
		thiz->on_const     = coder_service_on_const;
		thiz->on_enum      = coder_service_on_enum;
		thiz->on_struct    = coder_service_on_struct;
		thiz->on_union     = coder_service_on_union;
		thiz->destroy      = coder_service_destroy;
	}

	return thiz;
}
