#include "ftk_app.h"

typedef struct _PrivInfo
{
	FtkBitmap* icon;
}PrivInfo;

static FtkBitmap* ftk_app_dummy_get_icon(FtkApp* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, NULL);

	return priv->icon;
}

static const char* ftk_app_dummy_get_name(FtkApp* thiz)
{
	return NULL;
}

static Ret ftk_app_dummy_run(FtkApp* thiz, int argc, char* argv[])
{
	return RET_OK;
}

static void ftk_app_dummy_destroy(FtkApp* thiz)
{
	FTK_FREE(thiz);

	return;
}

FtkApp* ftk_app_dummy_create(void)
{
	FtkApp* thiz = FTK_ZALLOC(sizeof(FtkApp) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->get_icon = ftk_app_dummy_get_icon;
		thiz->get_name = ftk_app_dummy_get_name;
		thiz->run = ftk_app_dummy_run;
		thiz->destroy = ftk_app_dummy_destroy;
	}

	return thiz;
}
