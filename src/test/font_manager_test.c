#include "ftk.h"

int main(int argc, char* argv[])
{
	FtkFont* font = NULL;
	FtkFontDesc* font_desc = NULL;
	FtkFontManager* font_manager = NULL;

	ftk_init(argc, argv);

	font_desc = ftk_font_desc_create("size:24 bold:0 italic:0");
	font_manager = ftk_font_manager_create(16);

	font = ftk_font_manager_get_default_font(font_manager);
	assert(font != NULL);
	assert(ftk_font_height(font) == 16);
	
	font = ftk_font_manager_load(font_manager, font_desc);
	assert(font != NULL);
	assert(ftk_font_height(font) == 24);

	ftk_font_desc_unref(font_desc);
	ftk_font_manager_destroy(font_manager);

	return 0;
}
