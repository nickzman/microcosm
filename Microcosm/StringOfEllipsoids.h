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


#ifndef STRING_OF_ELLIPSOIDS_H
#define STRING_OF_ELLIPSOIDS_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impEllipsoid.h>



class StringOfEllipsoids : public Gizmo{
public:
	unsigned int numEllipsoids;
	float mOffset;

	StringOfEllipsoids(unsigned int num, float offset, int gizmoSpeed) : Gizmo(gizmoSpeed){
		numEllipsoids = num >= 1 ? num : 1;  // at least 1
		for(unsigned int i=0; i<numEllipsoids; ++i){
			impEllipsoid* e = new impEllipsoid;
			e->setThickness(0.04f);
			mShapes.push_back(e);
		}
		mOffset = offset;
	}

	~StringOfEllipsoids(){
		for(unsigned int i=0; i<mShapes.size(); ++i)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		float n(0.0f);
		for(unsigned int i=0; i<numEllipsoids; ++i){
			const float mult(0.3f);
			m.makeScale(cosf(mCoeffPhase[7]*4.0f+n) * 0.4f + 1.2f, cosf(mCoeffPhase[8]*4.0f+n) * 0.4f + 1.2f, cosf(mCoeffPhase[9]*4.0f+n) * 0.4f + 1.2f);
			m.rotate(cosf(mCoeffPhase[10]*2.0f+n), 1.0f, 0.0f, 0.0f);
			m.rotate(cosf(mCoeffPhase[11]*2.0f+n), 0.0f, 1.0f, 0.0f);
			m.rotate(cosf(mCoeffPhase[12]*2.0f+n), 0.0f, 0.0f, 1.0f);
			m.translate(cosf(mCoeffPhase[13]*3.0f+n*(1.0f+0.5f*mCoeff[14])) * mult, cosf(mCoeffPhase[15]*3.0f+n*(1.0f+0.5f*mCoeff[16])) * mult, cosf(mCoeffPhase[17]*3.0f+n*(1.0f+0.5f*mCoeff[18])) * mult);
			m.postMult(mMatrix);
			mShapes[i]->setMatrix(m.get());
			n += mOffset;
		}
	}
};


#endif
