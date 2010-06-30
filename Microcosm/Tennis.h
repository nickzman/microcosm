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


#ifndef TENNIS_H
#define TENNIS_H


#include <Microcosm/Gizmo.h>
#include <rsMath/rsMath.h>
#include <Implicit/impRoundedHexahedron.h>
#include <Implicit/impSphere.h>


//extern int dGizmoSpeed;


class Tennis : public Gizmo{
private:
	rsVec pos;
	rsVec dir;
	int dGizmoSpeed;

public:
	Tennis(int gizmoSpeed) : Gizmo(gizmoSpeed){
		mMaxDisplacement = 0.4f;
	
		pos.set(0.0f, 0.0f, 0.0f);
		dir.set(1.0f + rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
		dir.normalize();

		for(int i=0; i<2; i++){
			impRoundedHexahedron* hexa = new impRoundedHexahedron;
			hexa->setThickness(0.03f);
			hexa->setSize(0.0f, 0.1f, 0.1f);
			mShapes.push_back(hexa);
		}

		impSphere* sphere = new impSphere;
		sphere->setThickness(0.05f);
		mShapes.push_back(sphere);
		dGizmoSpeed = gizmoSpeed;
	}

	~Tennis(){
		for(unsigned int i=0; i<mShapes.size(); i++)
			delete mShapes[i];
	}

	void update(float frametime, int mode){
		updateConstants(frametime);
		updateMatrix(mode);

		rsMatrix m;
		const float soffset = 0.35f;
		pos += dir * (frametime * 0.05f * float(dGizmoSpeed));
		bool newdirection = false;
		if(pos[0] < -soffset){
			pos[0] += -soffset - pos[0];
			dir[0] *= -1.0f;
			newdirection = true;
		}
		else if(pos[0] > soffset){
			pos[0] += soffset - pos[0];
			dir[0] *= -1.0f;
			newdirection = true;
		}
		if(pos[1] < -soffset){
			pos[1] += -soffset - pos[1];
			dir[1] *= -1.0f;
		}
		else if(pos[1] > soffset){
			pos[1] += soffset - pos[1];
			dir[1] *= -1.0f;
		}
		if(pos[2] < -soffset){
			pos[2] += -soffset - pos[2];
			dir[2] *= -1.0f;
		}
		else if(pos[2] > soffset){
			pos[2] += soffset - pos[2];
			dir[2] *= -1.0f;
		}
		if(newdirection){
			dir[1] += rsRandf(0.5f) - 0.25f;
			dir[2] += rsRandf(0.5f) - 0.25f;
			dir.normalize();
			if(dir[0] > -0.3f && dir[0] < 0.3f){
				dir[0] *= 2.0f;
				dir.normalize();
			}
		}
		impSphere* sphere = static_cast<impSphere*>(mShapes[2]);
		m.makeTranslate(pos);
		m.postMult(mMatrix);
		sphere->setPosition(&((m.get())[12]));

		const float hoffset = 0.3f;
		impRoundedHexahedron* hexa = static_cast<impRoundedHexahedron*>(mShapes[0]);
		rsVec driftpos(-0.43f, hoffset * cosf(mCoeffPhase[0] * 6.0f), hoffset * cosf(mCoeffPhase[1] * 6.0f));
		rsVec hitpos(-0.43f, pos[1], pos[2]);
		float t = -pos[0] / soffset;
		if(t < 0.0f)
			t = 0.0f;
		if(t > 1.0f)
			t = 1.0f;
		m.makeTranslate(driftpos * (1.0f - t) + hitpos * t);
		m.postMult(mMatrix);
		hexa->setMatrix(m.get());

		hexa = static_cast<impRoundedHexahedron*>(mShapes[1]);
		driftpos.set(0.43f, hoffset * cosf(mCoeffPhase[0] * 6.0f), hoffset * cosf(mCoeffPhase[1] * 6.0f));
		hitpos.set(0.43f, pos[1], pos[2]);
		t = pos[0] / soffset;
		if(t < 0.0f)
			t = 0.0f;
		if(t > 1.0f)
			t = 1.0f;
		m.makeTranslate(driftpos * (1.0f - t) + hitpos * t);
		m.postMult(mMatrix);
		hexa->setMatrix(m.get());
	}
};


#endif
