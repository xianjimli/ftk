#include "ftk.h"
#include "ftk_bitmap.h"
#include "ftk_bitmap_factory.h"
#include "ftk_allocator_default.h"

int main(int argc, char* argv[])
{
	FtkColor c = {0};
	FtkBitmap* bitmap = NULL;
	FtkBitmapFactory* f = NULL;
	ftk_set_allocator(ftk_allocator_default_create());
	
	if(argv[1] != NULL)
	{
		f = ftk_bitmap_factory_create();
		bitmap = ftk_bitmap_factory_load(f, argv[1]);
		c = ftk_bitmap_get_pixel(bitmap, 0, 0);
		ftk_logi("r=%02x g=%02x b=%02x a=%02x\n", c.r, c.g, c.b, c.a);
		ftk_bitmap_unref(bitmap);
		ftk_bitmap_factory_destroy(f);
	}

	ftk_bitmap_auto_test();

	return 0;
}

