/*
   (c) Copyright 2001-2008  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH
   (c) Copyright 2011 Li XianJing 

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>
#include <sys/utsname.h>

#include "ftk_log.h"
#include "ftk_source_ps2mouse.h"

#define s8 char

typedef enum 
{
	false,
	true
}bool;

#define PS2_SET_RES		 0xE8	/* Set resolution	 */
#define PS2_SET_SCALE11  0xE6	 /* Set 1:1 scaling    */
#define PS2_SET_SAMPLE	 0xF3	 /* Set sample rate    */
#define PS2_ENABLE_DEV	 0xF4	 /* Enable aux device  */
#define PS2_ACK		     0xFA    /* Command byte ACK	 */

#define PS2_SEND_ID		0xF2
#define PS2_Iprintf    -1
#define PS2_ID_PS2		0
#define PS2_ID_IMPS2	3

typedef struct _PrivInfo
{
	int	fd;
	int x;
	int y;
	int	dx;
	int	dy;
	int max_x;
	int max_y;
	int	mouseId;
	int	packetLength;
	FtkEvent event;
	FtkOnEvent on_event;
	void* user_data;
	char filename[64];

	int pressed;
	int last_x;
	int last_y;
	int is_first_move;
} PrivInfo;

static void flush_event(PrivInfo *priv, int button, int press)
{
	if (priv->dx) 
	{
		priv->x += priv->dx;
	}

	if (priv->dy) 
	{
		priv->y += priv->dy;
	}
	
	priv->dy = 0;
	priv->dx = 0;
	if(priv->x < 0) priv->x = 0;
	if(priv->y < 0) priv->y = 0;
	if(priv->x > priv->max_x) priv->x = priv->max_x;
	if(priv->y > priv->max_y) priv->y = priv->max_y;

	if(button < 0)
	{
		if(!priv->is_first_move)
		{
			if(priv->last_x == priv->x && priv->last_y == priv->y)
			{
				/*skip duplicated move*/
				return;
			}

			priv->last_x = priv->x;
			priv->last_y = priv->y;
		}

		priv->is_first_move = false;
	
		priv->event.type = FTK_EVT_MOUSE_MOVE;
	}
	else
	{
		if(priv->pressed == press)
		{
			return;
		}

		priv->pressed = press;
		priv->event.type = press ? FTK_EVT_MOUSE_DOWN : FTK_EVT_MOUSE_UP;
	}
	
	priv->event.u.mouse.x = priv->x;
	priv->event.u.mouse.y = priv->y;
	priv->on_event(priv->user_data, &priv->event);
	priv->event.type = FTK_EVT_NOP;

	return;
}

void dispatch_event(PrivInfo* priv)
{
	int readlen = 0;
	unsigned char pos = 0;
	unsigned char packet[4] = {0};
	unsigned char last_buttons = 0;
	unsigned char buf[256] = {0};

	if( (readlen = read(priv->fd, buf, 256)) > 0 ) 
	{
		int i = 0;
		for ( i = 0; i < readlen; i++ ) 
		{
			if ( pos == 0  &&  (buf[i] & 0xc0) ) 
			{
				continue;
			}
			packet[pos++] = buf[i];
			if ( pos == priv->packetLength ) 
			{
				int dx, dy, dz;
				int buttons;

				pos = 0;
				if ( !(packet[0] & 0x08) ) 
				{
					/* We've lost sync! */
					i--;	/* does this make sense? oh well,
							 it will resync eventually (will it ?)*/
					continue;
				}

				buttons = packet[0] & 0x07;
				dx = (packet[0] & 0x10) ?	packet[1]-256  :  packet[1];
				dy = (packet[0] & 0x20) ? -(packet[2]-256) : -packet[2];
				if (priv->mouseId == PS2_ID_IMPS2) 
				{
					/* Just strip off the extra buttons if present
					   and sign extend the 4 bit value */
					dz = (s8)((packet[3] & 0x80) ?
							   packet[3] | 0xf0 : packet[3] & 0x0F);
					if (dz) {
						flush_event( priv , -1, -1);
					}
				}
				else 
				{
					dz = 0;
				}

				priv->dx += dx;
				priv->dy += dy;

				flush_event( priv, -1, -1);

				if ( last_buttons != buttons ) {
					unsigned char changed_buttons;

					changed_buttons = last_buttons ^ buttons;

					/* make sure the compressed motion event is dispatched
					   before any button change */
					flush_event(priv, -1, -1);
					
					if ( changed_buttons & 0x01 ) {
						flush_event(priv, 1, buttons & 0x01);
					}
					
					if ( changed_buttons & 0x02 ) {
						flush_event(priv, 2, buttons & 0x02);
					}
					
					if ( changed_buttons & 0x04 ) {
						flush_event(priv, 3, buttons & 0x04);
					}

					//FIXME
					flush_event(priv, 1, 0);

					last_buttons = buttons;
				}
			}
		}
		flush_event( priv , -1, -1);
	}

	return ;
}


static int
ps2WriteChar( int fd, unsigned char c, bool verbose )
{
	struct timeval tv;
	fd_set fds;

	tv.tv_sec = 0;
	tv.tv_usec = 200000;	  /*  timeout 200 ms  */

	FD_ZERO( &fds );
	FD_SET( fd, &fds );

	write( fd, &c, 1 );

	if ( select(fd+1, &fds, NULL, NULL, &tv) == 0 ) {
		return -1;
	}

	read( fd, &c, 1 );

	if ( c != PS2_ACK )
		return -2;

	return 0;
}


static int
ps2GetId( int fd, bool verbose )
{
	unsigned char c;

	if ( ps2WriteChar(fd, PS2_SEND_ID, verbose) < 0 )
		return PS2_Iprintf;

	read( fd, &c, 1 );

	return( c );
}


static int
ps2Write( int fd, const unsigned char *priv, size_t len, bool verbose)
{
	size_t i;
	int    error = 0;

	for ( i = 0; i < len; i++ ) {
		if ( ps2WriteChar(fd, priv[i], verbose) < 0 ) {
			if ( verbose )
			error++;
		}
	}

	if ( error && verbose )
		printf( "DirectFB/PS2Mouse: missed %i ack's!\n", error);

	return( error );
}


static int
init_ps2( int fd, bool verbose )
{
	static const unsigned char basic_init[] =
	{ PS2_ENABLE_DEV, PS2_SET_SAMPLE, 100 };
	static const unsigned char imps2_init[] =
	{ PS2_SET_SAMPLE, 200, PS2_SET_SAMPLE, 100, PS2_SET_SAMPLE, 80 };
	static const unsigned char ps2_init[] =
	{ PS2_SET_SCALE11, PS2_ENABLE_DEV, PS2_SET_SAMPLE, 100, PS2_SET_RES, 3 };
	int mouseId;

	struct timeval tv;
	fd_set fds;
	int count = 100;

	/* read all data from the file descriptor before initializing the mouse */
	while (true) {
		unsigned char buf[64];

		tv.tv_sec  = 0;
		tv.tv_usec = 50000;		 /*  timeout 1/50 sec  */

		FD_ZERO( &fds );
		FD_SET( fd, &fds );

		if (select( fd+1, &fds, NULL, NULL, &tv ))
			read( fd, buf, sizeof(buf) );
		else
			break;

		if (! --count) {
			printf( "DirectFB/PS2Mouse: "
					"PS/2 mouse keeps sending data, "
					"initialization failed\n" );
			return -1;
		}
	}

	ps2Write( fd, basic_init, sizeof (basic_init), verbose );
	/* Do a basic init in case the mouse is confused */
	if (ps2Write( fd, basic_init, sizeof (basic_init), verbose ) != 0) {
		if (verbose)
			printf( "DirectFB/PS2Mouse: PS/2 mouse failed init\n" );
		return -1;
	}

	ps2Write( fd, ps2_init, sizeof (ps2_init), verbose );

	if (ps2Write(fd, imps2_init, sizeof (imps2_init), verbose) != 0) {
		if (verbose)
			printf ("DirectFB/PS2Mouse: mouse failed IMPS/2 init\n");
		return -2;
	}

	if ((mouseId = ps2GetId( fd, verbose )) < 0)
		return mouseId;

	if ( mouseId != PS2_ID_IMPS2 )	 /*  unknown id, assume PS/2  */
		mouseId = PS2_ID_PS2;

	return mouseId;
}

/**************************************************************************************************/

static Ret open_device(PrivInfo* priv, int max_x, int max_y)
{
	int			fd = 0;
	int			mouseId = -1;
	int			flags = O_RDWR | O_SYNC | O_EXCL;;

	fd = open( "/dev/input/mice", flags );
	if (fd < 0) 
	{
		return RET_OK;
	}

	fcntl( fd, F_SETFL, fcntl ( fd, F_GETFL ) & ~O_NONBLOCK );
	mouseId = init_ps2( fd, true );
		
	if (mouseId  < 0) {
		close( fd );
		return RET_OK;
	}

	priv->max_x = max_x;
	priv->max_y = max_y;
	priv->fd		   = fd;
	priv->mouseId	   = mouseId;
	priv->packetLength = (mouseId == PS2_ID_IMPS2) ? 4 : 3;

	return RET_OK;
}

static int ftk_source_ps2mouse_get_fd(FtkSource* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fd;
}

static int ftk_source_ps2mouse_check(FtkSource* thiz)
{
	return -1;
}

static Ret ftk_source_ps2mouse_dispatch(FtkSource* thiz)
{
	int ret = 0;
	DECL_PRIV(thiz, priv);

	dispatch_event(priv);

	return RET_OK;
}

static void ftk_source_ps2mouse_destroy(FtkSource* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		close(priv->fd);
		FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
	}

	return;
}

FtkSource* ftk_source_ps2mouse_create(const char* filename, FtkOnEvent on_event, void* user_data, int max_x, int max_y)
{
	FtkSource* thiz = (FtkSource*)FTK_ZALLOC(sizeof(FtkSource) + sizeof(PrivInfo));

	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);

		thiz->get_fd   = ftk_source_ps2mouse_get_fd;
		thiz->check    = ftk_source_ps2mouse_check;
		thiz->dispatch = ftk_source_ps2mouse_dispatch;
		thiz->destroy  = ftk_source_ps2mouse_destroy;

		thiz->ref = 1;
		priv->fd = open(filename, O_RDONLY);
		ftk_strncpy(priv->filename, filename, sizeof(priv->filename));

		open_device(priv, max_x, max_y);
		if(priv->fd < 0)
		{
			FTK_ZFREE(thiz, sizeof(thiz) + sizeof(PrivInfo));
			return NULL;
		}

		priv->on_event = on_event;
		priv->user_data = user_data;
		priv->is_first_move = true;
		ftk_logd("%s: %d=%s priv->user_data=%p\n", __func__, priv->fd, filename, priv->user_data);
	}

	return thiz;
}

