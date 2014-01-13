/*
 * ftk_source_input.c
 *
 *  Created on: 2011-1-1
 *	  Author: Yi.Qiu
 */

#include "ftk_log.h"
#include "ftk_key.h"
#include "ftk_event.h"
#include "ftk_source.h"

typedef struct _PrivInfo
{
	int		 fd;
	FtkEvent event;
	FtkOnEvent  on_event;
	void*	   user_data;
	rt_device_t	device;
} PrivInfo;

static int s_key_map[0xff] = {0};

static void ftk_keymap_init(void)
{
	s_key_map[0x1] = FTK_KEY_ESC;
	s_key_map[0xc] = FTK_KEY_MINUS;
	s_key_map[0x39] = FTK_KEY_SPACE;
	s_key_map[0xd] = FTK_KEY_EQUAL;
	s_key_map[0xe] = FTK_KEY_BACKSPACE;
	s_key_map[0xf] = FTK_KEY_TAB;
	s_key_map[0x1c] = FTK_KEY_ENTER;
	s_key_map[0xb] = FTK_KEY_0;
	s_key_map[0x2] = FTK_KEY_1; 
	s_key_map[0x3] = FTK_KEY_2;
	s_key_map[0x4] = FTK_KEY_3;
	s_key_map[0x5] = FTK_KEY_4;
	s_key_map[0x6] = FTK_KEY_5;
	s_key_map[0x7] = FTK_KEY_6;
	s_key_map[0x8] = FTK_KEY_7;
	s_key_map[0x9] = FTK_KEY_8;
	s_key_map[0xa] = FTK_KEY_9;
	s_key_map[0x3b] = FTK_KEY_F1; 
	s_key_map[0x3c] = FTK_KEY_F2;
	s_key_map[0x3d] = FTK_KEY_F3;
	s_key_map[0x3e] = FTK_KEY_F4;
	s_key_map[0xef] = FTK_KEY_F5;
	s_key_map[0x40] = FTK_KEY_F6;
	s_key_map[0x41] = FTK_KEY_F7;
	s_key_map[0x42] = FTK_KEY_F8;
	s_key_map[0x43] = FTK_KEY_F9;
	s_key_map[0x1e] = FTK_KEY_A;
	s_key_map[0x30] = FTK_KEY_B;
	s_key_map[0x2c] = FTK_KEY_C;
	s_key_map[0x20] = FTK_KEY_D;
	s_key_map[0x12] = FTK_KEY_E;
	s_key_map[0x21] = FTK_KEY_F;
	s_key_map[0x22] = FTK_KEY_G;
	s_key_map[0x23] = FTK_KEY_H;
	s_key_map[0x17] = FTK_KEY_I;
	s_key_map[0x24] = FTK_KEY_J;
	s_key_map[0x25] = FTK_KEY_K;
	s_key_map[0x26] = FTK_KEY_L;
	s_key_map[0x32] = FTK_KEY_M;
	s_key_map[0x31] = FTK_KEY_N;
	s_key_map[0x18] = FTK_KEY_O;
	s_key_map[0x19] = FTK_KEY_P;
	s_key_map[0x10] = FTK_KEY_Q;
	s_key_map[0x13] = FTK_KEY_R;
	s_key_map[0x1f] = FTK_KEY_S;
	s_key_map[0x14] = FTK_KEY_T;
	s_key_map[0x16] = FTK_KEY_U;
	s_key_map[0x2f] = FTK_KEY_V;
	s_key_map[0x11] = FTK_KEY_W;
	s_key_map[0x2d] = FTK_KEY_X;
	s_key_map[0x15] = FTK_KEY_Y;
	s_key_map[0x2c] = FTK_KEY_Z;
	s_key_map[0x4b] = FTK_KEY_LEFT;
	s_key_map[0x4d] = FTK_KEY_RIGHT;
	s_key_map[0x50] = FTK_KEY_DOWN;
	s_key_map[0x2e] = FTK_KEY_DELETE;
	s_key_map[0x48] = FTK_KEY_UP;
}	

static int ftk_source_input_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fd;
}

static int ftk_source_input_check(FtkSource* thiz)
{
	return -1;
}

static Ret ftk_source_input_dispatch(FtkSource* thiz)
{
	char ch;
	int ret = 0;
	DECL_PRIV(thiz, priv);

	while(rt_device_read(priv->device, 0, &ch, 1) == 1)
	{
		priv->event.type = (ch & 0x80) ? FTK_EVT_KEY_UP : FTK_EVT_KEY_DOWN;
		priv->event.u.key.code = s_key_map[ch & 0x7F];
		
		if(priv->on_event != NULL && priv->event.type != FTK_EVT_NOP)
		{
			priv->on_event(priv->user_data, &priv->event);
			priv->event.type = FTK_EVT_NOP;
		}
	}	
	
	return RET_OK;
}

static void ftk_source_input_destroy(FtkSource* thiz)
{
	if (thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		ftk_rtthread_select_fd_free(priv->fd);

		rt_device_close(priv->device);

		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
	}
}

static rt_err_t ftk_source_input_rx_ind(rt_device_t dev, rt_size_t size)
{
	FtkSource* thiz = (FtkSource*)dev->user_data;
	DECL_PRIV(thiz, priv);
	
	ftk_rtthread_set_file_readble(priv->fd);
	
	return RET_OK;
}

FtkSource* ftk_source_input_create(const char* filename, FtkOnEvent on_event, void* user_data)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));
	rt_device_t device = NULL;

	if (thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->get_fd   = ftk_source_input_get_fd;
		thiz->check	= ftk_source_input_check;
		thiz->dispatch = ftk_source_input_dispatch;
		thiz->destroy  = ftk_source_input_destroy;

		thiz->ref = 1;

		priv->device = rt_device_find(filename); 
		if (priv->device == NULL)
		{
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}
		if (rt_device_open(priv->device, RT_DEVICE_OFLAG_RDONLY) != RT_EOK)
		{
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}

		priv->device->user_data = (void *)thiz;
		rt_device_set_rx_indicate(priv->device, ftk_source_input_rx_ind);

		priv->fd = ftk_rtthread_select_fd_alloc();

		priv->on_event  = on_event;
		priv->user_data = user_data;

		ftk_keymap_init();
		ftk_logd("%s: %d=%s priv->user_data=%p\n", __func__, priv->fd, filename, priv->user_data);
	}
	
	return thiz;
}

