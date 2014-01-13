#include "ftk.h"
#include "ftk_wnd_manager.h"
#include "ftk_display_rotate.h"

static inline int is_mouse_event(FtkEvent* event)
{
	if(event->type == FTK_EVT_MOUSE_DOWN || event->type == FTK_EVT_MOUSE_UP 
		|| event->type == FTK_EVT_MOUSE_MOVE || event->type == FTK_EVT_MOUSE_LONG_PRESS)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Ret  ftk_wnd_manager_set_rotate(FtkWndManager* thiz, FtkRotate rotate)
{
	FtkRotate old = FTK_ROTATE_0;
	return_val_if_fail(thiz != NULL, RET_FAIL);

	old = ftk_display_get_rotate(ftk_default_display());
	
	if(old != rotate)
	{
		FtkEvent event;
		ftk_event_init(&event, FTK_EVT_SCREEN_ROTATED);
		ftk_wnd_manager_queue_event(thiz, &event);
	}

	return ftk_display_set_rotate(ftk_default_display(), rotate);
}

Ret  ftk_wnd_manager_queue_event_auto_rotate(FtkWndManager* thiz, FtkEvent* event)
{
	FtkRotate rotate = ftk_display_get_rotate(ftk_default_display());
	FtkDisplay* display = ftk_display_get_real_display(ftk_default_display());

	if(is_mouse_event(event))
	{
		int width = ftk_display_width(display);
		int height = ftk_display_height(display);
		
		switch(rotate)
		{
			case FTK_ROTATE_90:
			{
				int y = event->u.mouse.y;
				event->u.mouse.y = width - event->u.mouse.x;
				event->u.mouse.x = y;
				break;
			}
			case FTK_ROTATE_180:
			{
				event->u.mouse.x = width - event->u.mouse.x;
				event->u.mouse.y = height - event->u.mouse.y;
				break;
			}
			case FTK_ROTATE_270:
			{
				int x = event->u.mouse.x;
				event->u.mouse.x = height - event->u.mouse.y;
				event->u.mouse.y = x;
				break;
			}
			default:break;
		}
	}

	return ftk_wnd_manager_queue_event(thiz, event);
}
