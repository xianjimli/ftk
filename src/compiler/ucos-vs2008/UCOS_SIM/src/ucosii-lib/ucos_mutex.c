
/*
* File: ucos_mutex.c    
* Author:  MinPengli <MinPengli@gmail.com>
* Brief: mutex implement
*
* Copyright (c) 2009 - 2010  MinPengli <minpengli@gmail.com>
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
* 2010-03-19 MinPengli <MinPengli@gmail.com> created
*
*/

#include <includes.h>
#include <ucos_mutex.h>

struct _mutex_t
{
	OS_EVENT *mtx;
};

mutex_t *mutex_init (void)
{
	mutex_t *mutex = NULL;
	OS_EVENT *local_mutex = NULL;

	mutex = malloc(sizeof(mutex_t));
	if(mutex == NULL)
	{
		return NULL;
	}

	do{
		local_mutex = OSSemCreate(1);
	}while(!local_mutex);

	mutex->mtx = local_mutex;
	
    return mutex;
}

int mutex_destroy (mutex_t *mutex)
{
    INT8U err = 0;

	if(mutex == NULL)
		return -1;

	OSSemDel(mutex->mtx, OS_DEL_ALWAYS, &err);
	free(mutex);

    return 0;
}

int mutex_lock (mutex_t *mutex)
{
    INT8U err = 0;

	if(mutex == NULL)
		return -1;

	OSSemPend(mutex->mtx, 0, &err);
	    
    return 0;
}

int mutex_trylock (mutex_t *mutex)
{
    INT8U err = 0;

	if(mutex == NULL)
	{
		return -1;
	}
    
	OSSemPend(mutex->mtx, 500, &err);
	if(err == OS_TIMEOUT);
	{
	    return -1;
	}
	    
    return 0;
}

int mutex_unlock (mutex_t *mutex)
{
	if(mutex == NULL)
	{
		return -1;
	}

	if(OS_NO_ERR==OSSemPost(mutex->mtx))
	{
        return 0;
	}
    else
    {
        return -1;
	}
}