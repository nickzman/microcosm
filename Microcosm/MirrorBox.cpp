/*
 * Copyright (C) 2010  Terence M. Welsh
 *
 * This file is part of Microcosm.
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


#ifdef WIN32
#include <windows.h>
#endif
#include <Microcosm/MirrorBox.h>
#include <Microcosm/rsCamera.h>
#include <OpenGL/OpenGL.h>
#include <Implicit/impCubeVolume.h>


/*extern rsCamera gCamera;
extern impSurface* drawSurface0;
extern impSurface* drawSurface1;
extern impSurface* drawSurface2;*/


MirrorBox::MirrorBox(){
	// Planes overflow past 0.0 and 1.0 by a thousandth to hide cracks
	// caused by floating point inaccuracy
	plane0[0] = 1.0;  plane0[1] = 0.0;  plane0[2] = 0.0;  plane0[3] = 0.0001;
	plane1[0] = -1.0; plane1[1] = 0.0;  plane1[2] = 0.0;  plane1[3] = 1.0001;
	plane2[0] = 0.0;  plane2[1] = 1.0;  plane2[2] = 0.0;  plane2[3] = 0.0001;
	plane3[0] = 0.0;  plane3[1] = -1.0; plane3[2] = 0.0;  plane3[3] = 1.0001;
	plane4[0] = 0.0;  plane4[1] = 0.0;  plane4[2] = 1.0;  plane4[3] = 0.0001;
	plane5[0] = 0.0;  plane5[1] = 0.0;  plane5[2] = -1.0; plane5[3] = 1.0001;

	for(unsigned int i=0; i<6; ++i){
		mCoeffRate[i] = rsRandf(0.1f) + 0.02f;
		mCoeffPhase[i] = rsRandf(RS_PIx2) - RS_PI;
		// test constants
		//mCoeffRate[i] = 0.01f * float(i) + 0.01f;
		//mCoeffPhase[i] = 0.0f;
	}
}


void MirrorBox::update(float frametime){
	for(unsigned int i=0; i<6; ++i){
		mCoeffPhase[i] += mCoeffRate[i] * frametime;
		if(mCoeffPhase[i] > RS_PI)
			mCoeffPhase[i] -= RS_PIx2;
		mCoeff[i] = cosf(mCoeffPhase[i]);
	}

	mMatrix.makeTranslate(-0.5f, -0.5f, -0.5f);
	mMatrix.rotate(mCoeff[0] * 3.0f, 1.0f, 0.0f, 0.0f);
	mMatrix.rotate(mCoeff[1] * 3.0f, 0.0f, 1.0f, 0.0f);
	mMatrix.rotate(mCoeff[2] * 3.0f, 0.0f, 0.0f, 1.0f);
	mMatrix.translate(0.5f * mCoeff[3] + 0.5f, 0.5f * mCoeff[4] + 0.5f, 0.5f * mCoeff[5] + 0.5f);
	//mMatrix.translate( 0.5f,  0.5f,  0.5f);

	// test code for keeping objects centered
	mMatrix.identity();
}


void MirrorBox::draw(const float& x, const float& y, const float& z, const float& eyex, const float& eyey, const float& eyez, rsCamera gCamera, impSurface *drawSurface0, impSurface *drawSurface1, impSurface *drawSurface2){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, z);

	if(gCamera.inViewVolume(rsVec(x+0.5f, y+0.5f, z+0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CCW);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex+0.5f, eyey+0.5f, eyez+0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x+0.5f, y+0.5f, z-0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CW);
			glScalef(1.0f, 1.0f, -1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex+0.5f, eyey+0.5f, eyez-0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x+0.5f, y-0.5f, z+0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CW);
			glScalef(1.0f, -1.0f, 1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex+0.5f, eyey-0.5f, eyez+0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x+0.5f, y-0.5f, z-0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CCW);
			glScalef(1.0f, -1.0f, -1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex+0.5f, eyey-0.5f, eyez-0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x-0.5f, y+0.5f, z+0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CW);
			glScalef(-1.0f, 1.0f, 1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex-0.5f, eyey+0.5f, eyez+0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x-0.5f, y+0.5f, z-0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CCW);
			glScalef(-1.0f, 1.0f, -1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex-0.5f, eyey+0.5f, eyez-0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x-0.5f, y-0.5f, z+0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CCW);
			glScalef(-1.0f, -1.0f, 1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex-0.5f, eyey-0.5f, eyez+0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	if(gCamera.inViewVolume(rsVec(x-0.5f, y-0.5f, z-0.5f), 0.866025f)){
		glPushMatrix();
			glFrontFace(GL_CW);
			glScalef(-1.0f, -1.0f, -1.0f);
			setClipPlanes();
			glMultMatrixf(mMatrix.get());
			drawSubBox(eyex-0.5f, eyey-0.5f, eyez-0.5f, drawSurface0, drawSurface1, drawSurface2);
		glPopMatrix();
	}

	glPopMatrix();

	glFrontFace(GL_CCW);
}


void MirrorBox::setClipPlanes(){
#if USE_CLIP_PLANES
	glClipPlane(GL_CLIP_PLANE0, plane0);
	glClipPlane(GL_CLIP_PLANE1, plane1);
	glClipPlane(GL_CLIP_PLANE2, plane2);
	glClipPlane(GL_CLIP_PLANE3, plane3);
	glClipPlane(GL_CLIP_PLANE4, plane4);
	glClipPlane(GL_CLIP_PLANE5, plane5);
#endif
}


void MirrorBox::drawSubBox(const float& eyex, const float& eyey, const float& eyez, impSurface *drawSurface0, impSurface *drawSurface1, impSurface *drawSurface2){
	glPushMatrix();
		glTranslatef(0.5f, 0.5f, 0.5f);
		const float dist_sq(eyex * eyex + eyey * eyey + eyez * eyez);
		if(dist_sq < 16.0f)
			drawSurface0->draw();
		else if(dist_sq < 36.0f)
			drawSurface1->draw();
		else
			drawSurface2->draw();
	glPopMatrix();

/*	srand(0);
	float x, y, z;
	glBegin(GL_TRIANGLES);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		x = rsRandf(1.0f);  y = rsRandf(1.0f);  z = rsRandf(1.0f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
		glVertex3f(x + rsRandf(0.6f) - 0.3f, y + rsRandf(0.4f) - 0.3f, z + rsRandf(0.6f) - 0.3f);
	glEnd();*/
}
