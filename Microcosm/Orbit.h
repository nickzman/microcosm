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


#ifndef ORBIT_H
#define ORBIT_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impTorus.h>



class Orbit : public Gizmo{
public:
	impTorus* torus1;
	impTorus* torus2;
	impTorus* torus3;

	Orbit(int gizmoSpeed) : Gizmo(gizmoSpeed){
		torus1 = new impTorus;
		torus1->setRadius(0.21f);
		torus1->setThickness(0.04f);
		mShapes.push_back(torus1);
		torus2 = new impTorus;
		torus2->setRadius(0.32f);
		torus2->setThickness(0.04f);
		mShapes.push_back(torus2);
		torus3 = new impTorus;
		torus3->setRadius(0.43f);
		torus3->setThickness(0.04f);
		mShapes.push_back(torus3);
	}

	~Orbit(){
		delete torus1;
		delete torus2;
		delete torus3;
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		m.makeRotate(mCoeff[9] * 8.0f, 1.0f, 0.0f, 0.0f);
		m.rotate(mCoeff[8] * 10.0f, 0.0f, 1.0f, 0.0f);
		m.postMult(mMatrix);
		torus1->setMatrix(m.get());

		m.makeRotate(mCoeff[7] * 6.0f, 0.0f, 1.0f, 0.0f);
		m.rotate(mCoeff[6] * 7.5f, 0.0f, 0.0f, 1.0f);
		m.postMult(mMatrix);
		torus2->setMatrix(m.get());

		m.makeRotate(mCoeff[5] * 4.0f, 0.0f, 0.0f, 1.0f);
		m.rotate(mCoeff[4] * 5.0f, 1.0f, 0.0f, 0.0f);
		m.postMult(mMatrix);
		torus3->setMatrix(m.get());
	}
};


#endif
