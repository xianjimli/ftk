/*
 * File: fconf_xml.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   xml implementation for interface FConf.
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
 * 2010-08-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_path.h"
#include "fconf_xml.h"
#include "ftk_mmap.h"
#include "ftk_util.h"
#include "ftk_xml_parser.h"
#include "ftk_allocator.h"

typedef enum _XmlNodeAttr
{
	NODE_ATTR_READONLY = 1,
	NODE_ATTR_MODIFIED = 2
}XmlNodeAttr;

typedef struct _XmlNode
{
	char* name;
	char* value;
	unsigned int attr;
	struct _XmlNode* next;
	struct _XmlNode* prev;
	struct _XmlNode* parent;
	struct _XmlNode* children;
}XmlNode;

static XmlNode* xml_node_create(const char* name, const char* value)
{
	XmlNode* node = NULL;
	return_val_if_fail(name != NULL, NULL);

	if((node = FTK_ZALLOC(sizeof(XmlNode))) != NULL)
	{
		node->name = ftk_strdup(name);
		if(node->name != NULL)
		{
			if(value != NULL)
			{
				node->value = ftk_strdup(value);
			}
		}
		else
		{	
			FTK_FREE(node);
		}
	}

	return node;
}

static Ret xml_node_set_value(XmlNode* node, const char* value)
{
	return_val_if_fail(node != NULL, RET_FAIL);
	return_val_if_fail((node->attr & NODE_ATTR_READONLY) == 0, RET_FAIL);

	FTK_FREE(node->value);
	if(value != NULL)
	{
		node->value = ftk_strdup(value);
	}

	return RET_OK;
}

static Ret xml_node_set_readonly(XmlNode* node, int readonly)
{
	return_val_if_fail(node != NULL, RET_FAIL);

	if(readonly)
	{
		node->attr |= NODE_ATTR_READONLY;
	}
	else
	{
		node->attr &= ~NODE_ATTR_READONLY;
	}

	return RET_OK;
}

static Ret xml_node_set_modified(XmlNode* node, int modified)
{
	return_val_if_fail(node != NULL, RET_FAIL);

	if(modified)
	{
		node->attr |= NODE_ATTR_MODIFIED;
	}
	else
	{
		node->attr &= ~NODE_ATTR_MODIFIED;
	}

	return RET_OK;
}

static int xml_node_get_child_count(XmlNode* node)
{
	int nr = 0;
	XmlNode* iter = NULL;
	return_val_if_fail(node != NULL, 0);

	for(iter = node->children; iter != NULL; iter = iter->next)
	{
		nr++;
	}

	return nr;
}

static XmlNode* xml_node_get_child(XmlNode* node, size_t index)
{
	XmlNode* iter = NULL;
	return_val_if_fail(node != NULL, 0);

	for(iter = node->children; iter != NULL; iter = iter->next)
	{
		if(index == 0)
		{
			return iter;
		}
		else
		{
			index--;
		}
	}	

	return NULL;
}

static XmlNode* xml_node_find(XmlNode* node, FtkPath* path)
{
	return_val_if_fail(node != NULL && node->name != NULL && path != NULL, NULL);

	for(; node != NULL; node = node->next)
	{
		if(strcmp(node->name, ftk_path_current(path)) == 0)
		{
			if(ftk_path_is_leaf(path))
			{
				return node;
			}
			else if(node->children != NULL)
			{
				ftk_path_down(path);
				return xml_node_find(node->children, path);
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

static XmlNode* xml_node_append_sibling(XmlNode* node, const char* name, const char* value)
{
	XmlNode* iter = NULL;
	XmlNode* sibling = NULL;
	return_val_if_fail(name != NULL, NULL);

	if((sibling = xml_node_create(name, value)) != NULL)
	{
		if(node != NULL)
		{
			for(iter = node; iter->next != NULL; iter = iter->next)
			{
			}
			
			iter->next = sibling;
			sibling->prev = iter;
			sibling->parent = node->parent;
		}
	}

	return sibling;
}

static Ret xml_node_add(XmlNode* node, FtkPath* path, const char* value)
{
	XmlNode* iter = node;
	return_val_if_fail(node != NULL && node->name != NULL && path != NULL, RET_FAIL);

	for(; iter != NULL; iter = iter->next)
	{
		if(strcmp(iter->name, ftk_path_current(path)) == 0)
		{
			if(ftk_path_is_leaf(path))
			{
				xml_node_set_value(iter, value);
			}
			else
			{
				ftk_path_down(path);
				if(iter->children == NULL)
				{
					iter->children = xml_node_create(ftk_path_current(path), NULL);
					iter->children->parent = iter;
				}
				xml_node_add(iter->children, path, value);
			}
			
			return RET_OK;
		}
	}

	iter = xml_node_append_sibling(node, ftk_path_current(path), value);
	if(!ftk_path_is_leaf(path))
	{
		return xml_node_add(iter, path, value);
	}

	return RET_OK;
}

static void xml_node_destroy(XmlNode* node)
{
	XmlNode* iter = NULL;
	XmlNode* temp = NULL;
	
	if(node != NULL)
	{
		iter = node->children;
		while(iter != NULL)
		{
			temp = iter->next;
			xml_node_destroy(iter);
			iter = temp;
		}

		if(node->parent != NULL)
		{
			if(node->parent->children == node)
			{
				node->parent->children = node->next;
			}
		}

		if(node->prev != NULL)
		{
			node->prev->next = node->next;
		}

		if(node->next != NULL)
		{
			node->next->prev = node->prev;
		}

		FTK_ZFREE(node, sizeof(XmlNode));
	}

	return;
}

static Ret xml_node_save(XmlNode* node, FILE* fp)
{
	return_val_if_fail(node != NULL && fp != NULL, RET_FAIL);

	if(node->children == NULL)
	{
		fprintf(fp, "<%s value=\"%s\" readonly=\"%d\"/>\n", 
			node->name, node->value, node->attr & NODE_ATTR_READONLY ? 1 : 0);
	}
	else
	{
		XmlNode* iter = NULL;
		fprintf(fp, "<%s>\n", node->name);
		for(iter = node->children; iter != NULL; iter = iter->next)
		{
			xml_node_save(iter, fp);
		}
		fprintf(fp, "</%s>\n", node->name);
	}

	return RET_OK;
}

typedef struct _PrivInfo
{
	int modified;
	XmlNode* root;
	char* root_path;
	FtkPath* path;
	FConfOnChanged on_changed;
	void* on_changed_ctx;
}PrivInfo;

Ret fconf_xml_save(FConf* thiz)
{
	FILE* fp = NULL;
	XmlNode* iter = NULL;
	DECL_PRIV(thiz, priv);
	char filename[FTK_MAX_PATH+1] = {0};
	return_val_if_fail(thiz != NULL && priv->root != NULL, RET_FAIL);	

	for(iter = priv->root; iter != NULL; iter = iter->next)
	{
		if(iter->attr & NODE_ATTR_MODIFIED)
		{
			ftk_snprintf(filename, FTK_MAX_PATH, "%s%c%s.cnf", priv->root_path, FTK_PATH_DELIM, iter->name);
			if((fp = fopen(filename, "w+")) != NULL)
			{
				xml_node_save(iter, fp);
				xml_node_set_modified(iter, 0);
				fclose(fp);
			}
		}
	}

	priv->modified = 0;
	ftk_logd("%s: done\n", __func__);

	return RET_OK;
}

typedef struct _BuilderInfo
{
	XmlNode* root;
	XmlNode* current;
}BuilderInfo;

static void fconf_xml_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	int i = 0;
	int readonly = 0;
	XmlNode* node = NULL;
	const char* data = NULL;
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	return_if_fail(thiz != NULL && tag != NULL && attrs != NULL);
	
	for(i = 0; attrs[i] != NULL; i+=2)
	{
		const char* name = attrs[i];
		const char* value = attrs[i+1];
		if(strcmp(name, "value") == 0)
		{
			data = value;
		}
		else if(strcmp(name, "readonly") == 0)
		{
			readonly = ftk_str2bool(value);
		}
	}

	node = xml_node_append_sibling(info->current->children, tag, data);
	return_if_fail(node != NULL);
	if(readonly)
	{
		xml_node_set_readonly(node, readonly);
	}
	
	if(info->current->children == NULL)
	{
		info->current->children = node;
		node->parent = info->current;
	}
	info->current = node;

	return;
}

static void fconf_xml_builder_on_end(FtkXmlBuilder* thiz, const char* tag)
{
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	return_if_fail(info->current != NULL);

	if(info->current->parent != NULL)
	{
		info->current = info->current->parent;
	}

	return;
}

static void fconf_xml_builder_on_text(FtkXmlBuilder* thiz, const char* text, size_t length)
{
	return;
}

static void fconf_xml_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));
	}

	return;
}

static FtkXmlBuilder* fconf_xml_builder_create(void)
{
	FtkXmlBuilder* thiz = FTK_ZALLOC(sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));

	if(thiz != NULL)
	{
		thiz->on_start_element = fconf_xml_builder_on_start;
		thiz->on_end_element   = fconf_xml_builder_on_end;
		thiz->on_text		   = fconf_xml_builder_on_text;
		thiz->destroy		   = fconf_xml_builder_destroy;
	}

	return thiz;
}

static XmlNode*  fconf_xml_parse(const char* name, const char* xml, size_t length)
{
	XmlNode* node = NULL;
    FtkXmlParser* parser = NULL;
    FtkXmlBuilder* builder = NULL;
    return_val_if_fail(xml != NULL, NULL);

	node = xml_node_create(name, NULL);
	return_val_if_fail(node != NULL, NULL);

    if((parser  = ftk_xml_parser_create()) == NULL)
    {
    	xml_node_destroy(node);
    	return_val_if_fail(parser != NULL, NULL);
    }

    builder = fconf_xml_builder_create();
    if(builder != NULL)
    {
		BuilderInfo* priv      = (BuilderInfo* )builder->priv;
		priv->root = priv->current = node;
        ftk_xml_parser_set_builder(parser, builder);
        ftk_xml_parser_parse(parser, xml, length);
    }
    ftk_xml_builder_destroy(builder);
    ftk_xml_parser_destroy(parser);

    return node;
}

Ret  fconf_xml_load_buffer(FConf* thiz, const char* name, const char* xml, size_t length)
{
    DECL_PRIV(thiz, priv);
    XmlNode* node = NULL;
    XmlNode* iter = NULL;
    node = fconf_xml_parse(name, xml, length);
	return_val_if_fail(node != NULL, RET_FAIL);
	
	if(priv->root == NULL)
	{
		priv->root = node;
	}
	else
	{
		for(iter = priv->root; iter->next != NULL; iter = iter->next);

		iter->next = node;
		node->prev = iter;
	}

	return RET_OK;
}

Ret  fconf_xml_load_file(FConf* thiz, const char* filename)
{
    FtkMmap* m = NULL;
    char* p = NULL;
    char root_node_name[FTK_MAX_PATH + 1] = {0};
    return_val_if_fail(thiz != NULL && filename != NULL, RET_FAIL);

	p = strrchr(filename, FTK_PATH_DELIM);
	if(p != NULL)
	{
		strncpy(root_node_name, p, FTK_MAX_PATH);
	}
	else
	{
		strncpy(root_node_name, filename, FTK_MAX_PATH);
	}

    if((p = strrchr(root_node_name, '.')) != NULL)
    {
    	*p = '\0';
    }

    m = ftk_mmap_create(filename, 0, -1);
    return_val_if_fail(m != NULL, RET_FAIL);
	fconf_xml_load_buffer(thiz, root_node_name, ftk_mmap_data(m), ftk_mmap_length(m));
    ftk_mmap_destroy(m);

    return RET_OK;
}

Ret  fconf_xml_load_dir(FConf* thiz, const char* path)
{
    DIR* dir = NULL;
    struct dirent* iter = NULL;
    char filename[FTK_MAX_PATH+1] = {0};
    return_val_if_fail(thiz != NULL && path != NULL, RET_FAIL);
    
	dir = opendir(path);
    return_val_if_fail(dir != NULL, RET_FAIL);

    while((iter = readdir(dir)) != NULL)
    {
        if(iter->d_name[0] == '.') continue;
        if(strstr(iter->d_name, ".cnf") == NULL) continue;

        ftk_snprintf(filename, sizeof(filename)-1, "%s%c%s", path, FTK_PATH_DELIM, iter->d_name);
        fconf_xml_load_file(thiz, filename);
    }
    closedir(dir);

    return RET_OK;
}

Ret fconf_xml_load(FConf* thiz, const char* dir)
{
	return fconf_xml_load_dir(thiz, dir);
}

static Ret fconf_xml_reg_changed_notify(FConf* thiz, FConfOnChanged on_changed, void* ctx)
{
	DECL_PRIV(thiz, priv);

	priv->on_changed = on_changed;
	priv->on_changed_ctx = ctx;

	return RET_OK;
}

static Ret fconf_xml_on_changed(FConf* thiz, FConfChangeType change_type, const char* value)
{
	XmlNode* iter = NULL;
	DECL_PRIV(thiz, priv);

	ftk_path_root(priv->path);
	for(iter = priv->root; iter != NULL; iter = iter->next)
	{
		if(strcmp(iter->name, ftk_path_current(priv->path)) == 0)
		{
			xml_node_set_modified(iter, 1);
			break;
		}
	}

	if(priv->on_changed != NULL)
	{
		priv->on_changed(priv->on_changed_ctx, 1, change_type, ftk_path_full(priv->path), value);
	}

	priv->modified = 1;

	return RET_OK;
}


Ret fconf_xml_remove(FConf* thiz, const char* path)
{
	Ret ret = RET_FAIL;
	XmlNode* node = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->root != NULL && path != NULL, RET_FAIL);

	ftk_path_set_path(priv->path, path);
	if((node = xml_node_find(priv->root, priv->path)) != NULL)
	{
		if(priv->root == node)
		{
			priv->root = node->next;
		}

		ret = RET_OK;
		xml_node_destroy(node);
		fconf_xml_on_changed(thiz, FCONF_CHANGED_BY_REMOVE, NULL);
	}

	return ret;
}

Ret fconf_xml_get(FConf* thiz, const char* path, char** value)
{
	XmlNode* node = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->root != NULL && path != NULL && value != NULL, RET_FAIL);

	ftk_path_set_path(priv->path, path);
	if((node = xml_node_find(priv->root, priv->path)) != NULL)
	{
		*value = node->value;
	}
	else
	{
		*value = NULL;
	}

	return *value != NULL ? RET_OK : RET_FAIL;
}

Ret fconf_xml_set(FConf* thiz, const char* path, const char* value)
{
	Ret ret = RET_FAIL;
	XmlNode* node = NULL;
	DECL_PRIV(thiz, priv);
	FConfChangeType type = FCONF_CHANGED_BY_SET;
	return_val_if_fail(thiz != NULL && path != NULL && value != NULL, RET_FAIL);

	ftk_path_set_path(priv->path, path);
	if(priv->root == NULL)
	{
		type = FCONF_CHANGED_BY_ADD;
		if(ftk_path_is_leaf(priv->path))
		{
			priv->root = xml_node_create(ftk_path_current(priv->path), value);
		}
		else
		{
			priv->root = xml_node_create(ftk_path_current(priv->path), NULL);
			ret = xml_node_add(priv->root, priv->path, value);
		}
	}
	else
	{
		if((node = xml_node_find(priv->root, priv->path)) != NULL)
		{
			ret = xml_node_set_value(node, value);
		}
		else
		{
			ftk_path_root(priv->path);
			type = FCONF_CHANGED_BY_ADD;
			ret = xml_node_add(priv->root, priv->path, value);
		}
	}

	if(ret == RET_OK)
	{
		fconf_xml_on_changed(thiz, type, value);
	}

	return ret;
}

Ret fconf_xml_get_child_count(FConf* thiz, const char* path, int* nr)
{
	Ret ret = RET_FAIL;
	XmlNode* node = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->root != NULL && path != NULL && nr != NULL, RET_FAIL);

	*nr = 0;
	ftk_path_set_path(priv->path, path);
	if((node = xml_node_find(priv->root, priv->path)) != NULL)
	{
		ret = RET_OK;
		*nr = xml_node_get_child_count(node);
	}

	return ret;
}

Ret fconf_xml_get_child(FConf* thiz, const char* path, int index, char** child)
{
	Ret ret = RET_FAIL;
	XmlNode* node = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->root != NULL && path != NULL && child != NULL, RET_FAIL);

	*child = NULL;
	ftk_path_set_path(priv->path, path);
	if((node = xml_node_find(priv->root, priv->path)) != NULL)
	{
		if((node = xml_node_get_child(node, index)) != NULL)
		{
			ret = RET_OK;
			*child = node->name;
		}
	}

	return ret;
}

static Ret fconf_xml_lock(FConf* thiz)
{
	return RET_FAIL;
}

static Ret fconf_xml_unlock(FConf* thiz)
{
	return RET_FAIL;
}

void fconf_xml_destroy(FConf* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		XmlNode* temp = NULL;
		XmlNode* iter = priv->root;
		
		fconf_xml_save(thiz);
		while(iter != NULL)
		{
			temp = iter->next;
			xml_node_destroy(iter);
			iter = temp;
		}
		ftk_path_destroy(priv->path);
		FTK_FREE(priv->root_path);
		FTK_FREE(thiz);
	}

	return;
}

int fconf_xml_is_dirty(FConf* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, 0);

	return priv->modified;
}

FConf* fconf_xml_create(const char* dir)
{
	FConf* thiz = NULL;
	
	return_val_if_fail(dir != NULL, NULL);
	thiz = FTK_ZALLOC(sizeof(FConf) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->lock = fconf_xml_lock;
		thiz->unlock = fconf_xml_unlock;
		thiz->remove = fconf_xml_remove;
		thiz->set = fconf_xml_set;
		thiz->get = fconf_xml_get;
		thiz->get_child_count = fconf_xml_get_child_count;
		thiz->get_child = fconf_xml_get_child;
		thiz->reg_changed_notify = fconf_xml_reg_changed_notify;
		thiz->destroy = fconf_xml_destroy;

		priv->path = ftk_path_create(NULL);
		if(dir != NULL)
		{
			priv->root_path = ftk_strdup(dir);
			fconf_xml_load(thiz, dir);
		}
	}

	return thiz;
}

#ifdef FCONF_XML_TEST
#include <assert.h>
#include "fconf.c"
#include "ftk_allocator_default.h"

void test_default(void)
{
	FConf* thiz = NULL;
	thiz = fconf_xml_create("./config");
	fconf_test(thiz);
	fconf_xml_save(thiz);
	fconf_destroy(thiz);

	return;
}

const char* testcase = "<a1><b1><c1 value=\"data1\"/> <c2 value=\"data2\"/></b1>\
<b2><c1 value=\"data3\"/> <c2 value=\"data4\"/></b2></a1>";

void test_load(void)
{
	FConf* thiz = NULL;
	char* data = NULL;
	thiz = fconf_xml_create("./config");
	fconf_xml_load_buffer(thiz, "test", testcase, strlen(testcase)); 
	assert(fconf_get(thiz, "/test/a1/b1/c1", &data) == RET_OK);
	assert(strcmp(data, "data1") == 0);
	assert(fconf_get(thiz, "/test/a1/b1/c2", &data) == RET_OK);
	assert(strcmp(data, "data2") == 0);
	
	assert(fconf_get(thiz, "/test/a1/b2/c1", &data) == RET_OK);
	assert(strcmp(data, "data3") == 0);
	assert(fconf_get(thiz, "/test/a1/b2/c2", &data) == RET_OK);
	assert(strcmp(data, "data4") == 0);
	fconf_destroy(thiz);

	return;
}
int main(int argc, char* argv[])
{
#ifndef USE_STD_MALLOC
	ftk_set_allocator((ftk_allocator_default_create()));
#endif
	test_default();
	test_load();

	return 0;
}
#endif
