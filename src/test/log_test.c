#include "ftk_log.h"
#include "ftk_globals.h"

int main(int argc, char* argv[])
{
	ftk_set_log_level(FTK_LOG_V);
	printf("log_level=FTK_LOG_E\n");
	ftk_logv("logv\n");	
	ftk_logd("logd\n");	
	ftk_loge("loge\n");	
	
	ftk_set_log_level(FTK_LOG_D);
	printf("log_level=FTK_LOG_E\n");
	ftk_logv("logv\n");	
	ftk_logd("logd\n");	
	ftk_loge("loge\n");	
	
	ftk_set_log_level(FTK_LOG_E);
	printf("log_level=FTK_LOG_E\n");
	ftk_logv("logv\n");	
	ftk_logd("logd\n");	
	ftk_loge("loge\n");	
	
	return 0;
}
