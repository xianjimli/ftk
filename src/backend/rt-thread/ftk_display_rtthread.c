
#include <stdlib.h>
#include "ftk_log.h"
#include "ftk_display_mem.h"
#include "ftk_display_rtthread.h"

FtkDisplay* ftk_display_lcd_create(const char* filename)
{
	rt_device_t device;
	rt_err_t result;
	struct rt_device_graphic_info info;
    FtkDisplay* thiz = NULL;
	FtkPixelFormat format = 0;
	
	device = rt_device_find(filename);
	if(device == RT_NULL) return NULL;

	/* get framebuffer address */
	result = rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info);
	if(result != RT_EOK)
	{
		/* get device information failed */
		return NULL;
	}

    if(info.bits_per_pixel == 16)
    {
        format = FTK_PIXEL_RGB565;
    }
    else if(info.bits_per_pixel == 24)
    {
        format = FTK_PIXEL_BGR24;
    }
    else if(info.bits_per_pixel == 32)
    {
        format = FTK_PIXEL_BGRA32;
    }
    else
    {
        assert(!"not supported framebuffer format.");
    }

    thiz = ftk_display_mem_create(format, info.width, info.height,
            info.framebuffer, NULL, NULL);

    return thiz;
}

