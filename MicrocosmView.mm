//
//  MicrocosmView.m
//  Microcosm
//
//  Created by Nick Zitzmann on 6/26/10.
//  Copyright (c) 2010, __MyCompanyName__. All rights reserved.
//

#import "MicrocosmView.h"
#import "Microcosm/extensions.h"
#import <AGL/agl.h>

inline BOOL canUseShaders(void)
{
	SInt32 osVersion;
	
	Gestalt(gestaltSystemVersion, &osVersion);
	if (queryExtension("GL_ARB_multitexture") == false || queryExtension("GL_ARB_shader_objects") == false || osVersion < 0x1043)	// check to see if those extensions are available, and that we're using Mac OS X 10.4.3 or later
		return NO;
	return YES;
}

@interface MicrocosmView (Private)
- (void)readDefaults:(ScreenSaverDefaults *)defaults;
@end

@implementation MicrocosmView

@synthesize mainScreenOnly = lMainScreenOnly;

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
	
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self)
	{
		lSettings.doingPreview = isPreview;
		if (isPreview)
			lMainScreen = YES;
		else
			lMainScreen = (frame.origin.x == 0 && frame.origin.y == 0) ? YES : NO;
		lIsConfiguring = NO;
		lMainScreenOnly = [defaults boolForKey:@"MainScreenOnly"];
		if (lMainScreen || !lMainScreenOnly)
		{
			NSOpenGLPixelFormatAttribute attribs[] = 
			{
				NSOpenGLPFAAccelerated, (NSOpenGLPixelFormatAttribute)YES,
				NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)32,
				NSOpenGLPFADoubleBuffer, (NSOpenGLPixelFormatAttribute)YES,
				NSOpenGLPFAMinimumPolicy, (NSOpenGLPixelFormatAttribute)YES,
				NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
				(NSOpenGLPixelFormatAttribute)0
			};
            NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease];
            
            if (format)
            {
                lView = [[[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];
                [self addSubview:lView];
				
                lSettings.frameTime = 0;
                
				[[lView openGLContext] makeCurrentContext];	// must do this before RSSShadersSupported() will work
				setDefaults(0, &lSettings);
                if ([defaults objectForKey:@"SingleTime"])
                {
                    [self readDefaults:defaults];
                }
                
				if (!canUseShaders())
					lSettings.dShaders = NO;
                [self setAnimationTimeInterval:1/30.0];
            }
		}
    }
    return self;
}


- (void)dealloc
{
	[ibConfigureSheet release];
	[super dealloc];
}


- (void)setFrameSize:(NSSize)size
{
	[super setFrameSize:size];
	if (lView)
		[lView setFrameSize:size];
	reshape(int(size.width), int(size.height), &lSettings);
}


- (void)startAnimation
{
    [super startAnimation];
	if (lIsConfiguring == NO && lView)
    {
        if (lMainScreen || !lMainScreenOnly)
        {
			GLint interval = 1;
            
            [self lockFocus];
            [[lView openGLContext] makeCurrentContext];
            
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();
            CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &interval);	// don't allow screen tearing
            [[lView openGLContext] flushBuffer];
            
            initSaver(&lSettings);
			[self unlockFocus];
        }
    }
}


- (void)stopAnimation
{
	[super stopAnimation];
	
	if (lIsConfiguring == NO && lView)
    {
        if (lMainScreen || !lMainScreenOnly)
		{
			[[lView openGLContext] makeCurrentContext];
			cleanUp(&lSettings);
            lSettings.frameTime=0;
		}
	}
}


- (void)drawRect:(NSRect)rect
{
	[[NSColor blackColor] set];
    NSRectFill(rect);
    
    if (lView == nil)
    {    
		if (lMainScreen || !lMainScreenOnly)
        {
            NSRect tFrame=[self frame];
            NSRect tStringFrame;
            NSDictionary * tAttributes;
            NSString * tString;
            NSMutableParagraphStyle * tParagraphStyle;
            
            tParagraphStyle=[[NSParagraphStyle defaultParagraphStyle] mutableCopy];
            [tParagraphStyle setAlignment:NSCenterTextAlignment];
            
            tAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSize]],NSFontAttributeName,[NSColor whiteColor],NSForegroundColorAttributeName,tParagraphStyle,NSParagraphStyleAttributeName,nil];
            
            [tParagraphStyle release];
            
            tString=NSLocalizedStringFromTableInBundle(@"Minimum OpenGL requirements\rfor this Screen Effect\rnot available\ron your graphic card.",@"Localizable",[NSBundle bundleForClass:[self class]],@"No comment");
            
            tStringFrame.origin=NSZeroPoint;
            tStringFrame.size=[tString sizeWithAttributes:tAttributes];
            
            tStringFrame=SSCenteredRectInRect(tStringFrame,tFrame);
            
            [tString drawInRect:tStringFrame withAttributes:tAttributes];
        }
    }
}


- (void)animateOneFrame
{
	if (lIsConfiguring == NO && lView)
    {
        if (lMainScreen || !lMainScreenOnly)
        {
			lSettings.frameTime = timer.tick();
			if (lSettings.readyToDraw)
			{
				draw(&lSettings);
				[[lView openGLContext] flushBuffer];
			}
        }
    }
}


- (BOOL)hasConfigureSheet
{
    return (lView != nil);
}


- (NSWindow*)configureSheet
{
	lIsConfiguring = YES;
	if (ibConfigureSheet == nil)
	{
		if ([NSBundle loadNibNamed:@"ConfigureSheet" owner:self])
		{
			[ibVersionTxt setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];	// set the version text
			if (canUseShaders())
				[ibShaderCbx setEnabled:NO];
		}
		else
			NSLog(@"Warning: %@ couldn't load ConfigureSheet.nib.", [self className]);
	}
	return ibConfigureSheet;
}


#pragma mark -


- (IBAction)closeSheet:(id)sender
{
	NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
	
	[inDefaults setInteger:lSettings.dSingleTime forKey:@"SingleTime"];
	[inDefaults setInteger:lSettings.dKaleidoscopeTime forKey:@"KaleidoscopeTime"];
	[inDefaults setInteger:lSettings.dBackground forKey:@"Background"];
	[inDefaults setInteger:lSettings.dResolution forKey:@"Resolution"];
	[inDefaults setInteger:lSettings.dDepth forKey:@"Depth"];
	[inDefaults setInteger:lSettings.dFov forKey:@"Fov"];
	[inDefaults setInteger:lSettings.dGizmoSpeed forKey:@"GizmoSpeed"];
	[inDefaults setInteger:lSettings.dColorSpeed forKey:@"ColorSpeed"];
	[inDefaults setInteger:lSettings.dCameraSpeed forKey:@"CameraSpeed"];
	[inDefaults setBool:lSettings.dShaders forKey:@"Shaders"];
	[inDefaults setBool:lSettings.dFog forKey:@"Fog"];
	[inDefaults setBool:lMainScreenOnly forKey:@"MainScreenOnly"];
	
	lIsConfiguring = NO;
	if ([self isAnimating])
	{
		[self stopAnimation];
		[self startAnimation];
	}
	[NSApp endSheet:ibConfigureSheet];
	[ibConfigureSheet close];
}


- (IBAction)reset:(id)sender
{
	[self willChangeValueForKey:@"singleTime"];
	[self willChangeValueForKey:@"kaleidoscopeTime"];
	[self willChangeValueForKey:@"background"];
	[self willChangeValueForKey:@"resolution"];
	[self willChangeValueForKey:@"depth"];
	[self willChangeValueForKey:@"fov"];
	[self willChangeValueForKey:@"gizmoSpeed"];
	[self willChangeValueForKey:@"colorSpeed"];
	[self willChangeValueForKey:@"cameraSpeed"];
	[self willChangeValueForKey:@"shaders"];
	[self willChangeValueForKey:@"fog"];
	setDefaults(1, &lSettings);
	if (!canUseShaders())
		lSettings.dShaders = NO;
	[self didChangeValueForKey:@"fog"];
	[self didChangeValueForKey:@"shaders"];
	[self didChangeValueForKey:@"cameraSpeed"];
	[self didChangeValueForKey:@"colorSpeed"];
	[self didChangeValueForKey:@"gizmoSpeed"];
	[self didChangeValueForKey:@"fov"];
	[self didChangeValueForKey:@"depth"];
	[self didChangeValueForKey:@"resolution"];
	[self didChangeValueForKey:@"background"];
	[self didChangeValueForKey:@"kaleidoscopeTime"];
	[self didChangeValueForKey:@"singleTime"];
	self.mainScreenOnly = NO;
}


#pragma mark -


- (int)singleTime
{
	return lSettings.dSingleTime;
}


- (void)setSingleTime:(int)st
{
	lSettings.dSingleTime = st;
}


- (int)kaleidoscopeTime
{
	return lSettings.dKaleidoscopeTime;
}


- (void)setKaleidoscopeTime:(int)kTime
{
	lSettings.dKaleidoscopeTime = kTime;
}


- (int)background
{
	return lSettings.dBackground;
}


- (void)setBackground:(int)bg
{
	lSettings.dBackground = bg;
}


- (int)resolution
{
	return lSettings.dResolution;
}


- (void)setResolution:(int)r
{
	lSettings.dResolution = r;
}


- (int)depth
{
	return lSettings.dDepth;
}


- (void)setDepth:(int)d
{
	lSettings.dDepth = d;
}


- (int)fov
{
	return lSettings.dFov;
}


- (void)setFov:(int)f
{
	lSettings.dFov = f;
}


- (int)gizmoSpeed
{
	return lSettings.dGizmoSpeed;
}


- (void)setGizmoSpeed:(int)gs
{
	lSettings.dGizmoSpeed = gs;
}


- (int)colorSpeed
{
	return lSettings.dColorSpeed;
}


- (void)setColorSpeed:(int)cs
{
	lSettings.dColorSpeed = cs;
}

- (int)cameraSpeed
{
	return lSettings.dCameraSpeed;
}


- (void)setCameraSpeed:(int)cs
{
	lSettings.dCameraSpeed = cs;
}


- (BOOL)shaders
{
	return lSettings.dShaders;
}


- (void)setShaders:(BOOL)shaders
{
	lSettings.dShaders = shaders;
}


- (BOOL)fog
{
	return lSettings.dFog;
}


- (void)setFog:(BOOL)fog
{
	lSettings.dFog = fog;
}

@end

@implementation MicrocosmView (Private)

- (void)readDefaults:(ScreenSaverDefaults *)defaults
{
	lSettings.dSingleTime = int([defaults integerForKey:@"SingleTime"]);
	lSettings.dKaleidoscopeTime = int([defaults integerForKey:@"KaleidoscopeTime"]);
	lSettings.dBackground = int([defaults integerForKey:@"Background"]);
	lSettings.dResolution = int([defaults integerForKey:@"Resolution"]);
	lSettings.dDepth = int([defaults integerForKey:@"Depth"]);
	lSettings.dFov = int([defaults integerForKey:@"Fov"]);
	lSettings.dGizmoSpeed = int([defaults integerForKey:@"GizmoSpeed"]);
	lSettings.dColorSpeed = int([defaults integerForKey:@"ColorSpeed"]);
	lSettings.dCameraSpeed = int([defaults integerForKey:@"CameraSpeed"]);
	//if (canUseShaders())
		lSettings.dShaders = [defaults boolForKey:@"Shaders"];
	lSettings.dFog = [defaults boolForKey:@"Fog"];
	//lMainScreenOnly = [defaults boolForKey:@"MainScreenOnly"];
}

@end
