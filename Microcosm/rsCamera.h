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


#ifndef RSCAMERA_H
#define RSCAMERA_H



#include <rsMath/rsMath.h>



class rsCamera{
public:
	rsMatrix mProjMat;
	rsMatrix mViewMat;
	float mNear, mFar;
	float cullVec[4][3];  // vectors perpendicular to viewing volume planes

	rsCamera();
	~rsCamera(){}
	void setProjectionMatrix(float fovy, float aspectratio, float nearclip, float farclip);
	void setViewMatrix(float* mat){ mViewMat.set(mat); }
	void setViewMatrix(rsMatrix &mat){ mViewMat = mat; }

	// Test to see if a sphere is partially or wholly contained in view volume
	bool inViewVolume(rsVec pos, float radius);

	// Apply projection and modelview matrices with glPushMatrix()es
	void apply();

	// glPopMatrix()es, to be called sometime after apply()
	void revoke();
};



#endif
