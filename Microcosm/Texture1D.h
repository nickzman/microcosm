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

#ifndef TEXTURE1D_H
#define TEXTURE1D_H


#include <rsMath/rsMath.h>
#include <OpenGL/gl.h>


#define TEX_SIZE 128
#define NUM_COLORS 4
#define NUM_TEX_COEFF (6 * NUM_COLORS)


class Texture1D{
private:
	GLuint mTexId;
	unsigned char mData[TEX_SIZE * 4];

	rsVec4 mColor[NUM_COLORS];

	// A rate of change for each coefficient
	float mCoeffRate[NUM_TEX_COEFF];
	// A phase for each coefficient
	float mCoeffPhase[NUM_TEX_COEFF];
	// a coefficient for each color's r, g, b, a, and position
	float mCoeff[NUM_TEX_COEFF];

public:
	Texture1D(int colorSpeed){ init(colorSpeed); }
	~Texture1D(){}

	void init(int colorSpeed);
	void update(float frameTime);
	void bind();

private:
	const float& minimum(const float& a, const float& b){ return (a<b) ? a : b; }
};


#endif
