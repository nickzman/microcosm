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


#ifndef UFO_H
#define UFO_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impEllipsoid.h>
#include <Implicit/impTorus.h>


class UFO : public Gizmo{
public:
	unsigned int numEllipsoids;

	UFO(unsigned int num, int gizmoSpeed) : Gizmo(gizmoSpeed){
		numEllipsoids = num >= 1 ? num : 1;  // at least 1
		for(unsigned int i=0; i<numEllipsoids; ++i){
			impEllipsoid* e = new impEllipsoid;
			e->setThickness(0.04f);
			mShapes.push_back(e);
		}

		impTorus* torus = new impTorus;
		torus->setRadius(0.43f);
		torus->setThickness(0.04f);
		mShapes.push_back(torus);
	}

	~UFO(){
		for(unsigned int i=0; i<mShapes.size(); ++i)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		const float offset = 0.35f;
		for(unsigned int i=0; i<numEllipsoids; ++i){
			const float phase = RS_PIx2 / float(numEllipsoids) * float(i);
			float ripple0 = (cosf(mCoeffPhase[6] * 10.0f + phase) + 1.0f) * 0.5f;
			ripple0 *= ripple0;
			ripple0 *= ripple0;
			ripple0 *= ripple0;
			float ripple1 = (cosf(mCoeffPhase[6] * 10.0f + phase + (RS_PI * 0.25f)) + 1.0f) * 0.5f;
			ripple1 *= ripple1;
			ripple1 *= ripple1;
			ripple1 *= ripple1;
			m.makeRotate(mCoeff[8], 1.0f, 0.0f, 0.0f);
			m.rotate(mCoeff[9], 0.0f, 1.0f, 0.0f);
			m.scale(1.0f + ripple1 - (ripple0 * 0.5f), 1.0f + ripple0 - (ripple1 * 0.5f), 1.0f + ripple1 - (ripple0 * 0.5f));
			m.rotate(mCoeff[10], 1.0f, 0.0f, 0.0f);
			m.rotate(mCoeff[11], 0.0f, 0.0f, 1.0f);
			m.translate(offset * (0.75f + 0.25f * cosf(mCoeffPhase[12] * 3.0f)), 0.0f, 0.0f);
			m.rotate(phase, 0.0f, 0.0f, 1.0f);
			m.postMult(mMatrix);
			mShapes[i]->setMatrix(m.get());
		}

		m.makeScale(0.75f + 0.25f * cosf(mCoeffPhase[13] * 3.0f), 1.0f, 1.0f);
		m.scale(1.0f, 0.75f + 0.25f * cosf(mCoeffPhase[14] * 4.0f), 1.0f);
		m.rotate(mCoeff[15] * 3.0f, 0.0f, 0.0f, 1.0f);
		m.rotate(mCoeff[16] * 4.0f, 0.0f, 1.0f, 0.0f);
		m.rotate(mCoeff[17] * 5.0f, 1.0f, 0.0f, 0.0f);
		m.postMult(mMatrix);
		mShapes[numEllipsoids]->setMatrix(m.get());
	}
};


#endif
