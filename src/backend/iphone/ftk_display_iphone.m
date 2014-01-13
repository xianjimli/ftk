
#include "ftk_display_iphone.h"
#include "ftk_iphone_view.h"

typedef struct _PrivInfo
{
	int screen_width;
	int screen_height;
	int width;
	int height;
	unsigned char* bits;
} PrivInfo;

static Ret ftk_display_iphone_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);

	ret = ftk_bitmap_copy_to_data_bgra32(bitmap, rect, priv->bits, xoffset, yoffset, priv->width, priv->height);

	ftk_iphone_view_draw(bitmap, rect, priv->bits, xoffset, yoffset, priv->width, priv->height);

	return ret;
}

static int ftk_display_iphone_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->screen_width;
}


static int ftk_display_iphone_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->screen_height;
}


static Ret ftk_display_iphone_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = ftk_display_width(thiz);
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);

	rect.x = r->x;
	rect.y = r->y;
	rect.width = FTK_MIN(bw, r->width);
	rect.height = FTK_MIN(bh, r->height);

	return ftk_bitmap_copy_from_data_bgra32(bitmap, priv->bits, w, h, &rect);
}

static void ftk_display_iphone_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->bits);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}
}

FtkDisplay* ftk_display_iphone_create(void)
{
	FtkDisplay* thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_iphone_update;
		thiz->snap = ftk_display_iphone_snap;
		thiz->width = ftk_display_iphone_width;
		thiz->height = ftk_display_iphone_height;
		thiz->destroy = ftk_display_iphone_destroy;

		CGRect rect = [[UIScreen mainScreen] bounds];

		priv->screen_width = rect.size.width;
		priv->screen_height = rect.size.height;
		priv->width = 512;
		priv->height = 512;
		priv->bits = FTK_ZALLOC(priv->width * priv->height * 4);
	}

	return thiz;
}

void ftk_display_iphone_show_keyboard(FtkDisplay* thiz)
{
	ftk_iphone_view_show_keyboard();
}

void ftk_display_iphone_hide_keyboard(FtkDisplay* thiz)
{
	ftk_iphone_view_hide_keyboard();
}
