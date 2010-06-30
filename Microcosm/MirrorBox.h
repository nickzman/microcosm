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

#ifndef MIRROR_BOX_H
#define MIRROR_BOX_H


#include <rsMath/rsMath.h>


#define USE_CLIP_PLANES 0

class rsCamera;
class impSurface;

// MirrorBox renders the same unit cube 8 times, reflecting it about the x, y, and z axes.
// So the whole MirrorBox has extents {(-1,-1,-1), (1,1,1)}.


class MirrorBox{
private:
	double plane0[4];
	double plane1[4];
	double plane2[4];
	double plane3[4];
	double plane4[4];
	double plane5[4];

	// Coefficients for rotating and translating contents of MirrorBox
	float mCoeffRate[6];  // coefficients' rates
	float mCoeffPhase[6];  // coefficients' phases
	float mCoeff[6];  // coefficients that oscillate within {-1.0, 1.0}

	rsMatrix mMatrix;

public:
	MirrorBox();
	~MirrorBox(){}
	void update(float frametime);
	// Pass center of MirrorBox and vector from eye to center.
	void draw(const float& x, const float& y, const float& z, const float& eyex, const float& eyey, const float& eyez, rsCamera gCamera, impSurface *drawSurface0, impSurface *drawSurface1, impSurface *drawSurface2);

private:
	// Pass vector from eye to center of this sub-box.  There are 8 sub-boxes.
	void drawSubBox(const float& eyex, const float& eyey, const float& eyez, impSurface *drawSurface0, impSurface *drawSurface1, impSurface *drawSurface2);
	inline void setClipPlanes();
};



#endif
