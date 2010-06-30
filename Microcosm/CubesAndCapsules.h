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


#ifndef CUBES_AND_CAPSULES_H
#define CUBES_AND_CAPSULES_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impRoundedHexahedron.h>
#include <Implicit/impCapsule.h>


class CubesAndCapsules : public Gizmo{
private:
	int mCount;
	float mDx;

public:
	CubesAndCapsules(int num, float dx, int gizmoSpeed) : Gizmo(gizmoSpeed){
		mCount = num;
		mDx = dx;

		for(int i=0; i<mCount; i++){
			impRoundedHexahedron* hexa = new impRoundedHexahedron;
			hexa->setThickness(0.03f);
			mShapes.push_back(hexa);
		}

		for(int i=0; i<mCount; i++){
			impCapsule* cap = new impCapsule;
			cap->setThickness(0.03f);
			mShapes.push_back(cap);
		}
	};

	~CubesAndCapsules(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;

		const float hoffset = 0.35f;
		const float size = 0.03f;
		float x = 0.0f;
		for(int i=0; i<mCount; i++){
			impRoundedHexahedron* hexa = static_cast<impRoundedHexahedron*>(mShapes[i]);
			m.makeRotate(cosf(mCoeffPhase[6] * 4.0f), 1.0f, 0.0f, 0.0f);
			m.rotate(cosf(mCoeffPhase[7] * 4.0f), 0.0f, 1.0f, 0.0f);
			m.translate(hoffset * cosf(mCoeffPhase[8] * 2.0f + x), hoffset * cosf(mCoeffPhase[9] * 3.0f + x), hoffset * cosf(mCoeffPhase[10] * 3.0f + x));
			m.postMult(mMatrix);
			hexa->setMatrix(m.get());
			hexa->setSize(size + size * cosf(mCoeffPhase[11] * 5.0f), size + size * cosf(mCoeffPhase[12] * 5.0f), size + size * cosf(mCoeffPhase[13] * 5.0f));
			x += mDx;
		}

		const float coffset = 0.32f;
		x = 0.0f;
		for(int i=0; i<mCount; i++){
			impCapsule* cap = static_cast<impCapsule*>(mShapes[i+mCount]);
			m.makeRotate(cosf(mCoeffPhase[14] * 4.0f), 1.0f, 0.0f, 0.0f);
			m.rotate(cosf(mCoeffPhase[15] * 4.0f), 0.0f, 1.0f, 0.0f);
			m.translate(coffset * cosf(mCoeffPhase[16] * 3.0f + x), coffset * cosf(mCoeffPhase[17] * 2.0f + x), coffset * cosf(mCoeffPhase[18] * 2.0f + x));
			m.postMult(mMatrix);
			cap->setMatrix(m.get());
			cap->setLength(0.07f + 0.07f * cosf(mCoeffPhase[19] * 5.0f));
			x += mDx;
		}
	}
};


#endif
