/*
* File: ucos_mutex.h
* Author:  MinPengli <minpengli@gmail.com>
* Brief: mutex implement
*
* Copyright (c) 2009 - 2010  MinPengli <minpengli@hotmail.com>
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
*  History:
* ================================================================
* 2010-03-19 MinPengli <minpengli@gmail.com> created
*
*/

#ifndef UCOS_MUTEX_H
#define UCOS_MUTEX_H

#include    <ucos_ii.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _mutex_t;
typedef struct _mutex_t mutex_t;

mutex_t *mutex_init (void);
int mutex_destroy (mutex_t *mutex);
int mutex_lock (mutex_t *mutex);
int mutex_trylock (mutex_t *mutex);
int mutex_unlock (mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif
