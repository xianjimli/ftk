
#include "ftk_jni.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_android.h"
#include <GLES/gl.h>
#include <GLES/glext.h>

typedef struct _PrivInfo
{
	FtkBitmap* bitmap;
}PrivInfo;

static int screen_width = 320;
static int screen_height = 480;

void Android_SetScreenResolution(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

static Ret  ftk_display_android_handle_event(FtkDisplay* thiz, FtkEvent* event)
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

static int display_bitmap(PrivInfo* priv, FtkBitmap* bitmap, int x, int y, int width, int height, int xoffset, int yoffset)
{
	GLint crop[4] = {0};
	crop[0] = x;
	crop[1] = y;
	crop[2] = width;
	crop[3] = height;

	if(Android_PreRender() != RET_OK)
	{
		return 0;
	}

	glColor4f(1,1,1,1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap), 
		0, GL_RGBA, GL_UNSIGNED_BYTE, ftk_bitmap_lock(bitmap));
	glDrawTexiOES(xoffset, yoffset, 0, width, height);

	Android_Render();

	return 0;
}

static Ret ftk_display_android_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	int i = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	int width = rect->width;
	int height = rect->height;
	int src_width = ftk_bitmap_width(bitmap);
	int src_height = ftk_bitmap_height(bitmap);
	int dst_width = ftk_bitmap_width(priv->bitmap);
	int dst_height = ftk_bitmap_height(priv->bitmap);
	FtkColor* src = ftk_bitmap_lock(bitmap);
	FtkColor* dst = ftk_bitmap_lock(priv->bitmap);

	return_val_if_fail(rect->x < src_width && rect->y < src_height
		&& xoffset < dst_width && yoffset < dst_height, RET_FAIL);

	width = (xoffset + width) >= dst_width ? dst_width - xoffset : width;
	height = (yoffset + height) >= dst_height ? dst_height - yoffset : height;

	yoffset = dst_height  - yoffset - height - 1;
	src += rect->y * src_width + rect->x;
	dst += (yoffset + height) * dst_width + xoffset;

	for(i = 0; i < height; i++)
	{
		memcpy(dst, src, sizeof(FtkColor) * width);
		dst -= dst_width;
		src += src_width;
	}

#if 0
	display_bitmap(priv, priv->bitmap, xoffset, yoffset, width, height, xoffset, yoffset);
#else
	display_bitmap(priv, priv->bitmap, 0, 0, screen_width, screen_height, 0, 0);
#endif

	return ret;
}

static Ret ftk_display_android_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	int i = 0;
	int xoffset = r->x;
	int yoffset = r->y;
	int width = r->width;
	int height = r->height;
	DECL_PRIV(thiz, priv);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	FtkColor* src = ftk_bitmap_lock(priv->bitmap);

	/*TODO*/
	//glReadPixels(r->x, r->y, r->width, r->height, GL_RGBA, GL_UNSIGNED_BYTE, ftk_bitmap_lock(bitmap));

	return RET_OK;
}

static int ftk_display_android_width(FtkDisplay* thiz)
{
	return screen_width;
}

static int ftk_display_android_height(FtkDisplay* thiz)
{
	return screen_height;
}

static void ftk_display_android_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(),
			(FtkListener)ftk_display_android_handle_event, thiz);

		ftk_bitmap_unref(priv->bitmap);
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
		FtkColor bg;
		DECL_PRIV(thiz, priv);
		thiz->update   = ftk_display_android_update;
		thiz->width    = ftk_display_android_width;
		thiz->height   = ftk_display_android_height;
		thiz->snap     = ftk_display_android_snap;
		thiz->destroy  = ftk_display_android_destroy;

		bg.a = 0xff;
		bg.r = 0xff;
		bg.g = 0xff;
		bg.b = 0xff;
		priv->bitmap = ftk_bitmap_create(screen_width, screen_height, bg);

		ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
			(FtkListener)ftk_display_android_handle_event, thiz);

		Android_InitEGL();
	}

	return thiz;
}
