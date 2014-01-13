/*
 * File:    fbus_stream_socket.c 
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   socket implemented stream interface.
 *
 * Copyright (c) 2009 - 2010  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2010-07-25 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "ftk_allocator.h"
#include "fbus_stream_socket.h"

#ifdef WIN32
#define ftk_close_socket closesocket
#else
#define ftk_close_socket close
#endif

typedef struct _PrivInfo
{
	int sock_no;
}PrivInfo;

static int fbus_stream_socket_read(FBusStream* thiz, char* buffer, size_t len)
{
	DECL_PRIV(thiz, priv);

	return recv(priv->sock_no, buffer, len, 0);
}

static int fbus_stream_socket_write(FBusStream* thiz, const char* buffer, size_t len)
{
	DECL_PRIV(thiz, priv);

	return send(priv->sock_no, buffer, len, 0);
}

static int fbus_stream_socket_get_fd(FBusStream* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->sock_no;
}

static void fbus_stream_socket_destroy(FBusStream* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		ftk_close_socket(priv->sock_no);

		FTK_FREE(thiz);
	}

	return;
}

FBusStream* fbus_stream_socket_create(int sock_no)
{
	FBusStream* thiz = NULL;
	return_val_if_fail(sock_no > 0, NULL);

	thiz = FTK_ZALLOC(sizeof(FBusStream) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		int opt_value = 1;
		DECL_PRIV(thiz, priv);

		thiz->read    = fbus_stream_socket_read;
		thiz->write   = fbus_stream_socket_write;
		thiz->get_fd  = fbus_stream_socket_get_fd;
		thiz->destroy = fbus_stream_socket_destroy;

		setsockopt(sock_no, IPPROTO_TCP, TCP_NODELAY, &opt_value, sizeof(opt_value));
		setsockopt(sock_no, SOL_SOCKET, SO_KEEPALIVE, &opt_value, sizeof(opt_value));
		priv->sock_no = sock_no;
	}

	return thiz;
}


FBusStream* fbus_stream_socket_connect(const char* host, int port)
{
	int sock_no = -1;
	struct sockaddr_in addr;
	struct hostent* hostent_ptr = NULL;
	
	return_val_if_fail(host != NULL && port > 0, NULL);

	hostent_ptr = gethostbyname(host);
	return_val_if_fail(hostent_ptr != NULL, NULL);

	addr.sin_family = PF_INET;
	addr.sin_port = htons((unsigned short)port);
	addr.sin_addr.s_addr = *(unsigned long*)(hostent_ptr->h_addr_list[0]);

	sock_no = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sock_no, (struct sockaddr*)&(addr), sizeof(addr)) == 0)
	{
		return fbus_stream_socket_create(sock_no);
	}
	else
	{
		perror("connect");
		return NULL;
	}
}

int fbus_stream_listen(int port)
{
	int opt_value = 1;
	struct sockaddr_in addr_server = {0};
	int sock_listen = socket(PF_INET, SOCK_STREAM, 0);
	return_val_if_fail(sock_listen > 0, -1);

	addr_server.sin_family = PF_INET;
	addr_server.sin_port = htons(port);
	addr_server.sin_addr.s_addr = INADDR_ANY;

	setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value));

	if(bind(sock_listen, (struct sockaddr*)&addr_server, sizeof(addr_server)) < 0)
	{
		perror("bind");
		ftk_close_socket(sock_listen);

		return -1;
	}

	if(listen(sock_listen, 5) < 0)
	{
		perror("listen");
		ftk_close_socket(sock_listen);

		return -1;
	}

	return sock_listen;
}

FBusStream* fbus_socket_accept(int sock_no)
{
	int addrlen = 0;
	int sock_client = 0;
	struct sockaddr_in addr_client = {0};

	memset(&addr_client, 0x00, sizeof(addr_client));
	sock_client = accept(sock_no, (struct sockaddr*)&addr_client, (socklen_t*)&addrlen);
	return_val_if_fail(sock_client > 0, NULL);

	return fbus_stream_socket_create(sock_client);
}

Ret        ftk_socket_close(int sock_no)
{
	return ftk_close_socket(sock_no);
}

