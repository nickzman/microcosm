/*
 * Copyright (C) 2010  Terence M. Welsh
 *
 * This file is part of Implicit.
 *
 * Implicit is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Implicit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <Implicit/impCapsule.h>


float impCapsule::value(float* position){
	const float& x(position[0]);
	const float& y(position[1]);
	const float& z(position[2]);

	const float tx(x * invtrmat[0] + y * invtrmat[1] + z * invtrmat[2] + invtrmat[3]);
	const float ty(x * invtrmat[4] + y * invtrmat[5] + z * invtrmat[6] + invtrmat[7]);
	const float tz(x * invtrmat[8] + y * invtrmat[9] + z * invtrmat[10] + invtrmat[11]);

	// Compute shrunken value.
	// Use 0.001 instead of 0.0 to avoid divide-by-zero.
	const float zz(fabsf(tz) - length);
	//const float sz((zz < 0.001f) ? 0.001f : zz);
	// rewritten with fewer conditionals
	const float sz(zz * (zz > 0.0f));

	return thicknessSquared / (tx*tx + ty*ty + sz*sz + IMP_MIN_DIVISOR);
}
