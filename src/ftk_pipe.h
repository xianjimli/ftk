/*
 * File: ftk_pipe.h    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   pipe
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
 * 2010-01-20 Li XianJing <xianjimli@hotmail.com> created
 * 2010-10-02 Jiao JinXing <jiaojinxing1987@gmail.com> add ftk_pipe_check.
 *
 */

#ifndef FTK_PIPE_H
#define FTK_PIEP_H

#include "ftk_typedef.h"

FTK_BEGIN_DECLS

struct _FtkPipe;
typedef struct _FtkPipe FtkPipe;

FtkPipe* ftk_pipe_create(void);
int   ftk_pipe_read(FtkPipe* thiz, void* buff, size_t length);
int   ftk_pipe_write(FtkPipe* thiz, const void* buff, size_t length);
int   ftk_pipe_get_read_handle(FtkPipe* thiz);
int   ftk_pipe_get_write_handle(FtkPipe* thiz);
void  ftk_pipe_destroy(FtkPipe* thiz);
int   ftk_pipe_check(FtkPipe* thiz);

FTK_END_DECLS

#endif/*FTK_MMAP_H*/

