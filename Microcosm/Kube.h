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


#ifndef KUBE_H
#define KUBE_H


#include <Microcosm/Gizmo.h>
#include <Implicit/impRoundedHexahedron.h>
#include <Implicit/impSphere.h>


class Kube : public Gizmo{
public:
	Kube(int gizmoSpeed) : Gizmo(gizmoSpeed){
		const float size = 0.06f;
		for(int i=0; i<9; i++){
			impRoundedHexahedron* hexa = new impRoundedHexahedron;
			hexa->setThickness(0.03f);
			hexa->setSize(size, size, size);
			mShapes.push_back(hexa);
		}

		impSphere* sphere = new impSphere;
		mShapes.push_back(sphere);
	}

	~Kube(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		const float offset = 0.18f;
		rsMatrix m;
		impRoundedHexahedron* hexa;
		float shift[10];
		for(int i=0; i<10; i++){
			shift[i] = mCoeff[i] * 5.0f;
			if(shift[i] < -1.0f)
				shift[i] = -1.0f;
			if(shift[i] > 1.0f)
				shift[i] = 1.0f;
			// smooth the motion
			if(shift[i] < 0.0f){
				const float x = shift[i] + 1.0f;
				shift[i] = (x * x) - 1.0f;
			}
			else{
				const float x = 1.0f - shift[i];
				shift[i] = 1.0f - (x * x);
			}
		}
		for(int i=0; i<8; i++){
			hexa = static_cast<impRoundedHexahedron*>(mShapes[i]);
			m.makeRotate(shift[0] * RS_PIo2, 1.0f, 0.0f, 0.0f);
			m.rotate(shift[1] * RS_PIo2, 0.0f, 1.0f, 0.0f);
			m.rotate(shift[2] * RS_PIo2, 0.0f, 0.0f, 1.0f);
			//m.rotate(shift[9] * 2.094395f, 1.0f, 1.0f, 1.0f);
			const float x = (i >= 4) ? offset * shift[3] : -offset * shift[3];
			const float y = ((i / 2) % 2) ? offset * shift[4] : -offset * shift[4];
			const float z = (i % 2) ? offset * shift[5] : -offset * shift[5];
			m.translate(x, y, z);
			m.rotate(shift[6] * RS_PIo2, 1.0f, 0.0f, 0.0f);
			m.rotate(shift[7] * RS_PIo2, 0.0f, 1.0f, 0.0f);
			m.rotate(shift[8] * RS_PIo2, 0.0f, 0.0f, 1.0f);
			m.postMult(mMatrix);
			hexa->setMatrix(m.get());
		}
		hexa = static_cast<impRoundedHexahedron*>(mShapes[8]);
		m.identity();
		m.postMult(mMatrix);
		hexa->setMatrix(mMatrix.get());

		const float sphereoffset = 0.35f;
		impSphere* sphere = static_cast<impSphere*>(mShapes[9]);
		m.makeTranslate(sphereoffset * sinf(mCoeffPhase[10] * 4.0f), sphereoffset * sinf(mCoeffPhase[11] * 4.0f), sphereoffset * sinf(mCoeffPhase[12] * 4.0f));
		m.postMult(mMatrix);
		sphere->setPosition(&((m.get())[12]));
		sphere->setThickness(0.05f + 0.01f * mCoeff[3]);
	}
};


#endif
