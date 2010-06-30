//
//  MicrocosmView.h
//  Microcosm
//
//  Created by Nick Zitzmann on 6/26/10.
//  Copyright (c) 2010, __MyCompanyName__. All rights reserved.
//

#import <ScreenSaver/ScreenSaver.h>
#import "Microcosm/Microcosm.h"
#import "util/rsTimer.h"

@interface MicrocosmView : ScreenSaverView 
{
	NSOpenGLView *lView;
	MicrocosmSaverSettings lSettings;
	BOOL lMainScreen;
	BOOL lMainScreenOnly;
	BOOL lIsConfiguring;
	
	rsTimer timer;
	
	IBOutlet NSWindow *ibConfigureSheet;
	IBOutlet NSTextField *ibVersionTxt;
	IBOutlet NSButton *ibShaderCbx;
}
- (IBAction)closeSheet:(id)sender;
- (IBAction)reset:(id)sender;

@property(assign,nonatomic) int singleTime;
@property(assign,nonatomic) int kaleidoscopeTime;
@property(assign,nonatomic) int background;
@property(assign,nonatomic) int resolution;
@property(assign,nonatomic) int depth;
@property(assign,nonatomic) int fov;
@property(assign,nonatomic) int gizmoSpeed;
@property(assign,nonatomic) int colorSpeed;
@property(assign,nonatomic) int cameraSpeed;
@property(assign,nonatomic) BOOL shaders;
@property(assign,nonatomic) BOOL fog;
@property(assign,nonatomic) BOOL mainScreenOnly;

@end
