#include "ftk.h"
#include "ftk_bitmap_factory.h"
#include "ftk_image_jpeg_decoder.h"
#include "ftk_image_png_decoder.h"

int main(int argc, char* argv[])
{
	const char* filename = argv[1];
	ftk_init(argc, argv);

	FtkBitmapFactory* thiz = ftk_bitmap_factory_create();
	FtkDisplay* display = ftk_default_display();

	if(display != NULL)
	{
		FtkRect rect = {0};
		FtkBitmap* bitmap = ftk_bitmap_factory_load(thiz, filename); 
		rect.width = ftk_bitmap_width(bitmap);
		rect.height = ftk_bitmap_height(bitmap);
		ftk_display_update(display, bitmap, &rect, 0, 50);
		printf("width=%d height=%d\n", ftk_display_width(display), ftk_display_height(display));
		ftk_bitmap_unref(bitmap);
	}
	ftk_bitmap_factory_destroy(thiz);
	ftk_run();

	return 0;
}
