
#include "ftk_typedef.h"
#include "ftk.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_display_rotate.h"
#include "ftk_display_sdl.h"
#include <SDL.h>

#ifdef WEBOS
#include <PDL.h>
#endif

typedef struct _PrivInfo
{
	SDL_Surface* screen;
	SDL_Surface* surface;
	int width;
	int height;
	FtkEvent event;
}PrivInfo;

static Ret ftk_display_sdl_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	SDL_Rect paint_rect;
	DECL_PRIV(thiz, priv);

#if 1
	SDL_LockSurface(priv->surface);
	ftk_bitmap_copy_to_data_rgba32(bitmap, rect, priv->surface->pixels, xoffset, yoffset, priv->width, priv->height);
	SDL_UnlockSurface(priv->surface);

#if 0
	paint_rect.x = xoffset;
	paint_rect.y = xoffset;
	paint_rect.w = (xoffset + rect->width) >= priv->width ? priv->width - xoffset : rect->width;
	paint_rect.h = (yoffset + rect->height) >= priv->height ? priv->height - yoffset : rect->height;
#else
	paint_rect.x = 0;
	paint_rect.y = 0;
	paint_rect.w = priv->width;
	paint_rect.h = priv->height;
#endif

	SDL_BlitSurface(priv->surface, &paint_rect, priv->screen, &paint_rect);

	if((priv->screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF)
	{
		SDL_Flip(priv->screen);
	}
	else
	{
		SDL_UpdateRects(priv->screen, 1, &paint_rect);
	}
#else
	SDL_GL_Lock();
	ftk_bitmap_copy_to_data_rgba32(bitmap, rect, priv->surface->pixels, xoffset, yoffset, priv->width, priv->height);
	SDL_GL_Unlock();

	paint_rect.x = 0;
	paint_rect.y = 0;
	paint_rect.w = priv->width;
	paint_rect.h = priv->height;

	SDL_BlitSurface(priv->surface, &paint_rect, priv->screen, &paint_rect);

	if((priv->screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF)
	{
		SDL_GL_SwapBuffers();
	}
	else
	{
		SDL_GL_UpdateRects(1, &paint_rect);
	}
#endif

	return RET_OK;
}

static Ret ftk_display_sdl_snap(FtkDisplay* thiz, FtkRect* r, FtkBitmap* bitmap)
{
#if 0
	int i = 0;
	int xoffset = r->x;
	int yoffset = r->y;
	int width = r->width;
	int height = r->height;
	DECL_PRIV(thiz, priv);
	FtkColor* dst = ftk_bitmap_lock(bitmap);
	FtkColor* src = ftk_bitmap_lock(priv->bitmap);

	/*TODO*/
#endif

	return RET_OK;
}

static int ftk_display_sdl_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->width;
}

static int ftk_display_sdl_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, 0);
	return priv->height;
}

static void ftk_display_sdl_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		SDL_FreeSurface(priv->surface);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));

#ifdef WEBOS
		PDL_Quit();
#endif
		SDL_Quit();
	}
}

FtkDisplay* ftk_display_sdl_create(void)
{
	SDL_VideoInfo *vi;
	FtkDisplay* thiz;

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->update = ftk_display_sdl_update;
		thiz->width = ftk_display_sdl_width;
		thiz->height = ftk_display_sdl_height;
		thiz->snap = ftk_display_sdl_snap;
		thiz->destroy = ftk_display_sdl_destroy;

		priv->width = 320;
		priv->height = 480;

		ftk_logd("%d-%d\n", priv->width, priv->height);

		SDL_Init(SDL_INIT_VIDEO/*|SDL_INIT_NOPARACHUTE*/);
#ifdef WEBOS
		PDL_Init(0);
#endif

		/* SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2); */

		vi = (SDL_VideoInfo*)SDL_GetVideoInfo();
		if(vi == NULL)
		{
			ftk_loge("SDL_GetVideoInfo() failed\n");
			return NULL;
		}

		ftk_logd("bpp: %d\n", vi->vfmt->BitsPerPixel);

		priv->screen = SDL_SetVideoMode(priv->width, priv->height, vi->vfmt->BitsPerPixel, SDL_HWSURFACE/*SDL_OPENGL*/);
		if(priv->screen == NULL)
		{
			ftk_loge("SDL_SetVideoMode() failed\n");
			/* TODO: cleanup */
			return NULL;
		}

		priv->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, priv->width, priv->height, 32, 0, 0, 0, 0);
		if(priv->surface == NULL)
		{
			ftk_loge("SDL_CreateRGBSurface() failed\n");
			return NULL;
		}
	}

	return thiz;
}
