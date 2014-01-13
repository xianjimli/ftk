/*
 * File: ftk_main_loop_select.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   select based main loop
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
 * 2010-03-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_main_loop.h"
#include "ftk_source_primary.h"

struct _FtkMainLoop
{
	int running;
	fd_set fdset;
	fd_set err_fdset;
	FtkSourcesManager* sources_manager;
};

FtkMainLoop* ftk_main_loop_create(FtkSourcesManager* sources_manager)
{
	FtkMainLoop* thiz = NULL;
	return_val_if_fail(sources_manager != NULL, NULL);

	thiz = (FtkMainLoop*)FTK_ZALLOC(sizeof(FtkMainLoop));
	if(thiz != NULL)
	{
		thiz->sources_manager = sources_manager;
		FD_ZERO(&thiz->fdset);
	}

	return thiz;
}

Ret ftk_main_loop_run(FtkMainLoop* thiz)
{
	int i = 0;
	int n = 0;
	int fd = 0;
	int mfd = 0;
	int ret = 0;
	int wait_time = 3000;
	int source_wait_time = 0;
	struct timeval tv = {0};
	FtkSource* source = NULL;

	thiz->running = 1;
	while(thiz->running)
	{
		n = 0;
		mfd = 0;
		wait_time = 3600 * 1000;
		
		FD_ZERO(&thiz->fdset);
		FD_ZERO(&thiz->err_fdset);

		for(i = 0; i < ftk_sources_manager_get_count(thiz->sources_manager); i++)
		{
			source = ftk_sources_manager_get(thiz->sources_manager, i);
			if((fd = ftk_source_get_fd(source)) >= 0)
			{
				FD_SET(fd, &thiz->fdset);
				FD_SET(fd, &thiz->err_fdset);
				if(mfd < fd) mfd = fd;
				n++;
			}

			source_wait_time = ftk_source_check(source);
			if(source_wait_time >= 0 && source_wait_time < wait_time)
			{
				wait_time = source_wait_time;
			}
		}

		tv.tv_sec = wait_time/1000;
		tv.tv_usec = (wait_time%1000) * 1000;

		ret = select(mfd + 1, &thiz->fdset, NULL, &thiz->err_fdset, &tv);

		for(i = 0; i < ftk_sources_manager_get_count(thiz->sources_manager);)
		{
			if(ftk_sources_manager_need_refresh(thiz->sources_manager))
			{
				break;
			}

			source = ftk_sources_manager_get(thiz->sources_manager, i);

			if(source->disable > 0)
			{
				ftk_sources_manager_remove(thiz->sources_manager, source);
				ftk_logd("%s:%d remove %p(disable)\n", __func__, __LINE__, source);

				continue;
			}

			if((fd = ftk_source_get_fd(source)) >= 0 && ret != 0)
			{
				if(ret > 0 && FD_ISSET(fd, &thiz->fdset))
				{
					if(ftk_source_dispatch(source) != RET_OK)
					{
						/*as current is removed, the next will be move to current, so dont call i++*/
						ftk_sources_manager_remove(thiz->sources_manager, source);
						ftk_logd("%s:%d remove %p\n", __func__, __LINE__, source);
					}
					else
					{
						i++;
					}
					continue;
				}
				else if(FD_ISSET(fd, &thiz->err_fdset))
				{
					ftk_sources_manager_remove(thiz->sources_manager, source);
					ftk_logd("%s:%d remove %p(error).\n", __func__, __LINE__, source);
					continue;
				}
			}

			if((source_wait_time = ftk_source_check(source)) == 0)
			{
				if(ftk_source_dispatch(source) != RET_OK)
				{
					/*as current is removed, the next will be move to current, so dont call i++*/
					ftk_sources_manager_remove(thiz->sources_manager, source);
					//ftk_logd("%s:%d remove %p\n", __func__, __LINE__, source);
				}
				else
				{
					i++;
				}
				continue;
			}

			i++;
		}
	}

	return RET_OK;
}

Ret ftk_main_loop_quit(FtkMainLoop* thiz)
{
	FtkEvent event;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	ftk_event_init(&event, FTK_EVT_NOP);
	thiz->running = 0;

	return ftk_source_queue_event(ftk_primary_source(), &event);
}

void ftk_main_loop_destroy(FtkMainLoop* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkMainLoop));
	}

	return;
}

