
#include "ftk_log.h"
#include "ftk_event.h"
#include "ftk_globals.h"
#include "ftk_input_method_wince.h"
#include <sipapi.h>

typedef struct _PrivInfo
{
	FtkWidget* editor;
	FtkInputType input_type;
}PrivInfo;

static Ret  ftk_input_method_wince_handle_event(FtkInputMethod* thiz, FtkEvent* event);

static Ret  ftk_input_method_wince_init(FtkInputMethod* thiz)
{
	return RET_OK;
}

static Ret  ftk_input_method_wince_reset(FtkInputMethod* thiz)
{
	return RET_OK;
}

static Ret  ftk_input_method_wince_activate(FtkInputMethod* thiz)
{
	ftk_input_method_wince_init(thiz);

	ftk_wnd_manager_add_global_listener(ftk_default_wnd_manager(),
		(FtkListener)ftk_input_method_wince_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_wince_deactivate(FtkInputMethod* thiz)
{
	ftk_input_method_wince_reset(thiz);
	
	ftk_wnd_manager_remove_global_listener(ftk_default_wnd_manager(), 
		(FtkListener)ftk_input_method_wince_handle_event, thiz);

	ftk_logd("%s\n", __func__);

	return RET_OK;
}

static Ret  ftk_input_method_wince_focus_in(FtkInputMethod* thiz, FtkWidget* editor)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && editor != NULL, RET_FAIL);

	ftk_logd("%s:%d ftk_input_method_wince_focus_in\n", __FILE__, __LINE__);

	priv->editor = editor;

	//SipEnumIM();
	//SipSetCurrentIM();

	//MzOpenSip();
	//MzCloseSip();

	SipShowIM(SIPF_ON);
	//SHSipPreference(hWnd, SIP_UP);

	return ftk_input_method_wince_activate(thiz);
}

static Ret  ftk_input_method_wince_focus_out(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	ftk_logd("%s:%d ftk_input_method_wince_focus_out\n", __FILE__, __LINE__);

	SipShowIM(SIPF_OFF);
	//SHSipPreference(hWnd, SIP_FORCEDOWN);
	//SHSipPreference(m_hwnd, SIP_DOWN);

	priv->editor = NULL;
	
	return ftk_input_method_wince_deactivate(thiz);
}

static Ret  ftk_input_method_wince_set_type(FtkInputMethod* thiz, FtkInputType input_type)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, RET_FAIL);

	priv->input_type = input_type;

	return RET_OK;
}

static Ret  ftk_input_method_wince_handle_event(FtkInputMethod* thiz, FtkEvent* event)
{
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL && event != NULL, RET_FAIL);

	switch(event->type)
	{
		case FTK_EVT_OS_IM_COMMIT:
		{
			FtkEvent evt = {0};

			ftk_logd("%s:%d FTK_EVT_OS_IM_COMMIT:%s\n", __FILE__, __LINE__, event->u.extra);

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

static void ftk_input_method_wince_destroy(FtkInputMethod* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_ZFREE(thiz, sizeof(FtkInputMethod) + sizeof(PrivInfo));
	}
}

FtkInputMethod* ftk_input_method_wince_create(void)
{
	FtkInputMethod* thiz = FTK_ZALLOC(sizeof(FtkInputMethod) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		thiz->ref  = 1;
		thiz->name = "wince ime";
		thiz->set_type     = ftk_input_method_wince_set_type;
		thiz->focus_in     = ftk_input_method_wince_focus_in;
		thiz->focus_out    = ftk_input_method_wince_focus_out;
		thiz->handle_event = ftk_input_method_wince_handle_event;
		thiz->destroy      = ftk_input_method_wince_destroy;
	}

	return thiz;
}

FtkWidget* ftk_input_method_wince_get_editor(FtkInputMethod* thiz)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(priv != NULL, NULL);

	return priv->editor;
}
