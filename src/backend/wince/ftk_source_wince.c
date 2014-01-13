
#include "ftk_source_wince.h"

typedef struct _PrivInfo
{
	void* user_data;
}PrivInfo;

static int ftk_source_wince_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_wince_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_wince_dispatch(FtkSource* thiz)
{
	MSG msg;

	while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	Sleep(20);

	return RET_OK;
}

static void ftk_source_wince_destroy(FtkSource* thiz)
{
	FTK_ZFREE(thiz, sizeof(FtkSource) + sizeof(PrivInfo));
}

FtkSource* ftk_source_wince_create(void)
{
	FtkSource* thiz = NULL;

	thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->get_fd   = ftk_source_wince_get_fd;
		thiz->check    = ftk_source_wince_check;
		thiz->dispatch = ftk_source_wince_dispatch;
		thiz->destroy  = ftk_source_wince_destroy;

		thiz->ref = 1;
	}

	return thiz;
}
