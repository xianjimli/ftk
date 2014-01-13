
#ifndef FTK_JNI_H
#define FTK_JNI_H

#include "ftk_typedef.h"
#include "ftk_bitmap.h"

FTK_BEGIN_DECLS

int main(int argc, char* argv[]);

void Android_SetScreenResolution(int width, int height);

void Android_Log(const char* fmt, ...);
int Android_OpenAsset(const char* filename, size_t* size);
FtkBitmap* Android_LoadImage(const char* filename);

void Android_InitEGL(void);
Ret Android_PreRender(void);
void Android_Render(void);

int* Android_GetBitmapPixels(void);
void Android_ReleaseBitmapPixels(int* pixels);
void Android_DrawBitmap(int offset, int stride, int x, int y, int width, int height);

void Android_ShowKeyboard(void);
void Android_HideKeyboard(void);
void Android_ShowInputMethodChooser(void);

FTK_END_DECLS

#endif/*FTK_JNI_H*/
