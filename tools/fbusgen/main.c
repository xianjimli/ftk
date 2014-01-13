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
#include "coder_client.h"
#include "coder_xxxx.h"
#include "coder_service.h"
#include "coder_share.h"

static gboolean coder_generator(IDL_tree_func_data *tfd, Coder* coder)
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
		return coder_on_interface(coder, name, parent);
	}
	else if(IDL_NODE_TYPE (p) == IDLN_OP_DCL)
	{
		return coder_on_function(coder, IDL_OP_DCL(p));
	}
	else if(IDL_NODE_TYPE (p) == IDLN_CONST_DCL)
	{
		return coder_on_const(coder, IDL_CONST_DCL(p));
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_STRUCT)
	{
		return coder_on_struct(coder, IDL_TYPE_STRUCT(p));
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_ENUM)
	{
		return coder_on_enum(coder, IDL_TYPE_ENUM(p));
	}
	else if(IDL_NODE_TYPE(p) == IDLN_TYPE_UNION)
	{
		return coder_on_union(coder, IDL_TYPE_UNION(p));
	}

	return TRUE;
}

struct _CoderNameAndCreator
{
	char* name;
	CoderCreate create;
}g_coders [] =
{
	{"intf", coder_intf_create},
	{"client", coder_client_create},
	{"service", coder_service_create},
	{"share", coder_share_create},
	{NULL, NULL}
};

static Coder* coder_create(const char* name)
{
	int i = 0;
	g_return_val_if_fail(name != NULL, NULL);

	for(i = 0; g_coders[i].name != NULL; i++)
	{
		if(strcmp(name, g_coders[i].name) == 0)
		{
			printf("Found coder %s.\n", name);
			return g_coders[i].create(name);
		}
	}

	printf("Cannt found coder %s, use xxxx\n", name);

	return coder_xxxx_create(name);
}

int main (int argc, char *argv[])
{
	int rv = 0;
	IDL_ns ns = {0};
	IDL_tree tree = {0};
	Coder* coder = NULL;
	const char* type = NULL;
	const char* filename = NULL;
	unsigned long parse_flags = 0;

	IDL_check_cast_enable (TRUE);

	if (argc < 3 || (type = strstr(argv[1], "-type=")) == NULL) 
	{
		fprintf (stderr, "usage: %s -type=intf|client|service|xxx idl \n", argv[0]);
		exit (1);	
	}

	type += 6;
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

	coder = coder_create(type);
	if(coder != NULL)
	{
		IDL_tree_walk_in_order (tree, (IDL_tree_func)coder_generator, coder);
		coder_destroy(coder);
	}

	IDL_ns_free (ns);
	IDL_tree_free (tree);

	return 0;
}
