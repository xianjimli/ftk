
#ifndef FTK_IPHONE_QUARTZ_VIEW_H
#define FTK_IPHONE_QUARTZ_VIEW_H

#import "ftk_iphone_view.h"
#import <QuartzCore/QuartzCore.h>

@interface FtkQuartzView : FtkView
{
@private
	CGContextRef ctx;
	UIImage* image;
}
@end

#endif/*FTK_IPHONE_QUARTZ_VIEW_H*/
