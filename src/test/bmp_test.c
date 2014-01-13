#include "ftk.h"
#include "ftk_image_bmp_decoder.h"

int main(int argc, char* argv[])
{
	FtkBitmap* bitmap = NULL;
	FtkImageDecoder* decoder = NULL;
	
	ftk_init(argc, argv);

	ftk_bitmap_autotest();
	decoder = ftk_image_bmp_decoder_create();
	bitmap = ftk_image_decoder_decode(decoder, argv[1]);
	
	if(bitmap != NULL)
	{
		printf("%s: width=%d height=%d\n", argv[1], 
			ftk_bitmap_width(bitmap), ftk_bitmap_height(bitmap));
		ftk_bitmap_unref(bitmap);
	}
	ftk_image_decoder_destroy(decoder);

	return 0;
}
