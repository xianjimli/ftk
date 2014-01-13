#include "ftk.h"
#include "ftk_theme.h"

const char* xml = "\
<theme name=\"android\" >\
<entry bg[normal]=\"ff00ffff\" \
	bg[focused]=\"ff01ffff\" \
	bg[active]=\"ff02ffff\" \
	bg[disable]=\"ff03a899\" \
	bg_image[normal]=\"Acrobat.png\" \
	bg_image[focused]=\"Chat.png\" \
	bg_image[active]=\"Flickr.png\" \
	bg_image[disable]=\"Flickr.png\"\
	fg_image[normal]=\"fAcrobat.png\" \
	fg_image[focused]=\"fChat.png\" \
	fg_image[active]=\"fFlickr.png\" \
	fg_image[disable]=\"fFlickr.png\"\
	fg[normal]=\"ff000000\" \
	fg[focused]=\"ff010000\" \
	fg[active]=\"ff020000\" \
	fg[disable]=\"ff030000\" \
	bd[normal]=\"ff000000\" \
	bd[focused]=\"ff010000\"\
	bd[active]=\"ff020000\" \
	bd[disable]=\"ff030000\" />\
</theme>";

int main(int argc, char* argv[])
{
	FtkColor c = {0};
	FtkTheme* thiz = NULL;
	FtkWidgetState state = FTK_WIDGET_NORMAL;
	FtkWidgetType type = FTK_ENTRY;
	
	ftk_set_allocator(ftk_allocator_default_create());
	thiz = ftk_theme_create(0);
	assert(ftk_theme_parse_data(thiz, xml, strlen(xml)) == RET_OK);

	for(; state < FTK_WIDGET_STATE_NR; state++)
	{
		c = ftk_theme_get_bg_color(thiz, type, state);
		assert(c.a == 0xff && c.r == state);
		c = ftk_theme_get_fg_color(thiz, type, state);
		assert(c.a == 0xff && c.r == state);
		c = ftk_theme_get_border_color(thiz, type, state);
		assert(c.a == 0xff && c.r == state);
	}

	ftk_theme_destroy(thiz);

	return 0;
}
