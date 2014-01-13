
#import "ftk_source_iphone.h"
#import <UIKit/UIKit.h>

static int ftk_source_iphone_get_fd(FtkSource* thiz)
{
	return -1;
}

static int ftk_source_iphone_check(FtkSource* thiz)
{
	return 0;
}

static Ret ftk_source_iphone_dispatch(FtkSource* thiz)
{
	SInt32 rc;

	if(setjmp(*jump_env()) == 0)
	{
		do
		{
			rc = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
		}
		while(rc == kCFRunLoopRunHandledSource);

		usleep(20000);
	}
}

static void ftk_source_iphone_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		FTK_ZFREE(thiz, sizeof(thiz));
	}
}

FtkSource* ftk_source_iphone_create(void)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource));
	if(thiz != NULL)
	{
		thiz->get_fd = ftk_source_iphone_get_fd;
		thiz->check = ftk_source_iphone_check;
		thiz->dispatch = ftk_source_iphone_dispatch;
		thiz->destroy = ftk_source_iphone_destroy;
		thiz->ref = 1;
	}

	return thiz;
}
