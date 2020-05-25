/*
 *  Microcosm.h
 *  Microcosm
 *
 *  Created by Nick Zitzmann on 6/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

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

#ifndef MICROCOSM_H
#define MICROCOSM_H

#include <vector>
#include "rsCamera.h"
#include "MirrorBox.h"
#include "Gizmo.h"
#include "util/rsTimer.h"

class rsText;
class impCubeVolume;
class impSurface;
class Texture1D;

typedef struct MicrocosmSaverSettings
{
	// Globals
#ifdef WIN32
	LPCTSTR registryPath = ("Software\\Really Slick\\Microcosm");
	HDC hdc;
	HGLRC hglrc;
#endif
	bool doingPreview;
	
	int readyToDraw;// = 0;
	float frameTime;// = 0.0f;
	float computeTime;// = 0.0f;
	float drawTime;// = 0.0f;
	float aspectRatio;
	float gHFov;
	float gVFov;
	rsCamera gCamera;
	int gMode;// = 0;   // 0 = single centered Gizmo; 1 = kaleidoscope mode
	float gModeTransition;// = 0.0f;
	float gModeTransitionDir;// = 1.0f;
	// text output
	rsText* textwriter;
	bool first;
	// Parameters edited in the dialog box
	int dSingleTime;
	int dKaleidoscopeTime;
	int dBackground;
	int dResolution;
	int dDepth;
	int dFov;
	int dGizmoSpeed;
	int dColorSpeed;
	int dCameraSpeed;
	int dShaders;
	int dFog;
	bool kStatistics;
	
	int gSpecificGizmo;// = -1;
	float gNumberInputTimer;// = 0.0f;
	
	MirrorBox mirrorbox;
	
	std::vector<Gizmo*> gizmos;
	unsigned int gGizmoIndex;// = 0;
	ShapeVector shapes;
	unsigned int gNumShapes;// = 0;
	impCrawlPointVector crawlpoints;
	impCubeVolume* volume0;
	impCubeVolume* volume1;
	impCubeVolume* volume2;
	// Double buffers so that each of 3 volumes can store into one surface
	// while the other is being used for drawing.
	impSurface* volSurface0[2];
	impSurface* volSurface1[2];
	impSurface* volSurface2[2];
	// Pointers to surfaces that can be used for drawing
	impSurface* drawSurface0;
	impSurface* drawSurface1;
	impSurface* drawSurface2;
	
	Texture1D* tex1d;
	
	// eye coordinates for surface function
	float sfEyeX, sfEyeY, sfEyeZ;
	
	// easter egg
	// Tennis gizmo is not immediately available
	bool gTennisAvailable;// = false;
	
	// multi-threading
	// Two worker threads are used to compute frame (n+1)'s surfaces while frame n
	// is being drawn.  This gives a bit of a performance advantage on multi-core
	// processors.
	// When the main thread is ready to draw on the screen, it signals the worker
	// threads to start computing surfaces.  Then when the main thread is done
	// drawing, it waits until the worker threads signal it that they are done.
	// That way the main thread does not restart the draw() function and change
	// the surface parameters until the worker threads are done using those parameters.
	bool gUseThreads;// = true;
#ifdef WIN32
	HANDLE gT0Start;  // for signaling thread 0 to start
	HANDLE gT0End;  // for thread 0 to signal main thread that it is done
	HANDLE gT1Start;
	HANDLE gT1End;
#else
	pthread_t gThread0;
	pthread_t gThread1;
	// conditional variables
	pthread_cond_t gT0Start;// = PTHREAD_COND_INITIALIZER;  // for signaling thread 0 to start
	pthread_cond_t gT0End;// = PTHREAD_COND_INITIALIZER;  // for thread 0 to signal main thread that it is done
	pthread_cond_t gT1Start;// = PTHREAD_COND_INITIALIZER;
	pthread_cond_t gT1End;// = PTHREAD_COND_INITIALIZER;
	// each conditional variable requires a mutex
	pthread_mutex_t gT0StartMutex;// = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t gT0EndMutex;// = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t gT1StartMutex;// = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t gT1EndMutex;// = PTHREAD_MUTEX_INITIALIZER;
#endif
	
	// camera variables
	float rotPhase[3];// = {rsRandf(RS_PIx2), rsRandf(RS_PIx2), rsRandf(RS_PIx2)};
	float rotRate[3];// = {0.075f + rsRandf(0.05f), 0.075f + rsRandf(0.05f), 0.075f + rsRandf(0.05f)};
	float rot[3];
	rsVec cam0Start;
	rsVec cam0End;//(rsRandf(2.0f) - 1.0f, rsRandf(2.0f) - 1.0f, -2.0f / tanf(0.5f * min(inSettings->gHFov, inSettings->gVFov)));
	rsVec cam0Pos;// = cam0End;
	rsMatrix cam0Background;
	rsVec cam1Pos;//(0.1f, 0.2f, 0.3f);
	float cam0t;// = 1.0f;
	
	rsTimer computeTimer;
	float easterEggTime;// = 0.0f;
	float transitionTime;// = 0.0f;
	int whichsurface;// = 0;
	rsTimer drawTimer;
	float tcphase[3];// = {rsRandf(RS_PIx2), rsRandf(RS_PIx2), rsRandf(RS_PIx2)};
};

__private_extern__ void draw(MicrocosmSaverSettings *inSettings);
__private_extern__ void setDefaults(int which, MicrocosmSaverSettings *inSettings);
__private_extern__ void reshape(int width, int height, MicrocosmSaverSettings *inSettings);
__private_extern__ void initSaver(MicrocosmSaverSettings *inSettings);
__private_extern__ void cleanUp(MicrocosmSaverSettings *inSettings);

#endif
