
#ifndef FTK_IPHONE_VIEW_H
#define FTK_IPHONE_VIEW_H

#include "ftk_event.h"
#include "ftk_bitmap.h"
#import <UIKit/UIKit.h>

@interface FtkView : UIView<UITextFieldDelegate>
{
@private
	UITextField* text_field;
	FtkEvent ev;
}

-(id)initWithFrame:(CGRect)frame;

-(void)drawView:(FtkBitmap*)bitmap rect:(FtkRect*)rect bits:(void*)bits xoffset:(int)xoffset yoffset:(int)yoffset width:(int)width height:(int)height;

-(void)showKeyboard;
-(void)hideKeyboard;

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
@end

void ftk_iphone_view_create_win(void);
void ftk_iphone_view_destroy_win(void);

void ftk_iphone_view_draw(FtkBitmap* bitmap, FtkRect* rect, void* bits, int xoffset, int yoffset, int width, int height);

void ftk_iphone_view_show_keyboard(void);
void ftk_iphone_view_hide_keyboard(void);

#endif/*FTK_IPHONE_VIEW_H*/
