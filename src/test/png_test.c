#include "ftk.h"
#include "ftk_image_png_decoder.h"

int main(int argc, char* argv[])
{
	FtkBitmap* bitmap = NULL;
	const char* filename = argv[1];
	ftk_set_allocator(ftk_allocator_default_create());
	FtkImageDecoder* thiz = ftk_image_png_decoder_create();
	
	assert(ftk_image_decoder_match(thiz, filename) == RET_OK);
	assert((bitmap = ftk_image_decoder_decode(thiz, filename)) != NULL);
	printf("w=%d h=%d\n", ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap));
	ftk_bitmap_unref(bitmap);
	ftk_image_decoder_destroy(thiz);

	return 0;
}
