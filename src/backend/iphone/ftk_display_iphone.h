
#ifndef FTK_DISPLAY_IPHONE_H
#define FTK_DISPLAY_IPHONE_H

#include "ftk_display.h"

FTK_BEGIN_DECLS

FtkDisplay* ftk_display_iphone_create(void);

void ftk_display_iphone_show_keyboard(FtkDisplay* thiz);
void ftk_display_iphone_hide_keyboard(FtkDisplay* thiz);

FTK_END_DECLS

#endif/*FTK_DISPLAY_IPHONE_H*/
