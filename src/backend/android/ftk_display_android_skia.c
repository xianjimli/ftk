
#include "ftk_jni.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_android.h"

typedef struct _PrivInfo
{
	int unused;
}PrivInfo;

static int screen_width = 320;
static int screen_height = 480;

void Android_SetScreenResolution(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

static Ret  ftk_display_android_skia_handle_event(FtkDisplay* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_FAIL);

	switch(event->type)
	{
		case FTK_EVT_OS_SCREEN_ROTATED:
		{
			ftk_wnd_manager_set_rotate(ftk_default_wnd_manager(), (FtkRotate)event->u.extra);
			ret = RET_REMOVE;
			break;
		}
		default:break;
	}

	return ret;
}

static Ret ftk_display_android_skia_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);

	int width = rect->width;
	int height = rect->height;
	int src_width = ftk_bitmap_width(bitmap);
	int src_height = ftk_bitmap_height(bitmap);
	FtkColor* src = ftk_bitmap_lock(bitmap);

	int dst_width = screen_width;
	int dst_height = screen_height;
	int* pixels = Android_GetBitmapPixels();
	FtkColor* dst = (FtkColor*)pixels;

	return_val_if_fail(rect->x < src_width && rect->y < src_height
		&& xoffset < dst_width && yoffset < dst_height, RET_FAIL);

	width = (xoffset + width) >= dst_width ? dst_width - xoffset : width;
	height = (yoffset + height) >= dst_height ? dst_height - yoffset : height;

	src += rect->y * src_width + rect->x;
	dst += yoffset * dst_width + xoffset;

	for(i = 0; i < height; i++)
	{
		memcpy(dst, src, sizeof(FtkColor) * width);
		dst += dst_width;
		src += src_width;
	}

	Android_ReleaseBitmapPixels(pixels);

#if 0
	Android_DrawBitmap(yoffset * screen_width + xoffset, screen_width, xoffset, yoffset, width, height);
#else
	Android_DrawBitmap(0, screen_width, 0, 0, screen_width, screen_height);
#endif

	return ret;
}

static Ret ftk_display_android_skia_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	int i = 0;
	int xoffset = r->x;
	int yoffset = r->y;
	int width = r->width;
	int height = r->height;
	DECL_PRIV(thiz, priv);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	//FtkColor* src = ftk_bitmap_lock(priv->bitmap);

	/*TODO*/

	return RET_OK;
}

static int ftk_display_android_skia_width(FtkDisplay* thiz)
{
	return screen_width;
}

static int ftk_display_android_skia_height(FtkDisplay* thiz)
{
	return screen_height;
}

static void ftk_display_android_skia_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(),
			(FtkListener)ftk_display_android_skia_handle_event, thiz);

		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

FtkDisplay* ftk_display_android_create(void)
{
	FtkDisplay* thiz = NULL;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		thiz->update   = ftk_display_android_skia_update;
		thiz->width    = ftk_display_android_skia_width;
		thiz->height   = ftk_display_android_skia_height;
		thiz->snap     = ftk_display_android_skia_snap;
		thiz->destroy  = ftk_display_android_skia_destroy;

		ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
			(FtkListener)ftk_display_android_skia_handle_event, thiz);
	}

	return thiz;
}
