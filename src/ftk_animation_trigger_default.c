/*
 * File: ftk_animation_trigger_default.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   default animation trigger.
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
 * 2011-03-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_mmap.h"
#include "ftk_window.h"
#include "ftk_globals.h"
#include "ftk_xml_parser.h"
#include "ftk_animation_alpha.h"
#include "ftk_animation_scale.h"
#include "ftk_animation_expand.h"
#include "ftk_animation_trigger.h"
#include "ftk_animation_translate.h"

#define FTK_MAX_ANIM_EVT_NR 32
#define FTK_MAX_ANIMATION_NR 3
#define FTK_ANIM_EVENT_NAME_LEN 31

typedef struct _TypeAndAnimation
{
	char event[FTK_ANIM_EVENT_NAME_LEN+1];
	FtkAnimation* animations[FTK_MAX_ANIMATION_NR];
}TypeAndAnimation;

typedef struct _AnimationTriggerPrivInfo
{
	FtkCanvas* old_window;
	FtkCanvas* new_window;
	int type_and_animations_nr;
	TypeAndAnimation type_and_animations[FTK_MAX_ANIM_EVT_NR];
}PrivInfo;

typedef struct _NameAndCreator
{
	const char* name;
	FtkAnimationCreate create;
}NameAndCreator;

static const NameAndCreator s_name_and_creators[] =
{
	{"scale",     ftk_animation_scale_create},
	{"alpha",     ftk_animation_alpha_create},
	{"expand",    ftk_animation_expand_create},
	{"translate", ftk_animation_translate_create},
	{NULL, NULL}
};

static TypeAndAnimation* ftk_animation_trigger_default_find_by_event(FtkAnimationTrigger* thiz, const char* event)
{
	size_t i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, NULL);

	for(i = 0; i < FTK_MAX_ANIM_EVT_NR; i++)
	{
		if(strcmp(priv->type_and_animations[i].event, event) == 0)
		{
			return priv->type_and_animations+i;
		}
	}

	return NULL;
}

static Ret ftk_animation_trigger_default_ensure_canvas(FtkAnimationTrigger* thiz)
{
	int w = 0;
	int h = 0;
	FtkColor bg = {0};
	DECL_PRIV(thiz, priv);

	if(priv->old_window && priv->new_window)
	{
		return RET_OK;
	}

	bg.a = 0xff;
	w = ftk_display_width(ftk_default_display());
	h = ftk_display_height(ftk_default_display());

	if(priv->old_window == NULL)
	{
		priv->old_window = ftk_canvas_create(w, h, &bg);
	}
	
	if(priv->new_window == NULL)
	{
		priv->new_window = ftk_canvas_create(w, h, &bg);
	}

	return priv->old_window && priv->new_window ? RET_OK : RET_FAIL;
}

static Ret ftk_animation_trigger_default_on_event(FtkAnimationTrigger* thiz, FtkAnimationEvent* evt)
{
	size_t i = 0;
	DECL_PRIV(thiz, priv);
	const char* hint = NULL;
	FtkBitmap* bitmap = NULL;
	FtkBitmap* old_bitmap = NULL;
	FtkBitmap* new_bitmap = NULL;
	FtkCanvas* save_canvas = NULL;
	FtkRect old_win_rect = {0,0,0,0};
	FtkRect new_win_rect = {0,0,0,0};
	char event_name[FTK_ANIM_EVENT_NAME_LEN];
	TypeAndAnimation* type_and_animation = NULL;

	return_val_if_fail(evt != NULL, RET_FAIL);
	return_val_if_fail(ftk_animation_trigger_default_ensure_canvas(thiz) == RET_OK, RET_FAIL);

	if(evt->type == FTK_EVT_SHOW)
	{
		hint = ftk_window_get_animation_hint(evt->new_window);
	}
	else
	{
		FtkColor bg = {0xff, 0xff, 0xff, 0xff};
		hint = ftk_window_get_animation_hint(evt->old_window);
		if(evt->new_window == NULL)
		{
			ftk_canvas_lock_buffer(priv->new_window, &bitmap);
			ftk_bitmap_clear(bitmap, bg);
			ftk_canvas_unlock_buffer(priv->new_window);
		}
	}

	if(hint[0] == '\0')
	{
		return RET_OK;
	}

	ftk_snprintf(event_name, FTK_ANIM_EVENT_NAME_LEN, "%s_%s", hint, evt->type == FTK_EVT_SHOW ? "show" : "hide");
	type_and_animation = ftk_animation_trigger_default_find_by_event(thiz, event_name);
	if(type_and_animation == NULL)
	{
		ftk_logd("%s: %s not exist.\n", __func__, event_name);
	}

	return_val_if_fail(type_and_animation != NULL, RET_FAIL);
	
	save_canvas = ftk_shared_canvas();
	
  	if(evt->old_window != NULL)
  	{
		ftk_window_disable_update(evt->old_window);
   		FTK_GET_PAINT_RECT(evt->old_window, old_win_rect);
		ftk_set_shared_canvas(priv->old_window);
		ftk_canvas_set_clip_region(priv->old_window, NULL);
    	ftk_window_paint_forcely(evt->old_window);
	}

	if(evt->new_window != NULL)
	{
		ftk_window_disable_update(evt->new_window);
   		FTK_GET_PAINT_RECT(evt->new_window, new_win_rect);
   		ftk_set_shared_canvas(priv->new_window);
		ftk_canvas_set_clip_region(priv->new_window, NULL);
		ftk_window_paint_forcely(evt->new_window);
	}

	ftk_set_shared_canvas(save_canvas);
	if(evt->old_window != NULL)
	{
    	ftk_window_enable_update(evt->old_window);
	}

	if(evt->new_window != NULL)
	{
    	ftk_window_enable_update(evt->new_window);
  	}

	ftk_canvas_lock_buffer(priv->old_window, &old_bitmap);
	ftk_canvas_lock_buffer(priv->new_window, &new_bitmap);

	for(i = 0; i < FTK_MAX_ANIMATION_NR; i++)
	{
		if(type_and_animation->animations[i] != NULL)
		{
			FtkAnimation* anim = type_and_animation->animations[i];
			ftk_animation_run(anim, old_bitmap, new_bitmap, &old_win_rect, &new_win_rect);
		}
	}

	ftk_canvas_unlock_buffer(priv->old_window);
	ftk_canvas_unlock_buffer(priv->new_window);

	ftk_logd("%s: type=%d %s - %s\n", __func__, evt->type, 
		evt->old_window != NULL ? ftk_widget_get_text(evt->old_window) : "null",
		evt->new_window != NULL ? ftk_widget_get_text(evt->new_window) : "null");

	return RET_OK;
}

static void ftk_animation_trigger_default_destroy(FtkAnimationTrigger* thiz)
{
	if(thiz != NULL)
	{
		int i = 0;
		DECL_PRIV(thiz, priv);

		for(i = 0; i < priv->type_and_animations_nr; i++)
		{
			size_t j = 0;
			for(j = 0; j < FTK_MAX_ANIMATION_NR; j++)
			{
				if(priv->type_and_animations[i].animations[j] != NULL)
				{
					ftk_animation_destroy(priv->type_and_animations[i].animations[j]);
				}
			}
		}

		ftk_canvas_destroy(priv->old_window);
		ftk_canvas_destroy(priv->new_window);
		FTK_FREE(thiz);
	}

	return;
}

typedef struct _BuilderInfo
{
	TypeAndAnimation* current;
	FtkAnimationTrigger* trigger;
}BuilderInfo;

static void ftk_animation_builder_on_start(FtkXmlBuilder* thiz, const char* tag, const char** attrs)
{
	BuilderInfo* info = (BuilderInfo*)thiz->priv;
	DECL_PRIV(info->trigger, priv);
	return_if_fail(thiz != NULL && tag != NULL);

	if(strcmp(tag, "event") == 0)
	{
		const char* event = attrs[1];
		if(priv->type_and_animations_nr < FTK_MAX_ANIM_EVT_NR)
		{
			info->current = priv->type_and_animations + priv->type_and_animations_nr;
			priv->type_and_animations_nr++;
			ftk_strncpy(info->current->event, event, FTK_ANIM_EVENT_NAME_LEN);
		}
	}
	else if(strcmp(tag, "animations") != 0)
	{
		int i = 0;
		FtkAnimationCreate create = NULL;
		for(i = 0; s_name_and_creators[i].name != NULL; i++)
		{
			if(strcmp(s_name_and_creators[i].name, tag) == 0)
			{
				create = s_name_and_creators[i].create;
				break;
			}
		}
	
		if(create != NULL && info->current != NULL)
		{
			for(i = 0; i < FTK_MAX_ANIMATION_NR; i++)
			{
				if(info->current->animations[i] == NULL)
				{
					int j = 0;
					FtkAnimation* anim = create();

					for(j = 0; attrs[j] != NULL; j+=2)
					{
						ftk_animation_set_param(anim, attrs[j], attrs[j+1]);
					}
					info->current->animations[i] = anim;
					break;
				}
			}
		}
	}

	return;
}

static void ftk_animation_builder_destroy(FtkXmlBuilder* thiz)
{
	if(thiz != NULL)
	{
		FTK_FREE(thiz);
	}

	return;
}

static FtkXmlBuilder* ftk_animation_builder_create(FtkAnimationTrigger* trigger)
{
	FtkXmlBuilder* thiz = (FtkXmlBuilder*)FTK_ZALLOC(sizeof(FtkXmlBuilder) + sizeof(BuilderInfo));

	if(thiz != NULL)
	{
		BuilderInfo* info = (BuilderInfo*)thiz->priv;
		info->trigger = trigger;
		thiz->on_start_element = ftk_animation_builder_on_start;
		thiz->destroy          = ftk_animation_builder_destroy;
	}

	return thiz;
}

static Ret ftk_animation_trigger_parse_data(FtkAnimationTrigger* thiz, const char* xml, size_t length)
{
	FtkXmlParser*  parser = NULL;
	FtkXmlBuilder* builder = NULL;
	return_val_if_fail(xml != NULL, RET_FAIL);

	parser  = ftk_xml_parser_create();
	return_val_if_fail(parser != NULL, RET_FAIL);

	builder = ftk_animation_builder_create(thiz);
	if(builder != NULL)
	{
		ftk_xml_parser_set_builder(parser, builder);
		ftk_xml_parser_parse(parser, xml, length);
	}
	ftk_xml_builder_destroy(builder);
	ftk_xml_parser_destroy(parser);

	return RET_OK;
}

static Ret ftk_animation_trigger_parse_file(FtkAnimationTrigger* thiz, const char* filename)
{
	FtkMmap* m = NULL;
	Ret ret = RET_FAIL;
	return_val_if_fail(thiz != NULL && filename != NULL, RET_FAIL);
	
	m = ftk_mmap_create(filename, 0, -1);
	if(m == NULL)
	{
		ftk_logd("%s: mmap %s failed.\n", __func__, filename);
	}
	return_val_if_fail(m != NULL, RET_FAIL);
	ret = ftk_animation_trigger_parse_data(thiz, (const char*)ftk_mmap_data(m), ftk_mmap_length(m));
	ftk_mmap_destroy(m);

	return ret;
}

void ftk_animation_trigger_default_dump(FtkAnimationTrigger* thiz)
{
	int i = 0;
	int j = 0;
	DECL_PRIV(thiz, priv);
	
	for(i = 0; i < priv->type_and_animations_nr; i++)
	{
		ftk_logd("event: %s\n", priv->type_and_animations[i].event);
		for(j = 0; j < FTK_MAX_ANIMATION_NR; j++)
		{
			if(priv->type_and_animations[i].animations[j] != NULL)
			{
				ftk_logd("animation:%s\n", priv->type_and_animations[i].animations[j]->name);
				ftk_params_dump(priv->type_and_animations[i].animations[j]->params);
			}
		}
	}

	return;
}

FtkAnimationTrigger* ftk_animation_trigger_default_create(const char* theme, const char* name)
{
	FtkAnimationTrigger* thiz = FTK_NEW_PRIV(FtkAnimationTrigger);

	if(thiz != NULL)
	{
		char filename[FTK_MAX_PATH + 1] = {0};

		ftk_strs_cat(filename, FTK_MAX_PATH, 
			ftk_config_get_data_dir(ftk_default_config()), "/theme/", theme, "/", name, NULL);

		ftk_normalize_path(filename);
		thiz->on_event = ftk_animation_trigger_default_on_event;
		thiz->destroy = ftk_animation_trigger_default_destroy;

		if(ftk_animation_trigger_parse_file(thiz, filename) == RET_OK)
		{	
			//ftk_animation_trigger_default_dump(thiz);
			ftk_logd("%s: them=%s\n", __func__, theme);
		}
		else
		{
			FTK_FREE(thiz);
		}
	}

	return thiz;
}

