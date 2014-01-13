/*
 * File: ftk_tscalibrate_sylixos.c
 * Author:  JiaoJinXing <jiaojinxing1987@gmail.com>
 * Brief:   sylixos touchscreen calibrate.
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
 * 2011-07-08 JiaoJinXing <jiaojinxing1987@gmail.com> created
 *
 */

/*
 *  tslib/tests/ts_calibrate.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_calibrate.c,v 1.8 2004/10/19 22:01:27 dlowder Exp $
 *
 * Basic test program for touchscreen library.
 */

#include "ftk_display_sylixos.h"
#include "ftk_source_sylixos.h"
#include "ftk_tscalibrate_sylixos.h"
#include <string.h>
#include <mouse.h>
#include <input_device.h>
#include "font.h"

void* ftk_display_bits(FtkDisplay* thiz, int* bpp);

union multiptr
{
    unsigned char  *p8;
    unsigned short *p16;
    unsigned long  *p32;
};

#define XORMODE 0x80000000

static inline void __set_pixel(unsigned int   bpp,
                               union multiptr loc,
                               unsigned int   xormode,
                               unsigned int   color)
{
    switch(bpp)
    {
    case 1:
    default:
        if (xormode)
            *loc.p8 ^= color;
        else
            *loc.p8 = color;
        break;

    case 2:
        if (xormode)
            *loc.p16 ^= color;
        else
            *loc.p16 = color;
        break;

    case 4:
        if (xormode)
            *loc.p32 ^= color;
        else
            *loc.p32 = color;
        break;
    }
}

static void draw_pixel(FtkDisplay* display, int x, int y, unsigned int colidx)
{
    void*          bits;
    int            bpp;
    unsigned int   xormode;
    union multiptr loc;

    if ((x < 0) || (x >= ftk_display_width(display)) ||
        (y < 0) || (y >= ftk_display_height(display)))
    {
        return;
    }

    bits    = ftk_display_bits(display, &bpp);
    loc.p8  = (unsigned char *)bits + (ftk_display_width(display) * y + x) * bpp;

    xormode = colidx & XORMODE;
    colidx &= ~XORMODE;

    __set_pixel(bpp, loc, xormode, colidx);
}

static void draw_line(FtkDisplay* display, int x1, int y1, int x2, int y2, unsigned int colidx)
{
    int tmp;
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (abs(dx) < abs(dy))
    {
        if (y1 > y2)
        {
            tmp = x1; x1 = x2; x2 = tmp;
            tmp = y1; y1 = y2; y2 = tmp;
            dx = -dx; dy = -dy;
        }
        x1 <<= 16;
        /* dy is apriori >0 */
        dx = (dx << 16) / dy;
        while (y1 <= y2)
        {
            draw_pixel(display, x1 >> 16, y1, colidx);
            x1 += dx;
            y1++;
        }
    }
    else
    {
        if (x1 > x2)
        {
            tmp = x1; x1 = x2; x2 = tmp;
            tmp = y1; y1 = y2; y2 = tmp;
            dx = -dx; dy = -dy;
        }
        y1 <<= 16;
        dy = dx ? (dy << 16) / dx : 0;
        while (x1 <= x2)
        {
            draw_pixel(display, x1, y1 >> 16, colidx);
            y1 += dy;
            x1++;
        }
    }
}

static void draw_cross(FtkDisplay* display, int x, int y, unsigned int colidx)
{
    draw_line(display, x - 10, y, x - 2, y, colidx);
    draw_line(display, x + 2, y, x + 10, y, colidx);
    draw_line(display, x, y - 10, x, y - 2, colidx);
    draw_line(display, x, y + 2, x, y + 10, colidx);

    draw_line(display, x - 6, y - 9, x - 9, y - 9, colidx);
    draw_line(display, x - 9, y - 8, x - 9, y - 6, colidx);
    draw_line(display, x - 9, y + 6, x - 9, y + 9, colidx);
    draw_line(display, x - 8, y + 9, x - 6, y + 9, colidx);
    draw_line(display, x + 6, y + 9, x + 9, y + 9, colidx);
    draw_line(display, x + 9, y + 8, x + 9, y + 6, colidx);
    draw_line(display, x + 9, y - 6, x + 9, y - 9, colidx);
    draw_line(display, x + 8, y - 9, x + 6, y - 9, colidx);
}

static void put_char(FtkDisplay* display, int x, int y, int c, unsigned int colidx)
{
    int i, j, bits;

    for (i = 0; i < font_vga_8x16.height; i++)
    {
        bits = font_vga_8x16.data[font_vga_8x16.height * c + i];
        for (j = 0; j < font_vga_8x16.width; j++, bits <<= 1)
        {
            if (bits & 0x80)
            {
                draw_pixel(display, x + j, y + i, colidx);
            }
            else
            {
                draw_pixel(display, x + j, y + i, 0);
            }
        }
    }
}

static void put_string(FtkDisplay* display, int x, int y, const char *s, unsigned int colidx)
{
    int i;

    for (i = 0; *s; i++, x += font_vga_8x16.width, s++)
    {
        put_char(display, x, y, *s, colidx);
    }
}

static void put_string_center(FtkDisplay* display, int x, int y, const char *s, unsigned int colidx)
{
    size_t sl = strlen (s);

    put_string(display,
               x - (sl / 2) * font_vga_8x16.width,
               y - font_vga_8x16.height / 2,
               s,
               colidx);
}

typedef struct
{
    int x[5], xfb[5];
    int y[5], yfb[5];
    int a[7];
} calibration;

struct ts_sample
{
    int          x;
    int          y;
    unsigned int pressure;
};

static int ts_read_raw(int ts_fd, struct ts_sample* samp, int nr)
{
    touchscreen_event_notify notify;
    fd_set                   rfds;

    FD_ZERO(&rfds);

    FD_SET(ts_fd, &rfds);

    if (select(ts_fd + 1, &rfds, NULL, NULL, NULL) != 1)
    {
        return -1;
    }

    if (read(ts_fd, (char *)&notify, sizeof(touchscreen_event_notify)) == sizeof(touchscreen_event_notify))
    {
        samp->x        = notify.xanalog;
        samp->y        = notify.yanalog;
        samp->pressure = notify.kstat & MOUSE_LEFT;
        return 1;
    }
    else
    {
        return -1;
    }
}

static int sort_by_x(const void* a, const void* b)
{
    return (((struct ts_sample*)a)->x - ((struct ts_sample*)b)->x);
}

static int sort_by_y(const void* a, const void* b)
{
    return (((struct ts_sample*)a)->y - ((struct ts_sample*)b)->y);
}

static void get_xy(int ts_fd, FtkDisplay* display, int* x, int* y)
{
#define MAX_SAMPLES 128
    struct ts_sample samp[MAX_SAMPLES];
    int index, middle;

    do
    {
        if (ts_read_raw(ts_fd, &samp[0], 1) < 0)
        {
            perror("ts_read_raw");
            continue;
        }
    } while (samp[0].pressure == 0);

    /* Now collect up to MAX_SAMPLES touches into the samp array. */
    index = 0;
    do
    {
        if (index < MAX_SAMPLES - 1)
        {
            index++;
        }
        if (ts_read_raw(ts_fd, &samp[index], 1) < 0)
        {
            perror("ts_read_raw");
            continue;
        }
    } while (samp[index].pressure > 0);

    printf("Took %d samples...\n", index);

    /*
     * At this point, we have samples in indices zero to (index-1)
     * which means that we have (index) number of samples.  We want
     * to calculate the median of the samples so that wild outliers
     * don't skew the result.  First off, let's assume that arrays
     * are one-based instead of zero-based.  If this were the case
     * and index was odd, we would need sample number ((index+1)/2)
     * of a sorted array; if index was even, we would need the
     * average of sample number (index/2) and sample number
     * ((index/2)+1).  To turn this into something useful for the
     * real world, we just need to subtract one off of the sample
     * numbers.  So for when index is odd, we need sample number
     * (((index+1)/2)-1).  Due to integer division truncation, we
     * can simplify this to just (index/2).  When index is even, we
     * need the average of sample number ((index/2)-1) and sample
     * number (index/2).  Calculate (index/2) now and we'll handle
     * the even odd stuff after we sort.
     */
    middle = index / 2;
    if (x)
    {
        qsort(samp, index, sizeof(struct ts_sample), sort_by_x);
        if (index & 1)
        {
            *x = samp[middle].x;
        }
        else
        {
            *x = (samp[middle-1].x + samp[middle].x) / 2;
        }
    }

    if (y)
    {
        qsort(samp, index, sizeof(struct ts_sample), sort_by_y);
        if (index & 1)
        {
            *y = samp[middle].y;
        }
        else
        {
            *y = (samp[middle-1].y + samp[middle].y) / 2;
        }
    }
}

static void get_sample(int ts_fd, FtkDisplay* display, calibration* cal, int index, int x, int y, const char* name)
{
    static int last_x = -1, last_y;

    put_string_center(display,
                      ftk_display_width(display) / 2,
                      ftk_display_height(display) / 4 + 60,
                      name,
                      0x0fffffff);

    if (last_x != -1)
    {
#define NR_STEPS 10
        int dx = ((x - last_x) << 16) / NR_STEPS;
        int dy = ((y - last_y) << 16) / NR_STEPS;
        int i;

        last_x <<= 16;
        last_y <<= 16;
        for (i = 0; i < NR_STEPS; i++)
        {
            draw_cross(display, last_x >> 16, last_y >> 16, 0x0fffffff | XORMODE);
            usleep(1000);
            draw_cross(display, last_x >> 16, last_y >> 16, 0x0fffffff | XORMODE);
            last_x += dx;
            last_y += dy;
        }
    }

    draw_cross(display, x, y, 0x0fffffff | XORMODE);
    get_xy(ts_fd, display, &cal->x[index], &cal->y[index]);
    draw_cross(display, x, y, 0x0fffffff | XORMODE);

    last_x = cal->xfb[index] = x;
    last_y = cal->yfb[index] = y;

    printf("%s : X = %4d Y = %4d\n", name, cal->x[index], cal->y[index]);
}

static int perform_calibration(calibration* cal)
{
    int   j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, a, b, c, e, f, i;
    float scaling = 65536.0;

    // Get sums for matrix
    n = x = y = x2 = y2 = xy = 0;
    for (j = 0; j < 5; j++)
    {
        n  += 1.0;
        x  += (float)cal->x[j];
        y  += (float)cal->y[j];
        x2 += (float)(cal->x[j]*cal->x[j]);
        y2 += (float)(cal->y[j]*cal->y[j]);
        xy += (float)(cal->x[j]*cal->y[j]);
    }

    // Get determinant of matrix -- check if determinant is too small
    det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
    if (det < 0.1 && det > -0.1)
    {
        printf("ts_calibrate: determinant is too small -- %f\n", det);
        return 0;
    }

    // Get elements of inverse matrix
    a = (x2*y2 - xy*xy) / det;
    b = (xy*y  - x*y2)  / det;
    c = (x*xy  - y*x2)  / det;
    e = (n*y2  - y*y)   / det;
    f = (x*y   - n*xy)  / det;
    i = (n*x2  - x*x)   / det;

    // Get sums for x calibration
    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z  += (float) cal->xfb[j];
        zx += (float)(cal->xfb[j]*cal->x[j]);
        zy += (float)(cal->xfb[j]*cal->y[j]);
    }

    // Now multiply out to get the calibration for framebuffer x coord
    cal->a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
    cal->a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
    cal->a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

    printf("%f %f %f\n",
            (a*z + b*zx + c*zy),
            (b*z + e*zx + f*zy),
            (c*z + f*zx + i*zy));

    // Get sums for y calibration
    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z  += (float) cal->yfb[j];
        zx += (float)(cal->yfb[j]*cal->x[j]);
        zy += (float)(cal->yfb[j]*cal->y[j]);
    }

    // Now multiply out to get the calibration for framebuffer y coord
    cal->a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
    cal->a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
    cal->a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

    printf("%f %f %f\n",
            (a*z + b*zx + c*zy),
            (b*z + e*zx + f*zy),
            (c*z + f*zx + i*zy));

    // If we got here, we're OK, so assign scaling to a[6] and return
    cal->a[6] = (int)scaling;

    return 1;
}

int ftk_sylixos_touchscreen_calibrate(int argc, char *argv[])
{
    FtkDisplay*  display;
    char         namebuffer[PATH_MAX + 1];
    char*        name;
    calibration  cal;
    int          xres;
    int          yres;
    int          cal_fd;
    int          ts_fd;
    unsigned int i;
    struct stat  sbuf;
    void*        bits;
    int          bpp;

    if (getenv_r("TSLIB_CALIBFILE", namebuffer, PATH_MAX + 1) >= 0)
    {
        name = namebuffer;
    }
    else
    {
        name = FTK_DEFAULT_CALIBFILE;
    }

    if (!(argc > 1 && (strcmp(argv[1], "ts") == 0)))
    {
        if (stat(name, &sbuf) == 0)
        {
            return 0;
        }
    }

    if (getenv_r("FRAMEBUFFER", namebuffer, PATH_MAX + 1) >= 0)
    {
        name = namebuffer;
    }
    else
    {
        name = FTK_DEFAULT_FRAMEBUFFER;
    }

    display = ftk_display_sylixos_create(name);
    if (display == NULL)
    {
        return -1;
    }

    xres = ftk_display_width(display);
    yres = ftk_display_height(display);

    bits = ftk_display_bits(display, &bpp);
    memset(bits, 0x00, xres * yres * bpp);

    if (getenv_r("TSLIB_TSDEVICE", namebuffer, PATH_MAX + 1) >= 0)
    {
        name = namebuffer;
    }
    else
    {
        name = FTK_DEFAULT_TOUCH;
    }

    ts_fd = open(name, O_RDONLY, 0666);
    if (ts_fd < 0)
    {
        ftk_display_destroy(display);
        return -1;
    }

    put_string_center(display,
                      xres / 2,
                      yres / 4,
                      "TSLIB calibration utility",
                      0x0fffffff);

    put_string_center(display,
                      xres / 2,
                      yres / 4 + 20,
                      "Touch crosshair to calibrate",
                      0x0fffffff);

    printf("xres = %d, yres = %d\n", xres, yres);

    get_sample(ts_fd, display, &cal, 0, 50,        50,        "  Top left  ");
    get_sample(ts_fd, display, &cal, 1, xres - 50, 50,        "  Top right ");
    get_sample(ts_fd, display, &cal, 2, xres - 50, yres - 50, "Bottom right");
    get_sample(ts_fd, display, &cal, 3, 50,        yres - 50, "Bottom left ");
    get_sample(ts_fd, display, &cal, 4, xres / 2,  yres / 2,  "   Center   ");

    if (perform_calibration(&cal))
    {
        printf("Calibration constants: ");
        for (i = 0; i < 7; i++)
        {
            printf("%d ", cal.a[i]);
        }
        printf("\n");

        if (getenv_r("TSLIB_CALIBFILE", namebuffer, PATH_MAX + 1) >= 0)
        {
            name = namebuffer;
        }
        else
        {
            name = FTK_DEFAULT_CALIBFILE;
        }

        cal_fd = creat(name, 0666);

        sprintf(namebuffer, "%d %d %d %d %d %d %d",
                cal.a[1],
                cal.a[2],
                cal.a[0],
                cal.a[4],
                cal.a[5],
                cal.a[3],
                cal.a[6]);

        write(cal_fd, namebuffer, strlen(namebuffer) + 1);

        close(cal_fd);

        i = 0;
    }
    else
    {
        printf("Calibration failed.\n");
        i = -1;
    }

    ftk_display_destroy(display);

    close(ts_fd);

    return i;
}
