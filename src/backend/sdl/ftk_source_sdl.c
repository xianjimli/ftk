
#include "ftk.h"
#include "ftk_typedef.h"
#include "ftk_source_sdl.h"
#include <SDL.h>

typedef struct _PrivInfo
{
	FtkEvent event;
}PrivInfo;

static int ftk_source_sdl_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_sdl_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_sdl_dispatch(FtkSource* thiz)
{
	SDL_Event event;
	DECL_PRIV(thiz, priv);

	do
	{
		if(!SDL_PollEvent(&event))
		{
			usleep(20000);
			break;
		}

		/* TODO: event.type */

		switch(event.type)
		{
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEBUTTONDOWN:
			priv->event.type = FTK_EVT_MOUSE_DOWN;
			priv->event.u.mouse.x = event.button.x;
			priv->event.u.mouse.y = event.button.y;
			ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
			break;
		case SDL_MOUSEBUTTONUP:
			priv->event.type = FTK_EVT_MOUSE_UP;
			priv->event.u.mouse.x = event.button.x;
			priv->event.u.mouse.y = event.button.y;
			ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
			break;
		case SDL_MOUSEMOTION:
			if(event.motion.state != SDL_PRESSED)
			{
				break;
			}
			priv->event.type = FTK_EVT_MOUSE_MOVE;
			priv->event.u.mouse.x = event.motion.x;
			priv->event.u.mouse.y = event.motion.y;
			ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &priv->event);
			break;
		case SDL_QUIT:
			FTK_QUIT();
			break;
		default:
			break;
		}
	} while(1);

	return RET_OK;
}

static void ftk_source_sdl_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));
	}
}

FtkSource* ftk_source_sdl_create(void)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		thiz->get_fd = ftk_source_sdl_get_fd;
		thiz->check = ftk_source_sdl_check;
		thiz->dispatch = ftk_source_sdl_dispatch;
		thiz->destroy = ftk_source_sdl_destroy;
		thiz->ref = 1;
	}

	return thiz;
}
