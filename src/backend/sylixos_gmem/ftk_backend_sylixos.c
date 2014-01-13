/*
 * File: ftk_backend_sylixos.c
 * Author:  Han.hui <sylixos@gmail.com>
 * Brief:   sylixos backend implemention.
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
 * 2011-05-15 Han.hui <sylixos@gmail.com> created
 *
 */

#include "ftk_display_sylixos.h"
#include "ftk_source_sylixos.h"
#include "ftk_tscalibrate_sylixos.h"
#include "ftk_wnd_manager.h"
#include "ftk_globals.h"

Ret ftk_backend_init(int argc, char *argv[])
{
    FtkDisplay* display = NULL;
    FtkSource*  source  = NULL;
    char*       name    = FTK_DEFAULT_FRAMEBUFFER;
    char        namebuffer[PATH_MAX + 1];

    ftk_sylixos_touchscreen_calibrate(argc, argv);

    if (getenv_r("FRAMEBUFFER", namebuffer, PATH_MAX + 1) >= 0)
    {
        name = namebuffer;
    }

    display = ftk_display_sylixos_create(name);

    ftk_set_display(display);

    source = ftk_source_sylixos_input_create();

    ftk_sources_manager_add(ftk_default_sources_manager(), source);

    return display != NULL && source != NULL ? RET_OK : RET_FAIL;
}
