#include "ftk_globals.h"
#include "ftk_config.h"
#include "ftk_allocator_default.h"

const char* args[] = {"./config_test", "--disable-statusbar", "--enable-cursor", "--theme=abc", "--data-dir=/data", "--test-data-dir=/testdata", NULL};
const char* xml="<statusbar enable=\"0\" />\n<cursor enable=\"1\" />\n <theme name=\"abc\" /><data_dir value=\"/data\" /> <test_data_dir value=\"/testdata\" />";

int main(int argc, char* argv[])
{
	FtkConfig* thiz = NULL;
	ftk_set_allocator(ftk_allocator_default_create());

	thiz = ftk_config_create();
	assert(ftk_config_init(thiz, (sizeof(args)/sizeof(args[0]))-1, (char **)args) == RET_OK);

	assert(strcmp(ftk_config_get_theme(thiz), "abc") == 0);
	assert(strcmp(ftk_config_get_data_dir(thiz), "/data") == 0);
	assert(strcmp(ftk_config_get_test_data_dir(thiz), "/testdata") == 0);
	assert(ftk_config_get_enable_cursor(thiz));
	assert(!ftk_config_get_enable_status_bar(thiz));

	assert(ftk_config_set_theme(thiz, "") == RET_OK);
	assert(ftk_config_set_data_dir(thiz, "") == RET_OK);
	assert(ftk_config_set_test_data_dir(thiz, "") == RET_OK);
	assert(ftk_config_set_enable_cursor(thiz, 0) == RET_OK);
	assert(ftk_config_set_enable_status_bar(thiz, 0) == RET_OK);
	assert(ftk_config_parse(thiz, xml, strlen(xml)) == RET_OK);
	
	assert(strcmp(ftk_config_get_theme(thiz), "abc") == 0);
	assert(strcmp(ftk_config_get_data_dir(thiz), "/data") == 0);
	assert(strcmp(ftk_config_get_test_data_dir(thiz), "/testdata") == 0);
	assert(ftk_config_get_enable_cursor(thiz));
	assert(!ftk_config_get_enable_status_bar(thiz));

	ftk_config_destroy(thiz);

	return 0;
}

