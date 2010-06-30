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


#ifndef RING_OF_TORI_H
#define RING_OF_TORI_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impTorus.h>



class RingOfTori : public Gizmo{
public:
	unsigned int numTori;
	impTorus** tori;

	RingOfTori(unsigned int num, int gizmoSpeed) : Gizmo(gizmoSpeed){
		mMaxDisplacement = 0.5f;

		numTori = num >= 2 ? num : 2;  // at least 2
		tori = new impTorus*[numTori];
		for(unsigned int i=0; i<numTori; ++i){
			tori[i] = new impTorus;
			tori[i]->setRadius(0.18f);
			tori[i]->setThickness(0.04f);
			mShapes.push_back(tori[i]);
		}
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		float x(0.0f);
		const float dx(RS_PIx2 / float(numTori));
		for(unsigned int i=0; i<numTori; ++i){
			m.makeScale(0.75f + 0.25f * cosf(mCoeffPhase[6] * 3.0f), 1.0f, 0.85f);
			m.rotate(mCoeffPhase[7] * 3.0f, 0.0f, 1.0f, 0.0f);
			m.translate(0.26f * cosf(mCoeffPhase[8] * 3.0f), 0.0f, 0.0f);
			m.rotate(mCoeffPhase[9] * 3.0f/* + x*/, 1.0f, 0.0f, 0.0f);
			m.rotate(x, 0.0f, 0.0f, 1.0f);
			m.postMult(mMatrix);
			tori[i]->setMatrix(m.get());
			x += dx;
		}
	}
};


#endif
