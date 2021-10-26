//
//  MicrocosmView.m
//  Microcosm
//
//  Created by Nick Zitzmann on 6/26/10.
//  Copyright (c) 2010, __MyCompanyName__. All rights reserved.
//

/* This file is part of Microcosm.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
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

#import "MicrocosmView.h"
#import "Microcosm/extensions.h"

@interface MicrocosmView (Private)
- (void)readDefaults:(ScreenSaverDefaults *)defaults;
@end

@implementation MicrocosmView

- (id)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
	NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
	
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self)
	{
		//lSettings.doingPreview = isPreview;
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
				NSOpenGLPFAAccelerated,
				NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)32,
				NSOpenGLPFADoubleBuffer,
				NSOpenGLPFAMinimumPolicy,
				NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
				NSOpenGLPFAAllowOfflineRenderers,
				(NSOpenGLPixelFormatAttribute)0
			};
            NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
            
            if (format)
            {
                lView = [[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format];
                [self addSubview:lView];
				lView.wantsBestResolutionOpenGLSurface = YES;
                
				[[lView openGLContext] makeCurrentContext];	// must do this before queryExtension() will work
				setDefaults(0, &lSettings);
                if ([defaults objectForKey:@"SingleTime"])
                {
                    [self readDefaults:defaults];
                }
                
				self.animationTimeInterval = 1.0/60.0;
				lSettings.first = true;
            }
		}
    }
    return self;
}


- (void)setFrameSize:(NSSize)size
{
	[super setFrameSize:size];
	if (lView)
		[lView setFrameSize:size];
	
	if (lView.wantsBestResolutionOpenGLSurface)	// on Lion & later, if we're using a best resolution surface, then call glViewport() with the appropriate width and height for the backing
	{
		NSRect newBounds = [lView convertRectToBacking:lView.bounds];
		
		reshape(int(newBounds.size.width), int(newBounds.size.height), &lSettings);
	}
	else
		reshape(int(size.width), int(size.height), &lSettings);
}


- (void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];
	if (@available(macOS 12.0, *))	// on Monterey and later, update the time interval for the window's screen's refresh rate
	{
		self.animationTimeInterval = self.window.screen.maximumRefreshInterval;
	}
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
			gettimeofday(&timer.prev_tv, NULL);	// reset the timer
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
				[[lView openGLContext] makeCurrentContext];
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
		if ([[NSBundle bundleForClass:self.class] loadNibNamed:@"ConfigureSheet" owner:self topLevelObjects:NULL])
		{
			[ibVersionTxt setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];	// set the version text
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
	[inDefaults synchronize];
	
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
	setDefaults(0, &lSettings);
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


- (BOOL)mainScreenOnly
{
	return lMainScreenOnly;
}


- (void)setMainScreenOnly:(BOOL)mso
{
	lMainScreenOnly = mso;
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
