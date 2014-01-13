
#include "ftk_jni.h"
#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_input_method_android.h"

typedef struct _PrivInfo
{
	FtkWidget* editor;
	FtkInputType input_type;
}PrivInfo;

static Ret  ftk_input_method_android_handle_event(FtkInputMethod* thiz, FtkEvent* event);

static Ret  ftk_input_method_android_init(FtkInputMethod* thiz)
{
	return RET_OK;
}

static Ret  ftk_input_method_android_reset(FtkInputMethod* thiz)
{
	return RET_OK;
}

static Ret  ftk_input_method_android_activate(FtkInputMethod* thiz)
{
	ftk_input_method_android_init(thiz);

	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
		(FtkListener)ftk_input_method_android_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_android_deactivate(FtkInputMethod* thiz)
{
	ftk_input_method_android_reset(thiz);
	
	ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
		(FtkListener)ftk_input_method_android_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_android_focus_in(FtkInputMethod* thiz, FtkWidget* editor)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && editor != NULL, RET_FAIL);

	priv->editor = editor;

	Android_ShowKeyboard();

	return ftk_input_method_android_activate(thiz);
}

static Ret  ftk_input_method_android_focus_out(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	Android_HideKeyboard();

	priv->editor = NULL;
	
	return ftk_input_method_android_deactivate(thiz);
}

static Ret  ftk_input_method_android_set_type(FtkInputMethod* thiz, FtkInputType input_type)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->input_type = input_type;

	return RET_OK;
}

static Ret  ftk_input_method_android_handle_event(FtkInputMethod* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_FAIL);

	switch(event->type)
	{
		case FTK_EVT_OS_IM_COMMIT:
		{
			FtkEvent evt = {0};
			if(priv->editor == NULL)
			{
				FTK_FREE(event->u.extra);
				ret = RET_REMOVE;
				break;
			}

			/* TODO: priv->input_type */

			evt.type = FTK_EVT_IM_COMMIT;
			evt.u.extra = event->u.extra;
			evt.widget = priv->editor;
			ftk_widget_event(priv->editor, &evt);

			FTK_FREE(event->u.extra);
			ret = RET_REMOVE;
			break;
		}
		case FTK_EVT_IM_ACT_COMMIT:
		{
			break;
		}
		default:break;
	}

	return ret;
}

static void ftk_input_method_android_destroy(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_ZFREE(thiz, sizeof(FtkInputMethod) + sizeof(PrivInfo));
	}
}

FtkInputMethod* ftk_input_method_android_create(void)
{
	FtkInputMethod* thiz = FTK_ZALLOC(sizeof(FtkInputMethod) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->ref  = 1;
		thiz->name = "android ime";
		thiz->set_type     = ftk_input_method_android_set_type;
		thiz->focus_in     = ftk_input_method_android_focus_in;
		thiz->focus_out    = ftk_input_method_android_focus_out;
		thiz->handle_event = ftk_input_method_android_handle_event;
		thiz->destroy      = ftk_input_method_android_destroy;
	}

	return thiz;
}
