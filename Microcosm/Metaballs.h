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


#ifndef METABALLS_H
#define METABALLS_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impSphere.h>


using namespace std;


class Metaballs : public Gizmo{
public:
	Metaballs(unsigned int num, int gizmoSpeed) : Gizmo(gizmoSpeed){
		mMaxDisplacement = 0.4f;

		unsigned int n = num >= 1 ? num : 1;  // at least 1
		for(unsigned int i=0; i<n; ++i){
			impSphere* sphere = new impSphere;
			sphere->setThickness(0.06f);
			mShapes.push_back(sphere);
		}
	}

	~Metaballs(){}

	void setScale(float s){
		mScale = s;

		// Must set thickness because impSphere ignores the scale portion of
		// its matrix for efficiency.
		for(unsigned int i=0; i<mShapes.size(); ++i)
			mShapes[i]->setThickness(0.2f * mScale);
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		float x(0.0f);
		const float dx(RS_PIx2 / float(mShapes.size()));
		//const float offset(0.3f);
		for(unsigned int i=0; i<mShapes.size(); ++i){
			m.makeTranslate(0.0f, 0.27f * mCoeff[7], 0.27f * mCoeff[8]);
			m.rotate((mCoeffPhase[9] * 2.0f + x), 1.0f, 0.0f, 0.0f);
			m.rotate((mCoeffPhase[10] * 3.0f + x), 0.0f, 1.0f, 1.0f);
			m.postMult(mMatrix);
			//mShapes[i]->setMatrix(m.get());  // Why doesn't this work?
			mShapes[i]->setPosition(&((m.get())[12]));
			x += dx;
		}
	}
};


#endif
