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


#ifndef CUBE_H
#define CUBE_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impSphere.h>
#include <Implicit/impCapsule.h>


class Cube : public Gizmo{
public:
	Cube(int gizmoSpeed) : Gizmo(gizmoSpeed){
		for(int i=0; i<3; i++){
			impSphere* sphere = new impSphere;
			sphere->setThickness(0.05f);
			mShapes.push_back(sphere);
		}

		for(int i=0; i<12; i++){
			impCapsule* cap = new impCapsule;
			cap->setThickness(0.03f);
			cap->setLength(0.2f);
			mShapes.push_back(cap);
		}
	}

	~Cube(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		const float offset = 0.3f;
		rsMatrix m;
		for(int i=0; i<3; i++){
			m.makeTranslate(offset * cosf(mCoeffPhase[i*2+1] * 5.0f), offset * cosf(mCoeffPhase[i*2+2] * 5.0f), offset * cosf(mCoeffPhase[i*2+3] * 5.0f));
			m.postMult(mMatrix);
			mShapes[i]->setPosition(&((m.get())[12]));
		}

		const float cubesize = 0.25f;
		impCapsule* cap = static_cast<impCapsule*>(mShapes[3]);
		m.makeTranslate(cubesize, cubesize, 0.0f);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());
		
		cap = static_cast<impCapsule*>(mShapes[4]);
		m.makeTranslate(-cubesize, cubesize, 0.0f);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[5]);
		m.makeTranslate(cubesize, -cubesize, 0.0f);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[6]);
		m.makeTranslate(-cubesize, -cubesize, 0.0f);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[7]);
		m.makeRotate(RS_PIo2, 1.0f, 0.0f, 0.0f);
		m.translate(cubesize, 0.0f, cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[8]);
		m.makeRotate(RS_PIo2, 1.0f, 0.0f, 0.0f);
		m.translate(-cubesize, 0.0f, cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[9]);
		m.makeRotate(RS_PIo2, 1.0f, 0.0f, 0.0f);
		m.translate(cubesize, 0.0f, -cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[10]);
		m.makeRotate(RS_PIo2, 1.0f, 0.0f, 0.0f);
		m.translate(-cubesize, 0.0f, -cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[11]);
		m.makeRotate(RS_PIo2, 0.0f, 1.0f, 0.0f);
		m.translate(0.0f, cubesize, cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[12]);
		m.makeRotate(RS_PIo2, 0.0f, 1.0f, 0.0f);
		m.translate(0.0f, -cubesize, cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[13]);
		m.makeRotate(RS_PIo2, 0.0f, 1.0f, 0.0f);
		m.translate(0.0f, cubesize, -cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());

		cap = static_cast<impCapsule*>(mShapes[14]);
		m.makeRotate(RS_PIo2, 0.0f, 1.0f, 0.0f);
		m.translate(0.0f, -cubesize, -cubesize);
		m.postMult(mMatrix);
		cap->setMatrix(m.get());
	}
};


#endif
