
#ifndef FTK_IPHONE_GLES_VIEW_H
#define FTK_IPHONE_GLES_VIEW_H

#import "ftk_iphone_view.h"
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface FtkGlesView : FtkView
{
@private
	EAGLContext* context;
	GLint backingWidth;
	GLint backingHeight;
	GLuint viewRenderbuffer;
	GLuint viewFramebuffer;
	GLuint depthRenderbuffer;
}

@property (nonatomic, retain) EAGLContext *context;

-(BOOL)createFramebuffer;
-(void)destroyFramebuffer;
@end

#endif/*FTK_IPHONE_GLES_VIEW_H*/
