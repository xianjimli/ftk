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

/*
 * File: luagen.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief: a code generator to generate lua glue functions.
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
 * 2010-03-07 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <libIDL/IDL.h>

#define STR_LENGTH 256

typedef struct 
{
	FILE* h;
	FILE* c;
	IDL_ns ns;
	IDL_tree tree;

	GString* str_init;
	GString* str_types;
	GString* str_const;
	GString* str_funcs;
	char interface[STR_LENGTH+1];
	char interface_lower[STR_LENGTH+1];

}CodeGenInfo;

#define G_STRING_FREE(str) if(str != NULL){g_string_free(str, 1); str = NULL;}

static gboolean lua_code_gen_end_interface(CodeGenInfo *info);

static void code_gen_info_reset(CodeGenInfo* info)
{
	G_STRING_FREE(info->str_types);
	G_STRING_FREE(info->str_funcs);
	G_STRING_FREE(info->str_init);
	G_STRING_FREE(info->str_const);

	if(info->h != NULL && info->h != stdout)
	{
		fclose(info->h);
		info->h = NULL;
	}

	if(info->c != NULL && info->c != stdout)
	{
		fclose(info->c);
		info->c = NULL;
	}

	return;
}

static char* name_to_upper(const char* src, char* dst, int skip)
{
	int i = 0;
	int j = 0;

	dst[0] = toupper(src[0]);
	for(i = 1, j = 1; src[i]; i++)
	{
		if(src[i] == '_')
		{
			continue;
		}
		else if(src[i-1] == '_')
		{
			dst[j++] = toupper(src[i]);
		}
		else
		{
			dst[j++] = src[i];
		}
	}
	dst[j] = '\0';

	return dst + skip;
}

static const char* name_to_lower(const char* src, char* dst)
{
	int i = 0;
	int j = 0;

	for(i = 0, j = 0; src[i] != '\0'; i++, j++)
	{
		if(isupper(src[i]))
		{
			if(i > 0)
			{
				dst[j++] = '_';
			}
		}
		dst[j] = tolower(src[i]);
	}
	dst[j] = '\0';

	return dst;
}

static void code_gen_info_init(CodeGenInfo* info, const char* intf)
{
	const char* name = intf;
	char filename[260] = {0};

	code_gen_info_reset(info);	
	info->str_types = g_string_sized_new(4096);
	info->str_funcs = g_string_sized_new(8192);
	info->str_init = g_string_sized_new(4096);
	info->str_const = g_string_sized_new(4096);

	strncpy(info->interface, name, sizeof(info->interface));
	name_to_lower(name, info->interface_lower);

	snprintf(filename, sizeof(filename), "lua_%s.h", info->interface_lower);
	info->h = fopen(filename, "w+");
	if(info->h == NULL) info->h = stdout;
	
	snprintf(filename, sizeof(filename), "lua_%s.c", info->interface_lower);
	info->c = fopen(filename, "w+");
	if(info->c == NULL) info->c = stdout;

	return;
}

static gboolean lua_code_gen_begin_interface(const char* name, const char* parent, CodeGenInfo *info)
{
	char parent_lower[STR_LENGTH+1] = {0};

	/*close last interface*/
	if(info->interface[0] && info->c != NULL && info->h != NULL)
	{
		lua_code_gen_end_interface(info);
	}

	if(parent != NULL)
	{
		name_to_lower(parent, parent_lower);
	}

	code_gen_info_init(info, name);

	g_string_append_printf(info->str_init, "int tolua_%s_init(lua_State* L)\n",	info->interface_lower);
	g_string_append(info->str_init, "{\n");
	g_string_append(info->str_init, "	tolua_open(L);\n");
	g_string_append(info->str_init, "	tolua_reg_types(L);\n");
	g_string_append(info->str_init, "	tolua_module(L, NULL, 0);\n");
	g_string_append_printf(info->str_init,  "	tolua_beginmodule(L, NULL);\n");
	if(!info->only_globals)
	{
		g_string_append_printf(info->str_init,  "	tolua_cclass(L,\"%s\", \"%s\", \"%s\", NULL);\n",
			info->interface, info->interface, parent);
		g_string_append_printf(info->str_init,  "	tolua_beginmodule(L, \"%s\");\n", info->interface);
	}

	g_string_append(info->str_types, "static void tolua_reg_types (lua_State* L)\n{\n");

	if(!info->only_globals)
	{
		g_string_append_printf(info->str_types, "	tolua_usertype(L, \"%s\");\n", info->interface);
	}
	
	fprintf(info->h, "/*This file is generated by luagen.*/\n");
	fprintf(info->h, "#include \"ftk.h\"\n");
	fprintf(info->h, "#include \"tolua++.h\"\n");
	fprintf(info->h, "#include \"%s.h\"\n", info->interface_lower);
	if(parent_lower[0])
	{
		fprintf(info->h, "#include \"lua_%s.h\"\n\n", parent_lower);
	}
	else
	{
		fprintf(info->h, "\n");
	}
	fprintf(info->h, "#ifndef LUA_%s_H\n", info->interface);
	fprintf(info->h, "#define LUA_%s_H\n\n", info->interface);
	if(parent != NULL && parent[0])
	{
		fprintf(info->h, "typedef %s %s;\n", parent, name);
	}
	fprintf(info->h, "int tolua_%s_init(lua_State* L);\n", info->interface_lower);

	fprintf(info->c, "/*This file is generated by luagen.*/\n");
	fprintf(info->c, "#include \"lua_%s.h\"\n", info->interface_lower);
	fprintf(info->c, "#include \"lua_ftk_callbacks.h\"\n\n");

	return TRUE;
}

static gboolean lua_code_gen_end_interface(CodeGenInfo *info)
{
	if(info->interface[0])
	{
		if(!info->only_globals)
		{
			g_string_append_printf(info->str_init, "	tolua_endmodule(L);\n");
		}
		else
		{
			g_string_append(info->str_init, info->str_const->str);
		}
		g_string_append_printf(info->str_init, "	tolua_endmodule(L);\n\n");
		g_string_append_printf(info->str_init, "\n	return 1;\n");
		g_string_append_printf(info->str_init, "}\n");
		g_string_append(info->str_types, "}\n\n");
	
		fprintf(info->c, "%s", info->str_types->str);
		fprintf(info->c, "%s", info->str_funcs->str);
		fprintf(info->c, "%s", info->str_init->str);
		fprintf(info->h, "\n#endif/*LUA_%s_H*/\n\n", info->interface);
	}
	
	return TRUE;
}

typedef struct _TypeInfo
{
	gboolean is_function;
	gboolean is_userdata;

	char name[STR_LENGTH+1];
	char lua_name[STR_LENGTH+1];
	char check[STR_LENGTH+1];
	char pop[STR_LENGTH+1];
	char push[STR_LENGTH+1];
	char free[STR_LENGTH+1];
	char orignal_name[STR_LENGTH+1];
}TypeInfo;


static void int_type_init(TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	strcpy(info->name     , "int");
	strcpy(info->lua_name , "lua_Number");
	strcpy(info->check    , "tolua_isnumber(L, %d, 0, &err)");
	strcpy(info->pop      , "tolua_tonumber");
	strcpy(info->push     , "	tolua_pushnumber(L, (lua_Number)retv);\n");
	strcpy(info->free     , "");

	return;
}

static void ret_type_init(TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	strcpy(info->name     , "Ret");
	strcpy(info->lua_name , "lua_Number");
	strcpy(info->check    , "tolua_isnumber(L, %d, 0, &err)");
	strcpy(info->pop      , "tolua_tonumber");
	strcpy(info->push     , "	tolua_pushnumber(L, (lua_Number)retv);\n");
	strcpy(info->free     , "");

	return;
}

static void str_type_init(TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	strcpy(info->name     , "char*");
	strcpy(info->lua_name , "char*");
	strcpy(info->check    , "tolua_isstring(L, %d, 0, &err)");
	strcpy(info->pop      , "(char*)tolua_tostring");
	strcpy(info->push     , "	tolua_pushstring(L, (char*)retv);\n");
	strcpy(info->free     , "");

	return;
}

static void func_type_init(const char* name, TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	info->is_function = 1;
	strcpy(info->name     , "char*");
	strcpy(info->lua_name , "void*");
	strcpy(info->check    , "tolua_isstring(L, %d, 0, &err)");
	strcpy(info->pop      , "(char*)tolua_tostring");
	strcpy(info->push     , "	assert(!\"not supported\");\n");
	strcpy(info->free     , "");

	return;
}

static void str_array_type_init(TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	strcpy(info->name     , "char**");
	strcpy(info->lua_name , "char**");
	strcpy(info->check    , "tolua_istable(L, %d, 0, &err)");
	strcpy(info->pop      , "tolua_tostrings");
	strcpy(info->push     , "	assert(!\"not supported\");\n");
	strcpy(info->free     , "	free(%s);\n");

	return;
}

static void userdata_type_init(const char* name, TypeInfo* info)
{
	memset(info, 0x00, sizeof(TypeInfo));
	char usertype[STR_LENGTH+1] = {0};
	strcpy(usertype, name);
	strcpy(info->name     , name);
	strcpy(info->lua_name , "void*");
	strcpy(info->check    , "tolua_isusertype(L, %d, \"");
	strcat(info->check    ,  name);
	strcat(info->check    , "\", 0, &err)");
	snprintf(info->pop, sizeof(info->pop), "*(%s*)tolua_tousertype", name);
	sprintf(info->push, "	{%s* copy=malloc(sizeof(%s)); if(copy != NULL) memcpy(copy, &retv, sizeof(%s));tolua_pushusertype_and_takeownership(L, (%s*)copy, \"%s\");}\n",
		info->name, info->name, info->name, info->name, usertype);
	strcpy(info->free     , "");

	return;
}

static void userdata_light_type_init(const char* name, TypeInfo* info)
{
	char* ptr = NULL;
	char usertype[STR_LENGTH+1] = {0};
	
	memset(info, 0x00, sizeof(TypeInfo));

	strcpy(info->name     , name);
	ptr = strstr(info->name, "Ptr");
	strcpy(ptr, "*");

	ptr = strstr(name, "Ptr");
	strncpy(usertype, name, ptr-name);
	strcpy(info->lua_name , "void*");
	strcpy(info->check    , "tolua_isusertype(L, %d, \"");
	strncat(info->check, name,  ptr - name);
	strcat(info->check    , "\", 0, &err)");
	strcpy(info->pop      , "tolua_tousertype");
	sprintf(info->push, "	tolua_pushusertype(L, (%s)retv, \"%s\");\n",
		info->name, usertype);
	strcpy(info->free     , "");

	return;
}

static gboolean is_pointer(const char* str)
{
	const char* ptr = strstr(str, "Ptr");

	return ptr != NULL && strlen(ptr) == 3;
}

static gboolean is_function(const char* str)
{
	const char* ptr = strstr(str, "Func");

	return ptr != NULL && strlen(ptr) == 4;
}

static int get_type_info(IDL_tree type, TypeInfo* info)
{
	if(IDL_NODE_IS_TYPE(type))
	{
		switch(IDL_NODE_TYPE(type))
		{
			case IDLN_TYPE_INTEGER:
			{
				int_type_init(info);
				break;
			}
			case IDLN_TYPE_STRING:
			{
				/*TODO*/
				break;
			}
			case IDLN_TYPE_CHAR:
			{
				/*TODO*/
				break;
			}
			case IDLN_TYPE_BOOLEAN:
			{
				/*TODO*/
				break;
			}
			case IDLN_TYPE_FLOAT:
			{
				/*TODO*/
				break;
			}
			default:break;
		}
	}
	else if(IDL_NODE_TYPE(type) == IDLN_IDENT)
	{
		const char* type_str = IDL_IDENT(type).str;
		if(strcmp(type_str, "StrPtr") == 0 || strcmp(type_str, "CStrPtr") == 0) 
		{
			str_type_init(info);
		}
		else if(strcmp(type_str, "StrArray") == 0 || strcmp(type_str, "CStrArray") == 0) 
		{
			str_array_type_init(info);
		}
		else if(is_pointer(type_str))
		{
			char* p = NULL;
			userdata_light_type_init(type_str, info);
		}
		else if(is_function(type_str))
		{
			func_type_init(type_str, info);
		}
		else if(strcmp(type_str, "Ret") == 0) 
		{
			ret_type_init(info);
		}
		else if(strcmp(type_str, "int") == 0 
			|| strcmp(type_str, "uint") == 0 
			|| strcmp(type_str, "size_t") == 0 
			|| strcmp(type_str, "short") == 0 
			|| strcmp(type_str, "ushort") == 0 
			|| strcmp(type_str, "char") == 0 
			|| strcmp(type_str, "uchar") == 0 
			|| strcmp(type_str, "long") == 0 
			|| strcmp(type_str, "ulong") == 0 ) 
		{
			int_type_init(info);
		}
		else
		{
			userdata_type_init(type_str, info);
			info->is_userdata = TRUE;
		}
		strcpy(info->orignal_name, type_str);
	}

	return 0;
}

static gboolean lua_code_gen_on_func_decl(struct _IDL_OP_DCL func, CodeGenInfo *info)
{
	int i = 0;
	char new_name[64] = {0};
	TypeInfo type_info = {0};
	struct _IDL_LIST iter = {0};
	gboolean is_static_func = FALSE;
	GString* call = g_string_sized_new(512);
	GString* retv = g_string_sized_new(512);
	GString* param_get = g_string_sized_new(512);
	GString* param_decl = g_string_sized_new(512);
	GString* param_free = g_string_sized_new(512);
	GString* param_check = g_string_sized_new(512);
	const char* func_name = IDL_IDENT(func.ident).str;

	if(func.op_type_spec != NULL)
	{
		get_type_info(func.op_type_spec, &type_info);
		g_string_append_printf(call, "	retv = %s(", func_name);
		g_string_append_printf(param_decl, "	%s retv;\n", type_info.name);
		g_string_append_printf(retv, type_info.push, type_info.lua_name, type_info.name);
	}
	else
	{
		g_string_append_printf(call, "	%s(", func_name);
	}
	
	g_string_append_printf(info->str_funcs, "static int lua_%s(lua_State* L)\n", func_name);
	g_string_append(info->str_funcs, "{\n");

	if(func.parameter_dcls != NULL)
	{
		for (i = 1, iter = IDL_LIST(func.parameter_dcls); iter.data != NULL; iter = IDL_LIST(iter.next)) 
		{
			const char* param_name = NULL;
			int attr = IDL_PARAM_DCL(iter.data).attr;

			get_type_info(IDL_PARAM_DCL(iter.data).param_type_spec, &type_info);
			param_name = IDL_IDENT(IDL_PARAM_DCL(iter.data).simple_declarator).str;

			if(i == 1)
			{
				is_static_func = strcmp(param_name, "thiz)") != 0;
			}
			else
			{
				if(attr != IDL_PARAM_OUT)
				{
					g_string_append_printf(param_check, " && ");
				}
				g_string_append(call, ", ");
			}

			g_string_append_printf(param_decl, "	%s %s;\n", type_info.name, param_name);
			g_string_append_printf(param_free, type_info.free, param_name);
			if(attr == IDL_PARAM_OUT)
			{
				g_string_append_printf(call, "&%s", param_name);
				g_string_append_printf(retv, type_info.push, type_info.lua_name, type_info.name);
			}
			else
			{
				if(type_info.is_function)
				{
					char func_name[STR_LENGTH+1] = {0};
					g_string_append_printf(call, "lua_%s, %s", name_to_lower(type_info.orignal_name, func_name), param_name);
				}
				else
				{
					g_string_append_printf(call, "%s", param_name);
				}
				g_string_append_printf(param_check, type_info.check, i);
				g_string_append_printf(param_get, "	%s = %s(L, %d, 0);\n", param_name, type_info.pop, i);
				i++;
			}
		
			if(iter.next == NULL) break;
		}
	}
	else
	{
		is_static_func = TRUE;
	}
	g_string_append(call, ");\n");

	if(i > 1)
	{
		g_string_append_printf(info->str_funcs, "	tolua_Error err = {0};\n", param_check->str);
	}
	g_string_append(info->str_funcs, param_decl->str);
	if(i > 1)
	{
		g_string_append_printf(info->str_funcs, "	int param_ok = %s;\n", param_check->str);
		g_string_append(info->str_funcs, "\n	return_val_if_fail(param_ok, 0);\n\n");
	}
	g_string_append(info->str_funcs, param_get->str);
	g_string_append(info->str_funcs, call->str);
	g_string_append(info->str_funcs, retv->str);
	g_string_append(info->str_funcs, param_free->str);
	g_string_append_printf(info->str_funcs, "\n	return 1;\n}\n\n", param_check->str);

	g_string_append_printf(info->str_init, "	tolua_function(L, \"%s\", lua_%s);\n",
		name_to_upper(func_name, new_name, strlen(info->interface)), func_name);

	g_string_free(call, 1);
	g_string_free(retv, 1);
	g_string_free(param_free, 1);
	g_string_free(param_decl, 1);
	g_string_free(param_check, 1);
	g_string_free(param_get, 1);

	return FALSE;
}

static gboolean lua_code_gen_on_const(struct _IDL_CONST_DCL constant, CodeGenInfo *info)
{
	if(IDL_NODE_TYPE (constant.const_type) == IDLN_TYPE_INTEGER)
	{
		const char* name = IDL_IDENT(constant.ident).str;
		g_string_append_printf(info->str_const, "	tolua_constant(L, \"%s\", %s);\n", name, name);
	}

	return FALSE;
}

static void lua_code_gen_get_func(CodeGenInfo *info, const char* name, TypeInfo* type_info, const char* var)
{
	g_string_append_printf(info->str_funcs, "static int tolua_get_%s_%s(lua_State* L)\n", name, var);
	g_string_append(info->str_funcs, "{\n");
	g_string_append_printf(info->str_funcs, "	%s retv;\n", type_info->lua_name);
	g_string_append_printf(info->str_funcs, "	%s* thiz = (%s*)  tolua_tousertype(L, 1, 0);\n", name, name);
	g_string_append(info->str_funcs, "	return_val_if_fail(thiz != NULL, 0);\n");
	if(type_info->is_userdata)
	{
		g_string_append_printf(info->str_funcs, "	retv = (%s)&(thiz->%s);\n", type_info->lua_name, var);
	}
	else
	{
		g_string_append_printf(info->str_funcs, "	retv = (%s)thiz->%s;\n", type_info->lua_name, var);
	}
	g_string_append_printf(info->str_funcs, type_info->push, type_info->lua_name);
	g_string_append(info->str_funcs, "\n	return 1;\n");
	g_string_append(info->str_funcs, "}\n\n");

	return;
}

static void lua_code_gen_set_func(CodeGenInfo *info, const char* name, TypeInfo* type_info, const char* var)
{
	g_string_append_printf(info->str_funcs, "static int tolua_set_%s_%s(lua_State* L)\n", name, var);
	g_string_append(info->str_funcs, "{\n");
	g_string_append_printf(info->str_funcs, "	%s* thiz = (%s*)  tolua_tousertype(L, 1, 0);\n", name, name);
	g_string_append(info->str_funcs, "	return_val_if_fail(thiz != NULL, 0);\n");
	g_string_append_printf(info->str_funcs, "	thiz->%s = (%s) (%s(L, 2, 0));\n", var, type_info->name, type_info->pop);
	g_string_append(info->str_funcs, "\n	return 1;\n");
	g_string_append(info->str_funcs, "}\n\n");

	return;
}

static gboolean lua_code_gen_on_struct(struct _IDL_TYPE_STRUCT s, CodeGenInfo *info)
{
	struct _IDL_LIST iter = {0};
	char lower_name[STR_LENGTH+1] = {0};
	const char* name = IDL_IDENT(s.ident).str;

	if(!info->only_globals)
	{
		return FALSE;
	}

	name_to_lower(name, lower_name);
	g_string_append_printf(info->str_types, "	tolua_usertype(L, \"%s\");\n", name);
	
	g_string_append_printf(info->str_init, "	tolua_cclass(L, \"%s\", \"%s\", \"\", NULL);\n",
			name, name);
	g_string_append_printf(info->str_funcs, "int lua_%s_create(lua_State* L)\n", lower_name);
	g_string_append_printf(info->str_funcs, "{\n");
	g_string_append_printf(info->str_funcs, "	%s* retv = calloc(1, sizeof(%s));\n", name, name);
	g_string_append_printf(info->str_funcs, "	tolua_pushusertype_and_takeownership(L, (void*)retv, \"%s\");\n\n", name);
	g_string_append_printf(info->str_funcs, "	return 1;\n}\n\n");

	g_string_append_printf(info->str_init, "	tolua_beginmodule(L, \"%s\");\n", name);
	g_string_append_printf(info->str_init, "	tolua_function(L, \"Create\", lua_%s_create);\n", lower_name);
	for (iter = IDL_LIST(s.member_list); iter.data != NULL; iter = IDL_LIST(iter.next)) 
	{
		TypeInfo type_info = {0};
		const char* var = NULL;
		struct _IDL_MEMBER member = IDL_MEMBER(iter.data);

		get_type_info(member.type_spec, &type_info);
		var = IDL_IDENT(IDL_LIST(member.dcls).data).str;

		lua_code_gen_get_func(info, name, &type_info, var);
		lua_code_gen_set_func(info, name, &type_info, var);
		g_string_append_printf(info->str_init, "	tolua_variable(L, \"%s\",tolua_get_%s_%s, tolua_set_%s_%s);\n",
			var, name, var, name, var);

		if(iter.next == NULL) break;
	}
  	g_string_append_printf(info->str_init, "	tolua_endmodule(L);\n");

	return FALSE;
}

static gboolean lua_code_gen_on_enum(struct _IDL_TYPE_ENUM e, CodeGenInfo *info)
{
	struct _IDL_LIST iter = {0};
	
	if(!info->only_globals)
	{
		return FALSE;
	}

	for (iter = IDL_LIST(e.enumerator_list); iter.data != NULL; iter = IDL_LIST(iter.next)) 
	{
		const char* name = IDL_IDENT(iter.data).str;
		g_string_append_printf(info->str_const, "	tolua_constant(L, \"%s\", %s);\n", name, name);

		if(iter.next == NULL) break;
	}

	return FALSE;
}

static gboolean lua_code_gen_on_union(struct _IDL_TYPE_UNION u, CodeGenInfo *info)
{
	struct _IDL_LIST iter = {0};
	
	if(!info->only_globals)
	{
		return FALSE;
	}
	/*TODO*/

	return FALSE;
}

static gboolean lua_gen_code(IDL_tree_func_data *tfd, CodeGenInfo *info)
{
	IDL_tree p = tfd->tree;

	if (IDL_NODE_TYPE (p) == IDLN_INTERFACE)
	{
		const char* parent = "";
		struct _IDL_INTERFACE intf = IDL_INTERFACE(p);
		const char* name = IDL_IDENT(intf.ident).str;

		if(intf.inheritance_spec != NULL)
		{
			struct _IDL_LIST list = IDL_LIST(intf.inheritance_spec);
			parent = IDL_IDENT(list.data).str;
		}
		return lua_code_gen_begin_interface(name, parent, info);
	}
	else if(IDL_NODE_TYPE (p) == IDLN_OP_DCL)
	{
		return lua_code_gen_on_func_decl(IDL_OP_DCL(p), info);
	}
	else if(IDL_NODE_TYPE (p) == IDLN_CONST_DCL)
	{
		return lua_code_gen_on_const(IDL_CONST_DCL(p), info);
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_STRUCT)
	{
		return lua_code_gen_on_struct(IDL_TYPE_STRUCT(p), info);
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_ENUM)
	{
		return lua_code_gen_on_enum(IDL_TYPE_ENUM(p), info);
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_UNION)
	{
		return lua_code_gen_on_union(IDL_TYPE_UNION(p), info);
	}

	return TRUE;
}

static gboolean fbus_gen_intf_end(CodeGenInfo *info)
{
	return TRUE;
}

static gboolean fbus_gen_intf(IDL_tree_func_data *tfd, CodeGenInfo *info)
{
}

int main (int argc, char *argv[])
{
	int rv = 0;
	IDL_ns ns = {0};
	IDL_tree tree = {0};
	CodeGenInfo info = {0};
	const char* type = NULL;
	const char* filename = NULL;
	unsigned long parse_flags = 0;

	IDL_check_cast_enable (TRUE);

	if (argc < 3 || (type = strstr(argv[1], "-type=")) == NULL) 
	{
		fprintf (stderr, "usage: %s -type=intf|proxy|service|xxx idl \n", argv[0]);
		exit (1);	
	}

	filename = argv[2];
	rv = IDL_parse_filename (filename, NULL, NULL, &tree, &ns, parse_flags, IDL_WARNING1);
	if (rv == IDL_ERROR) 
	{
		g_message ("IDL_ERROR: %s\n", filename);
		exit (1);	
	} 
	else if (rv < 0) 
	{
		perror (filename);
		exit (1);
	}

	info.ns = ns;
	info.tree = tree;

	if(strcmp(type, "intf") == 0)
	{
		IDL_tree_walk_in_order (tree, (IDL_tree_func)fbus_gen_intf, &info);
		fbus_gen_intf_end(&info);	
	}
	else if(strcmp(type, "proxy") == 0)
	{
		IDL_tree_walk_in_order (tree, (IDL_tree_func)fbus_gen_proxy, &info);
		fbus_gen_proxy_end(&info);	
	}
	else if(strcmp(type, "service") == 0)
	{
		IDL_tree_walk_in_order (tree, (IDL_tree_func)fbus_gen_service, &info);
		fbus_gen_service_end(&info);	
	}
	else
	{
		IDL_tree_walk_in_order (tree, (IDL_tree_func)fbus_gen_impl, &info);
		fbus_gen_impl_end(&info);	
	}

	IDL_ns_free (ns);
	IDL_tree_free (tree);
	code_gen_info_reset(&info);

	return 0;
}

