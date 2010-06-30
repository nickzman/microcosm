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


#include <Microcosm/Gizmo.h>


using namespace std;


//extern int gMode;
//extern int dGizmoSpeed;


Gizmo::Gizmo(int gizmoSpeed){
	mComputeComplexity = 1.0f;

	// Most Gizmos will stay partially inside the impCubeVolume of width 1.0 most of
	// the time using this nominal diplacement value
	mMaxDisplacement = 0.5f;

	mScale = 1.0f;

	for(unsigned int i=0; i<GIZMO_NUM_COEFFS; ++i){
		mCoeffRate[i] = rsRandf(0.01f * float(gizmoSpeed)) + 0.005f * float(gizmoSpeed);
		mCoeffPhase[i] = rsRandf(RS_PIx2) - RS_PI;
		// test constants
		//mCoeffRate[i] = 0.02f * float(i) + 0.02f;
		//mCoeffPhase[i] = 0.0f;
	}
}


Gizmo::~Gizmo(){
	mShapes.clear();
}


void Gizmo::setScale(float s){
	mScale = s;
}


float Gizmo::value(float* position){
	float value(0.0f);
	for(unsigned int i=0; i<mShapes.size(); ++i)
		value += mShapes[i]->value(position);
	return value;
}


void Gizmo::getShapes(ShapeVector &sv){
	for(unsigned int i=0; i<mShapes.size(); ++i)
		sv.push_back(mShapes[i]);
}


void Gizmo::addCrawlPoints(impCrawlPointVector &cpv){
	for(unsigned int i=0; i<mShapes.size(); ++i)
		mShapes[i]->addCrawlPoint(cpv);
}


void Gizmo::updateConstants(float frametime){
	for(unsigned int i=0; i<GIZMO_NUM_COEFFS; ++i){
		mCoeffPhase[i] += mCoeffRate[i] * frametime;
		if(mCoeffPhase[i] > RS_PI)
			mCoeffPhase[i] -= RS_PIx2;
		mCoeff[i] = cosf(mCoeffPhase[i]);
	}
}


void Gizmo::updateMatrix(int mode){
	if(mode == 0){
		// Gizmo stays in center, no drifting to sides of impVolume and no rotating
		mMatrix.makeScale(mScale);
	}
	else{
		mMatrix.makeRotate(mCoeff[0] * 2.0f, 1.0f, 0.0f, 0.0f);
		mMatrix.rotate(mCoeff[1] * 2.0f, 0.0f, 1.0f, 0.0f);
		mMatrix.rotate(mCoeff[2] * 2.0f, 0.0f, 0.0f, 1.0f);
		mMatrix.scale(mScale);
		mMatrix.translate(mCoeff[3] * mMaxDisplacement, mCoeff[4] * mMaxDisplacement, mCoeff[5] * mMaxDisplacement);
	}
}


void Gizmo::update(float frametime, int mode){
	updateConstants(frametime);
	updateMatrix(mode);

	for(unsigned int i=0; i<mShapes.size(); ++i)
		mShapes[i]->setMatrix(mMatrix.get());
}
