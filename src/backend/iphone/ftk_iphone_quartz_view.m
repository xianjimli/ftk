
#import "ftk_iphone_quartz_view.h"

@implementation FtkQuartzView

-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];

	ctx = nil;
	image = nil;

	return self;
}

-(void)layoutSubviews
{
	[super layoutSubviews];
}

-(void)dealloc
{
	CGContextRelease(ctx);
	[super dealloc];
}

-(void)drawView:(FtkBitmap*)bitmap rect:(FtkRect*)rect bits:(void*)bits xoffset:(int)xoffset yoffset:(int)yoffset width:(int)width height:(int)height
{
	if(ctx == nil)
	{
		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		ctx = CGBitmapContextCreate(bits, width, height, 8, 4 * width, color_space, kCGImageAlphaPremultipliedLast);
		CGColorSpaceRelease(color_space);
	}

	//if (image)
	//{
		//[image release];
		image = nil;
	//}

	[self setNeedsDisplayInRect:CGRectMake(xoffset, yoffset, rect->width, rect->height)];
}

-(void)drawRect:(CGRect)rect
{
	CGImageRef img_ref = CGBitmapContextCreateImage(ctx);
	if(!img_ref)
	{
		return;
	}

	CGImageRef redraw_img_ref = CGImageCreateWithImageInRect(img_ref, rect);
	if(!redraw_img_ref)
	{
		CGImageRelease(img_ref);
		return;
	}

	image = [UIImage imageWithCGImage:redraw_img_ref];
	if(!image)
	{
		CGImageRelease(redraw_img_ref);
		CGImageRelease(img_ref);
		return;
	}

	[image drawInRect:rect];

	CGImageRelease(redraw_img_ref);
	CGImageRelease(img_ref);
}

@end
