
#import "ftk_typedef.h"
#import "ftk_key.h"
#import "ftk_allocator.h"
#import "ftk_iphone_view.h"
#import "ftk_iphone_quartz_view.h"
#import "ftk_iphone_gles_view.h"

@implementation FtkView

-(id)initWithFrame:(CGRect)frame
{
	if((self = [super initWithFrame:frame]) == nil)
	{
		return nil;
	}

	self.userInteractionEnabled = YES;

	text_field = [[[UITextField alloc] initWithFrame:CGRectZero] autorelease];
	text_field.delegate = self;
	text_field.text = @" ";
	text_field.autocapitalizationType = UITextAutocapitalizationTypeNone;
	text_field.autocorrectionType = UITextAutocorrectionTypeNo;
	text_field.enablesReturnKeyAutomatically = NO;
	text_field.keyboardAppearance = UIKeyboardAppearanceDefault;
	text_field.keyboardType = UIKeyboardTypeDefault;
	text_field.returnKeyType = UIReturnKeyDefault;
	text_field.secureTextEntry = NO;
	text_field.hidden = YES;
	[self addSubview: text_field];

	return self;
}

-(void)layoutSubviews
{
}

-(void)dealloc
{
	[text_field release];
	[super dealloc];
}

-(void)drawView:(FtkBitmap*)bitmap rect:(FtkRect*)rect bits:(void*)bits xoffset:(int)xoffset yoffset:(int)yoffset width:(int)width height:(int)height
{
}

-(void)showKeyboard
{
	[text_field becomeFirstResponder];
}

-(void)hideKeyboard
{
	[text_field resignFirstResponder];
}

-(BOOL)textField:(UITextField*)_textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string
{
	if([string length] == 0)
	{
		ev.type = FTK_EVT_KEY_DOWN;
		ev.u.key.code = FTK_KEY_BACKSPACE;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);

		ev.type = FTK_EVT_KEY_UP;
		ev.u.key.code = FTK_KEY_BACKSPACE;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);
	}
	else
	{
		const char* utf8 = [string UTF8String];
		size_t len = strlen(utf8);
		char* buf = FTK_ALLOC(len + 1);
		if (buf == NULL)
		{
			return NO;
		}
		memcpy(buf, utf8, len);
		buf[len] = '\0';

		ftk_logd("%s:%d range:%d-%d %s\n", __FILE__, __LINE__, range.location, range.length, buf);

		ev.u.extra = buf;
		ev.type = FTK_EVT_OS_IM_COMMIT;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);
	}

	return YES;//NO; /* don't allow the edit! (keep placeholder text there) */
}

-(BOOL)textFieldShouldReturn:(UITextField*)_textField
{
	[self hideKeyboard];
	return YES;
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* enumerator = [touches objectEnumerator];
	UITouch* touch = (UITouch*)[enumerator nextObject];

	if(touch != NULL)
	{
		CGPoint locationInView = [touch locationInView:self];
		ev.type = FTK_EVT_MOUSE_DOWN;
		ev.u.mouse.x = locationInView.x;
		ev.u.mouse.y = locationInView.y;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);
	}
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* enumerator = [touches objectEnumerator];
	UITouch* touch = (UITouch*)[enumerator nextObject];

	if(touch != NULL)
	{
		CGPoint locationInView = [touch locationInView:self];
		ev.type = FTK_EVT_MOUSE_UP;
		ev.u.mouse.x = locationInView.x;
		ev.u.mouse.y = locationInView.y;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);
	}
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent*)event
{
	[self touchesEnded:touches withEvent:event];
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	NSEnumerator* enumerator = [touches objectEnumerator];
	UITouch* touch = (UITouch*)[enumerator nextObject];

	if(touch != NULL)
	{
		CGPoint locationInView = [touch locationInView:self];
		ev.u.mouse.x = locationInView.x;
		ev.u.mouse.y = locationInView.y;
		ev.type = FTK_EVT_MOUSE_MOVE;
		ftk_wnd_manager_queue_event_auto_rotate(ftk_default_wnd_manager(), &ev);
	}
}

@end

UIWindow* window;
FtkView* view;

void ftk_iphone_view_create_win(void)
{
	CGRect rect = [[UIScreen mainScreen] bounds];

	[UIApplication sharedApplication].statusBarHidden = YES;

	window = [[UIWindow alloc] initWithFrame:rect];
#if 1
	view = [FtkGlesView alloc];
#else
	view = [FtkQuartzView alloc];
#endif
	view = [view initWithFrame:rect];
	[window addSubview:view];
	[window makeKeyAndVisible];
}

void ftk_iphone_view_destroy_win(void)
{
	[window release];
	[view release];
}

void ftk_iphone_view_draw(FtkBitmap* bitmap, FtkRect* rect, void* bits, int xoffset, int yoffset, int width, int height)
{
	[view drawView:bitmap rect:rect bits:bits xoffset:xoffset yoffset:yoffset width:width height:height];
}

void ftk_iphone_view_show_keyboard(void)
{
	[view showKeyboard];
}

void ftk_iphone_view_hide_keyboard(void)
{
	[view hideKeyboard];
}
