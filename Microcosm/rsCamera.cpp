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
#include <Microcosm/rsCamera.h>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>


rsCamera::rsCamera(){
	setProjectionMatrix(RS_PIo2, 1.0f, 1.0f, 100.0f);
	mViewMat.identity();
}


void rsCamera::setProjectionMatrix(float fovy, float aspectratio, float nearclip, float farclip){
	mNear = nearclip;
	mFar = farclip;

	const float A(tanf(fovy*0.5f));
	float projmat[] = {1.0f / (aspectratio * A), 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / A, 0.0f, 0.0f,
		0.0f, 0.0f, -(mNear + mFar) / (mFar - mNear), -1.0f,
		0.0f, 0.0f, (-2.0f * mNear * mFar) / (mFar - mNear), 0.0f};
	mProjMat.set(projmat);

	// left and right planes' vectors
	{
		const float temp(atanf(aspectratio * A));
		const float cos_temp(cosf(temp));
		const float sin_temp(sinf(temp));
		cullVec[0][0] = cos_temp;
		cullVec[0][1] = 0.0f;
		cullVec[0][2] = -sin_temp;
		cullVec[1][0] = -cos_temp;
		cullVec[1][1] = 0.0f;
		cullVec[1][2] = -sin_temp;
	}

	// bottom and top planes' vectors
	{
		const float temp(fovy * 0.5f);
		const float cos_temp(cosf(temp));
		const float sin_temp(sinf(temp));
		cullVec[2][0] = 0.0f;
		cullVec[2][1] = cos_temp;
		cullVec[2][2] = -sin_temp;
		cullVec[3][0] = 0.0f;
		cullVec[3][1] = -cos_temp;
		cullVec[3][2] = -sin_temp;
	}
}



void rsCamera::apply(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(mProjMat.get());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(mViewMat.get());
}


void rsCamera::revoke(){
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


bool rsCamera::inViewVolume(rsVec pos, float radius){
	pos.transPoint(mViewMat);

	// check far plane
	if(pos[2] < -(mFar + radius))
		return 0;
	// check left plane
	if(pos[0]*cullVec[0][0] + pos[1]*cullVec[0][1] + pos[2]*cullVec[0][2] < -radius)
		return 0;
	// check right plane
	if(pos[0]*cullVec[1][0] + pos[1]*cullVec[1][1] + pos[2]*cullVec[1][2] < -radius)
		return 0;
	// check bottom plane
	if(pos[0]*cullVec[2][0] + pos[1]*cullVec[2][1] + pos[2]*cullVec[2][2] < -radius)
		return 0;
	// check top plane
	if(pos[0]*cullVec[3][0] + pos[1]*cullVec[3][1] + pos[2]*cullVec[3][2] < -radius)
		return 0;

	return 1;
}
