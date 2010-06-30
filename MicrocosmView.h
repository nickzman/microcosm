//
//  MicrocosmView.h
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

- (int)singleTime;
- (void)setSingleTime:(int)st;
- (int)kaleidoscopeTime;
- (void)setKaleidoscopeTime:(int)kTime;
- (int)background;
- (void)setBackground:(int)bg;
- (int)resolution;
- (void)setResolution:(int)r;
- (int)depth;
- (void)setDepth:(int)d;
- (int)fov;
- (void)setFov:(int)f;
- (int)gizmoSpeed;
- (void)setGizmoSpeed:(int)gs;
- (int)colorSpeed;
- (void)setColorSpeed:(int)cs;
- (int)cameraSpeed;
- (void)setCameraSpeed:(int)cs;
- (BOOL)shaders;
- (void)setShaders:(BOOL)shaders;
- (BOOL)fog;
- (void)setFog:(BOOL)fog;
- (BOOL)mainScreenOnly;
- (void)setMainScreenOnly:(BOOL)mso;

@end
