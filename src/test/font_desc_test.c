#include "ftk.h"

int main(int argc, char* argv[])
{
	FtkFontDesc* font_desc1 = NULL;
	FtkFontDesc* font_desc2 = NULL;
	ftk_set_allocator(ftk_allocator_default_create());

	font_desc1 = ftk_font_desc_create("size:16 bold:0 italic:0");
	font_desc2 = ftk_font_desc_create("size:24 bold:1 italic:1");

	assert(ftk_font_desc_get_size(font_desc1) == 16);
	assert(ftk_font_desc_get_size(font_desc2) == 24);

	assert(ftk_font_desc_is_bold(font_desc1) == 0);
	assert(ftk_font_desc_is_bold(font_desc2) == 1);

	assert(ftk_font_desc_is_italic(font_desc1) == 0);
	assert(ftk_font_desc_is_italic(font_desc2) == 1);
	
	assert(ftk_font_desc_is_equal(font_desc1, font_desc2) == 0);

	ftk_font_desc_unref(font_desc1);
	ftk_font_desc_unref(font_desc2);

	return 0;
}
