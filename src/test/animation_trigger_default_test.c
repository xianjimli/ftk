#include "ftk.h"
#include "ftk_animation_trigger_default.h"


int main(int argc, char* argv[])
{
	FtkConfig* config = NULL;
	FtkAnimationTrigger* thiz = NULL;
	ftk_set_allocator(ftk_allocator_default_create());

	config = ftk_config_create();
	ftk_set_config(config);
	ftk_config_init(config, argc, argv);
	thiz = ftk_animation_trigger_default_create("default", "animations.xml");

	ftk_animation_trigger_default_dump(thiz);
	ftk_animation_trigger_destroy(thiz);

	return 0;
}

