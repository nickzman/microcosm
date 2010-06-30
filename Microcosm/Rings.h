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


#ifndef RINGS_H
#define RINGS_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impRoundedHexahedron.h>
#include <Implicit/impTorus.h>


class Rings : public Gizmo{
private:
	int mCount;

public:
	Rings(int count, int gizmoSpeed) : Gizmo(gizmoSpeed){
		mCount = count;
	
		for(int i=0; i<mCount; i++){
			impRoundedHexahedron* hexa = new impRoundedHexahedron;
			hexa->setThickness(0.035f);
			mShapes.push_back(hexa);
		}

		impTorus* torus = new impTorus;
		torus->setRadius(0.4f);
		torus->setThickness(0.04f);
		mShapes.push_back(torus);
	}

	~Rings(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		const float size = 0.06f;
		rsMatrix m;
		for(int i=0; i<mCount; i++){
			impRoundedHexahedron* hexa = static_cast<impRoundedHexahedron*>(mShapes[i]);
			m.identity();
			m[1] = 0.5f * cosf(mCoeffPhase[0] * 5.0f);
			m[2] = 0.5f * cosf(mCoeffPhase[1] * 5.0f);
			m[4] = 0.5f * cosf(mCoeffPhase[2] * 5.0f);
			m.translate(0.32f, 0.0f, 0.0f);
			m.rotate(RS_PIx2 / float(mCount) * float(i), 0.0f, 0.0f, 1.0f);
			m.rotate(mCoeff[3] * 4.0f, 0.0f, 0.0f, 1.0f);
			m.postMult(mMatrix);
			hexa->setMatrix(m.get());
			hexa->setSize(size, size, size);
		}

		m.makeRotate(mCoeff[10] * 5.0f, 1.0f, 0.0f, 0.0f);
		m.rotate(mCoeff[11] * 5.0f, 0.0f, 1.0f, 0.0f);
		m.postMult(mMatrix);
		impTorus* torus = static_cast<impTorus*>(mShapes[mCount]);
		torus->setMatrix(m.get());
		torus->setRadius(0.3f + 0.1f * mCoeff[12]);
	}
};


#endif
