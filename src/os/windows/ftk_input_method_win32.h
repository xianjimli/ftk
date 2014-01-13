
#ifndef FTK_INPUT_METHOD_WIN32_H
#define FTK_INPUT_METHOD_WIN32_H

#include "ftk_input_method.h"

FTK_BEGIN_DECLS

FtkInputMethod* ftk_input_method_win32_create(void);

FtkWidget* ftk_input_method_win32_get_editor(FtkInputMethod* thiz);

FTK_END_DECLS

#endif/*FTK_INPUT_METHOD_WIN32_H*/
