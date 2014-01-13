/*
 * File: ftk_input_method_py.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   google pinyin input method.
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
 * 2011-04-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "pinyinime.h"
#include "ftk_file_system.h"
#include "ftk_input_method_gpinyin.h"

using namespace ime_pinyin;

typedef struct _PrivInfo
{
	int raw_text_size;

	FtkWidget* editor;
	FtkInputType input_type;
	FtkCommitInfo commit_info;
	char16 utf16[FTK_IM_WORD_LENGTH];
	char utf8[FTK_IM_WORD_LENGTH << 2];
}PrivInfo;

static Ret  ftk_input_method_gpinyin_handle_event(FtkInputMethod* thiz, FtkEvent* event);

static Ret  ftk_input_method_gpinyin_init(FtkInputMethod* thiz)
{
	Ret ret = RET_FAIL;
	char dict_filename[FTK_MAX_PATH+1];
	char user_dict_filename[FTK_MAX_PATH+1];

	ftk_snprintf(dict_filename, FTK_MAX_PATH, "%s/data/gpinyin.dat", DATA_DIR);
	if(!ftk_file_exist(dict_filename))
	{
		ftk_snprintf(dict_filename, FTK_MAX_PATH, "%s/data/gpinyin.dat", LOCAL_DATA_DIR);
		ftk_snprintf(user_dict_filename, FTK_MAX_PATH, "%s/data/gpinyin_user.dat", LOCAL_DATA_DIR);
	}
	else
	{
		ftk_snprintf(user_dict_filename, FTK_MAX_PATH, "%s/data/gpinyin_user.dat", DATA_DIR);
	}

	ret = im_open_decoder(dict_filename, user_dict_filename) ? RET_OK : RET_FAIL;
	im_set_max_lens(FTK_IM_RAW_TEXT_LENGTH, FTK_IM_WORD_LENGTH);

	return ret;
}

static Ret  ftk_input_method_gpinyin_reset(FtkInputMethod* thiz)
{
	im_reset_search();

	return RET_OK;
}

static Ret  ftk_input_method_gpinyin_activate(FtkInputMethod* thiz)
{
	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
		(FtkListener)ftk_input_method_gpinyin_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_gpinyin_deactivate(FtkInputMethod* thiz)
{
	ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
		(FtkListener)ftk_input_method_gpinyin_handle_event, thiz);
	ftk_input_method_gpinyin_reset(thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_gpinyin_focus_in(FtkInputMethod* thiz, FtkWidget* editor)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && editor != NULL, RET_FAIL);

	priv->editor = editor;

	return ftk_input_method_gpinyin_activate(thiz);
}

static Ret  ftk_input_method_gpinyin_focus_out(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->editor = NULL;
	
	return ftk_input_method_gpinyin_deactivate(thiz);
}

static Ret  ftk_input_method_gpinyin_set_type(FtkInputMethod* thiz, FtkInputType input_type)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->input_type = input_type;

	return RET_OK;
}

#define IS_IME_CHAR(c) ((c < 0xff) && (((c) >= 'a' && (c) <= 'z') || (c) == '\''))

static Ret  ftk_input_method_gpinyin_handle_event(FtkInputMethod* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_FAIL);
	
	switch(event->type)
	{
		case FTK_EVT_KEY_DOWN:
		{
			FtkEvent evt;
			int matched_nr = 0;
			int code = event->u.key.code;
		
			if((code != FTK_KEY_BACKSPACE) && !IS_IME_CHAR(code))
			{
				break;
			}

			if(code == FTK_KEY_BACKSPACE)
			{
				if(priv->raw_text_size == 0)
				{
					break;
				}
				else
				{
					priv->raw_text_size--;
					priv->commit_info.raw_text[priv->raw_text_size] = '\0';
				}
			}
			
			if(priv->raw_text_size >= FTK_IM_RAW_TEXT_LENGTH)
			{
				ret = RET_REMOVE;
				break;
			}

			if(IS_IME_CHAR(code))
			{
				priv->commit_info.raw_text[priv->raw_text_size++] = 0xff & (event->u.key.code);
				priv->commit_info.raw_text[priv->raw_text_size] = '\0';
			}

			priv->commit_info.candidate_nr = 0;
			if(priv->raw_text_size > 0)
			{
				matched_nr = im_search(priv->commit_info.raw_text, strlen(priv->commit_info.raw_text));
			}
			else
			{
				im_reset_search();
			}

			if(matched_nr > 0)
			{
				int i = 0;
				int n = 0;
				int offset = 0;
				FtkCommitInfo* info = &(priv->commit_info);

				info->candidate_nr = 0;
				info->candidates[0] = '\0';
				for(i = 0; i < matched_nr; i++)
				{
					im_get_candidate(i, priv->utf16, FTK_IM_WORD_LENGTH);
					utf16_to_utf8(priv->utf16, FTK_IM_WORD_LENGTH, priv->utf8, FTK_IM_WORD_LENGTH << 2);
					
					n = strlen(priv->utf8);
					if((offset + n + 1) < FTK_IM_CANDIDATE_BUFF_LENGTH 
						&& info->candidate_nr < FTK_IM_MAX_CANDIDATES)
					{
						info->candidate_nr++;
						strcpy(info->candidates+offset, priv->utf8);
						offset += n + 1;
						ftk_logd("%d/%d %s\n", i, matched_nr, priv->utf8);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				priv->commit_info.candidate_nr = (priv->raw_text_size) > 0 ? 1 : 0;
				strcpy(priv->commit_info.candidates, priv->commit_info.raw_text);
			}
			evt.type = FTK_EVT_IM_PREEDIT;
			evt.u.extra = &(priv->commit_info);
			evt.widget = priv->editor;

			ftk_widget_event(priv->editor, &evt);
			
			ret = RET_REMOVE;

			break;
		}
		case FTK_EVT_IM_ACT_COMMIT:
		{
			priv->raw_text_size = 0;
			priv->commit_info.raw_text[priv->raw_text_size] = '\0';

			break;
		}
		default:break;
	}

	return ret;
}

static void ftk_input_method_gpinyin_destroy(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkInputMethod) + sizeof(PrivInfo));
		im_close_decoder();
	}

	return;
}

extern "C" FtkInputMethod* ftk_input_method_gpinyin_create()
{
	FtkInputMethod* thiz = (FtkInputMethod*)FTK_ZALLOC(sizeof(FtkInputMethod) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->ref  = 1;
		thiz->name = _("Google PinYin");	
		thiz->set_type     = ftk_input_method_gpinyin_set_type;
		thiz->focus_in     = ftk_input_method_gpinyin_focus_in;
		thiz->focus_out    = ftk_input_method_gpinyin_focus_out;
		thiz->handle_event = ftk_input_method_gpinyin_handle_event;
		thiz->destroy      = ftk_input_method_gpinyin_destroy;
	
		ftk_input_method_gpinyin_init(thiz);

	}

	return thiz;
}
