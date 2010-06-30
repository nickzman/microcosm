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


#ifndef GIZMO_H
#define GIZMO_H


// A Gizmo is a collection of impShapes that interact with one another


#include <Implicit/impShape.h>
#include <rsMath/rsMath.h>
#include <vector>


#define GIZMO_NUM_COEFFS 25


typedef std::vector <impShape*> ShapeVector;


class Gizmo{
protected:
	ShapeVector mShapes;
	// to be set to compute time relative to other Gizmos
	float mComputeComplexity;

	// Amount to displace Gizmo when scale = 1.0
	float mMaxDisplacement;

	// scale factor
	float mScale;

	// Stuff to make Gizmo wander around
	float mCoeffRate[GIZMO_NUM_COEFFS];  // coefficients' rates
	float mCoeffPhase[GIZMO_NUM_COEFFS];  // coefficients' phases
	float mCoeff[GIZMO_NUM_COEFFS];  // coefficients that oscillate within {-1.0, 1.0}
	rsMatrix mMatrix;

	void updateConstants(float frametime);
	void updateMatrix(int mode);

public:
	Gizmo(int gizmoSpeed);
	~Gizmo();

	virtual void setScale(float s);

	// returns the value of this group of shapes at a given position
	// "position" is an array of 3 floats
	float value(float* position);

	// Add Gizmo's shapes to global vector of shapes
	// so that all shapes can be cycled through quickly.
	void getShapes(ShapeVector &sv);

	// Adds surface crawler start position(s) to given crawlPointVector
	void addCrawlPoints(impCrawlPointVector &cpv);
	virtual void update(float frametime, int mode);
};



#endif
