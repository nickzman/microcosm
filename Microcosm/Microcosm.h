/*
 *  Microcosm.h
 *  Microcosm
 *
 *  Created by Nick Zitzmann on 6/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MICROCOSM_H
#define MICROCOSM_H

#include <vector>
#include "rsCamera.h"
#include "MirrorBox.h"
#include "Gizmo.h"

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
};

__private_extern__ void draw(MicrocosmSaverSettings *inSettings);
__private_extern__ void setDefaults(int which, MicrocosmSaverSettings *inSettings);
__private_extern__ void reshape(int width, int height, MicrocosmSaverSettings *inSettings);
__private_extern__ void initSaver(MicrocosmSaverSettings *inSettings);
__private_extern__ void cleanUp(MicrocosmSaverSettings *inSettings);

#endif
