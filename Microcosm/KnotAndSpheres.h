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


#ifndef KNOT_AND_SPHERES_H
#define KNOT_AND_SPHERES_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impKnot.h>
#include <Implicit/impSphere.h>



class KnotAndSpheres : public Gizmo{
public:
	KnotAndSpheres(int coils, int twists, int spheres, int gizmoSpeed) : Gizmo(gizmoSpeed){
		impKnot* knot = new impKnot;
		knot->setRadius1(0.28f);
		knot->setRadius2(0.14f);
		knot->setNumCoils(coils);
		knot->setNumTwists(twists);
		knot->setThickness(0.04f);
		mShapes.push_back(knot);
		for(int i=0; i<spheres; i++){
			impSphere* sphere = new impSphere;
			sphere->setThickness(0.06f);
			mShapes.push_back(sphere);
		}
	
		setScale(1.0f);
	};

	~KnotAndSpheres(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void setScale(float s){
		mScale = s;

		// Must set thickness because impSphere ignores the scale portion of its matrix for efficiency.
		for(unsigned int i=1; i<mShapes.size(); ++i)
			mShapes[i]->setThickness(0.06f * mScale);
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		impKnot* knot = static_cast<impKnot*>(mShapes[0]);
		knot->setMatrix(mMatrix.get());
		const float fade0 = mCoeff[6] * 0.5f + 0.5f;
		const float fade1 = fade0 * (cosf(mCoeffPhase[7] * 5.0f) * 0.5f + 0.5f);
		knot->setRadius1(0.28f + 0.1f * fade1);
		knot->setRadius2(0.14f * (1.0f - fade1));

		rsMatrix m;
		const float mult = 0.35f;
		for(unsigned int i=1; i<mShapes.size(); i++){
			m.makeTranslate(mult * cosf(mCoeffPhase[7+i] * 5.0f), mult * cosf(mCoeffPhase[8+i] * 5.0f), mult * cosf(mCoeffPhase[9+i] * 5.0f));
			m.postMult(mMatrix);
			mShapes[i]->setPosition(&((m.get())[12]));
		}
	}
};


#endif
