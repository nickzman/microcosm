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


#ifndef FLOWER_H
#define FLOWER_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impEllipsoid.h>



class Flower : public Gizmo{
public:
	unsigned int numEllipsoids;
	float mOffset;

	Flower(unsigned int num, float offset, int gizmoSpeed) : Gizmo(gizmoSpeed){
		numEllipsoids = num >= 1 ? num : 1;  // at least 1
		for(unsigned int i=0; i<numEllipsoids; ++i){
			impEllipsoid* e = new impEllipsoid;
			//e->setThickness(0.04f);
			mShapes.push_back(e);
		}
		mOffset = offset;
	}

	~Flower(){
		for(unsigned int i=0; i<mShapes.size(); ++i)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		float n = 0.0f;
		float x = 0.0f;
		const float dx(RS_PIx2 / float(numEllipsoids));
		for(unsigned int i=0; i<numEllipsoids; ++i){
			const float z = cosf(mCoeffPhase[0] * 3.0f + n * mCoeff[1]);
			m.makeScale(0.25f, 0.25f, 0.75f - 0.5f * fabsf(z));
			m.translate(0.0f, 0.0f, z * 0.45f);
			m.rotate(cosf(mCoeffPhase[2] + x), 1.0f, 0.0f, 0.0f);
			m.rotate(cosf(mCoeffPhase[3] + x), 0.0f, 1.0f, 0.0f);
			//m.rotate(cosf(mCoeffPhase[4] + x), 0.0f, 1.0f, 0.0f);
			m.postMult(mMatrix);
			mShapes[i]->setMatrix(m.get());
			n += mOffset;
			x += dx;
		}
	}
};


#endif
