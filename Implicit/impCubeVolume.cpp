/*
 * Copyright (C) 2001-2010  Terence M. Welsh
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


#include <Implicit/impCubeVolume.h>
#include <rsMath/rsMath.h>
#include <iostream>



impCubeVolume::impCubeVolume(){
	int i, j;
	impCubeTables cubeTables;
	for(i=0; i<256; ++i){
		for(j=0; j<17; ++j)
			triStripPatterns[i][j] = cubeTables.triStripPatterns[i][j];
		for(j=0; j<6; ++j)
			crawlDirections[i][j] = cubeTables.crawlDirections[i][j];
	}

	surface = new impSurface;
	init(4, 4, 4, 0.2f);
	surfacevalue = 0.5f;

	fastnormals = true;
	crawlfromsides = false;
}


impCubeVolume::~impCubeVolume(){
	cubes.clear();
	cubeIndices.clear();
	sortableCubes.clear();
}


void impCubeVolume::init(unsigned int width, unsigned int height, unsigned int length, float cw){
	unsigned int i, j, k;
    
	cubes.clear();

	// frequently used values
	w = width;
	w_1 = width + 1;
	h = height;
	h_1 = height + 1;
	l = length;
	l_1 = length + 1;
	w_1xh_1 = w_1 * h_1;
	w_1xh_1xl_1 = w_1 * h_1 * l_1;

	// calculate position of left-bottom-front corner
	cubewidth = cw;
	lbf[0] = -float(w) * cubewidth * 0.5f;
	lbf[1] = -float(h) * cubewidth * 0.5f;
	lbf[2] = -float(l) * cubewidth * 0.5f;

	// allocate cubedata memory and set cube positions
	cubes.resize(w_1xh_1xl_1);
	for(i=0; i<w_1; ++i){
		for(j=0; j<h_1; ++j){
			for(k=0; k<l_1; ++k){
				const unsigned int index(cubeindex(i,j,k));
				cubes[index].x = lbf[0] + (cubewidth * float(i));
				cubes[index].y = lbf[1] + (cubewidth * float(j));
				cubes[index].z = lbf[2] + (cubewidth * float(k));
#ifdef __SSE__
				cubes[index].w = 1.0f;
#endif
				cubes[index].cube_frame = 0;
				cubes[index].corner_frame = 0;
				cubes[index].x_vertex_frame = 0;
				cubes[index].y_vertex_frame = 0;
				cubes[index].z_vertex_frame = 0;
			}
		}
	}
}


void impCubeVolume::makeSurface(MicrocosmSaverSettings *inSettings){
	unsigned int i, j, k;

	frame ++;

	surface->reset();
	
	// find gradient value at every corner
	for(i=0; i<=w; ++i){
		for(j=0; j<=h; ++j){
			for(k=0; k<=l; ++k){
				const unsigned int ci(cubeindex(i,j,k));
				cubes[ci].value = function(&(cubes[ci].x), inSettings);
			}
		}
	}

	// polygonize surface
	currentVertexIndex = 0;
	for(i=0; i<w; ++i){
		for(j=0; j<h; ++j){
			for(k=0; k<l; ++k){
				const unsigned int ci(cubeindex(i,j,k));
				cubes[ci].mask = calculateCornerMask(i, j, k);
				polygonize(ci, inSettings);
			}
		}
	}
}


void impCubeVolume::makeSurface(float eyex, float eyey, float eyez, MicrocosmSaverSettings *inSettings){
	unsigned int i, j, k;

	frame ++;

	surface->reset();

	// find gradient value at every corner
	for(i=0; i<=w; ++i){
		for(j=0; j<=h; ++j){
			for(k=0; k<=l; ++k){
				const unsigned int ci(cubeindex(i,j,k));
				cubes[ci].value = function(&(cubes[ci].x), inSettings);
			}
		}
	}

	// erase list from last frame
	sortableCubes.clear();

	// collect list of cubes
	sortableCube* sc;
	unsigned int mask;
	for(i=0; i<w; ++i){
		for(j=0; j<h; ++j){
			for(k=0; k<l; ++k){
				const unsigned int ci(cubeindex(i,j,k));
				mask = calculateCornerMask(i, j, k);
				if(mask != 0 && mask != 255){
					cubes[ci].mask = mask;
					sortableCubes.push_back(sortableCube(ci));
					sc = &(sortableCubes.back());
					const float xdist(cubes[ci].x - eyex);
					const float ydist(cubes[ci].y - eyey);
					const float zdist(cubes[ci].z - eyez);
					sc->depth = xdist * xdist + ydist * ydist + zdist * zdist;
				}
			}
		}
	}

	// sort list of cubes
	sortableCubes.sort();

	// polygonize
	currentVertexIndex = 0;
	for(std::list<sortableCube>::iterator c = sortableCubes.begin(); c != sortableCubes.end(); ++c)
		polygonize(c->index, inSettings);
}


void impCubeVolume::makeSurface(impCrawlPointVector &cpv, MicrocosmSaverSettings *inSettings){
	unsigned int i, j, k;
	bool crawlpointexit;
	unsigned int mask;

	frame ++;

	surface->reset();

	currentCubeIndex = 0;

	// crawl from every crawl point to create the surface
	for(unsigned int cp=0; cp<cpv.size(); ++cp){
		// find cube corresponding to crawl point
		i = int((cpv[cp].position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= int(w))
			i = int(w) - 1;
		j = int((cpv[cp].position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= int(h))
			j = int(h) - 1;
		k = int((cpv[cp].position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= int(l))
			k = int(l) - 1;

		// escape if starting on a finished cube
		crawlpointexit = false;
		while(!crawlpointexit){
			const unsigned int ci(cubeindex(i,j,k));
			if(cubes[ci].cube_frame == frame)
				crawlpointexit = true;  // escape if starting on a finished cube
			else{  // find index for this cube
				findcornervalues(i, j, k, inSettings);
				mask = calculateCornerMask(i, j, k);
				// save index for polygonizing
				cubes[ci].mask = mask;
				if(mask == 255)  // escape if outside surface
					crawlpointexit = true;
				else{
					if(mask == 0){  // this cube is inside volume
						cubes[ci].cube_frame = frame;
						++i;  // step to an adjacent cube and start over
						if(i >= w)  // escape if you step outside of volume
							crawlpointexit = true;
					}
					else{
						crawl_nosort(i, j, k, inSettings);
						crawlpointexit = true;
					}
				}
			}
		}
	}

	if(crawlfromsides){
		for(j=0; j<=h; ++j){
			for(i=j%2; i<=w; i+=2){
				// left side of volume
				cubedata& cube0(cubes[cubeindex(i,j,0)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					if(i!=0 && j!=0)
						crawl_nosort(i-1,j-1,0, inSettings);
					if(i!=w && j!=0)
						crawl_nosort(i,j-1,0, inSettings);
					if(i!=0 && j!=h)
						crawl_nosort(i-1,j,0, inSettings);
					if(i!=w && j!=h)
						crawl_nosort(i,j,0, inSettings);
				}
				// right side of volume
				cubedata& cube1(cubes[cubeindex(i,j,l)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					if(i!=0 && j!=0)
						crawl_nosort(i-1,j-1,l-1, inSettings);
					if(i!=w && j!=0)
						crawl_nosort(i,j-1,l-1, inSettings);
					if(i!=0 && j!=h)
						crawl_nosort(i-1,j,l-1, inSettings);
					if(i!=w && j!=h)
						crawl_nosort(i,j,l-1, inSettings);
				}
			}
		}
		for(k=1; k<l; ++k){
			for(i=k%2; i<=w; i+=2){
				// bottom of volume
				cubedata& cube0(cubes[cubeindex(i,0,k)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					if(i!=0){
						crawl_nosort(i-1,0,k-1, inSettings);
						crawl_nosort(i-1,0,k, inSettings);
					}
					if(i!=w){
						crawl_nosort(i,0,k-1, inSettings);
						crawl_nosort(i,0,k, inSettings);
					}
				}
				// top of volume
				cubedata& cube1(cubes[cubeindex(i,h,k)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					if(i!=0){
						crawl_nosort(i-1,h-1,k-1, inSettings);
						crawl_nosort(i-1,h-1,k, inSettings);
					}
					if(i!=w){
						crawl_nosort(i,h-1,k-1, inSettings);
						crawl_nosort(i,h-1,k, inSettings);
					}
				}
			}
		}
		for(k=1; k<l; ++k){
			for(j=(k%2)+1; j<h; j+=2){
				// back of volume
				cubedata& cube0(cubes[cubeindex(0,j,k)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					crawl_nosort(0,j-1,k-1, inSettings);
					crawl_nosort(0,j,k-1, inSettings);
					crawl_nosort(0,j-1,k, inSettings);
					crawl_nosort(0,j,k, inSettings);
				}
				// front of volume
				cubedata& cube1(cubes[cubeindex(w,j,k)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					crawl_nosort(w-1,j-1,k-1, inSettings);
					crawl_nosort(w-1,j,k-1, inSettings);
					crawl_nosort(w-1,j-1,k, inSettings);
					crawl_nosort(w-1,j,k, inSettings);
				}
			}
		}
	}

	// polygonize
	currentVertexIndex = 0;
	for(unsigned int c=0; c<currentCubeIndex; ++c)
		polygonize(cubeIndices[c], inSettings);
}


void impCubeVolume::makeSurface(float eyex, float eyey, float eyez, impCrawlPointVector &cpv, MicrocosmSaverSettings *inSettings){
	int i, j, k;
	bool crawlpointexit;
	unsigned int mask;

	frame ++;

	surface->reset();

	// erase list from last frame
	sortableCubes.clear();

	// crawl from every crawl point to create the surface
	for(unsigned int cp=0; cp<cpv.size(); ++cp){
		// find cube corresponding to crawl point
		i = int((cpv[cp].position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= int(w))
			i = int(w) - 1;
		j = int((cpv[cp].position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= int(h))
			j = int(h) - 1;
		k = int((cpv[cp].position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= int(l))
			k = int(l) - 1;

		// escape if starting on a finished cube
		crawlpointexit = false;
		while(!crawlpointexit){
			const unsigned int ci(cubeindex(i,j,k));
			if(cubes[ci].cube_frame == frame)
				crawlpointexit = true;  // escape if starting on a finished cube
			else{  // find index for this cube
				findcornervalues(i, j, k, inSettings);
				mask = calculateCornerMask(i, j, k);
				// save index for polygonizing
				cubes[ci].mask = mask;
				if(mask == 255)  // escape if outside surface
					crawlpointexit = true;
				else{
					if(mask == 0){  // this cube is inside volume
						cubes[ci].cube_frame = frame;
						--i;  // step to an adjacent cube and start over
						if(i < 0)  // escape if you step outside of volume
							crawlpointexit = true;
					}
					else{
						crawl_sort(i, j, k, inSettings);
						crawlpointexit = true;
					}
				}
			}
		}
	}

	if(crawlfromsides){
		for(j=0; j<=h; ++j){
			for(i=j%2; i<=w; i+=2){
				// left side of volume
				cubedata& cube0(cubes[cubeindex(i,j,0)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					if(i!=0 && j!=0)
						crawl_sort(i-1,j-1,0, inSettings);
					if(i!=w && j!=0)
						crawl_sort(i,j-1,0, inSettings);
					if(i!=0 && j!=h)
						crawl_sort(i-1,j,0, inSettings);
					if(i!=w && j!=h)
						crawl_sort(i,j,0, inSettings);
				}
				// right side of volume
				cubedata& cube1(cubes[cubeindex(i,j,l)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					if(i!=0 && j!=0)
						crawl_sort(i-1,j-1,l-1, inSettings);
					if(i!=w && j!=0)
						crawl_sort(i,j-1,l-1, inSettings);
					if(i!=0 && j!=h)
						crawl_sort(i-1,j,l-1, inSettings);
					if(i!=w && j!=h)
						crawl_sort(i,j,l-1, inSettings);
				}
			}
		}
		for(k=1; k<l; ++k){
			for(i=k%2; i<=w; i+=2){
				// bottom of volume
				cubedata& cube0(cubes[cubeindex(i,0,k)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					if(i!=0){
						crawl_sort(i-1,0,k-1, inSettings);
						crawl_sort(i-1,0,k, inSettings);
					}
					if(i!=w){
						crawl_sort(i,0,k-1, inSettings);
						crawl_sort(i,0,k, inSettings);
					}
				}
				// top of volume
				cubedata& cube1(cubes[cubeindex(i,h,k)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					if(i!=0){
						crawl_sort(i-1,h-1,k-1, inSettings);
						crawl_sort(i-1,h-1,k, inSettings);
					}
					if(i!=w){
						crawl_sort(i,h-1,k-1, inSettings);
						crawl_sort(i,h-1,k, inSettings);
					}
				}
			}
		}
		for(k=1; k<l; ++k){
			for(j=(k%2)+1; j<h; j+=2){
				// back of volume
				cubedata& cube0(cubes[cubeindex(0,j,k)]);
				if(cube0.corner_frame != frame){
					cube0.corner_frame = frame;
					cube0.value = function(&(cube0.x), inSettings);
				}
				if(cube0.value >= surfacevalue){
					crawl_sort(0,j-1,k-1, inSettings);
					crawl_sort(0,j,k-1, inSettings);
					crawl_sort(0,j-1,k, inSettings);
					crawl_sort(0,j,k, inSettings);
				}
				// front of volume
				cubedata& cube1(cubes[cubeindex(w,j,k)]);
				if(cube1.corner_frame != frame){
					cube1.corner_frame = frame;
					cube1.value = function(&(cube1.x), inSettings);
				}
				if(cube1.value >= surfacevalue){
					crawl_sort(w-1,j-1,k-1, inSettings);
					crawl_sort(w-1,j,k-1, inSettings);
					crawl_sort(w-1,j-1,k, inSettings);
					crawl_sort(w-1,j,k, inSettings);
				}
			}
		}
	}

	// find depths of cubes for sorting
	for(std::list<sortableCube>::iterator c = sortableCubes.begin(); c != sortableCubes.end(); ++c){
		const unsigned int ci(c->index);
		const float xdist(cubes[ci].x - eyex);
		const float ydist(cubes[ci].y - eyey);
		const float zdist(cubes[ci].z - eyez);
		c->depth = xdist * xdist + ydist * ydist + zdist * zdist;
	}

	// sort list of cubes
	sortableCubes.sort();

	// polygonize
	currentVertexIndex = 0;
	for(std::list<sortableCube>::iterator c = sortableCubes.begin(); c != sortableCubes.end(); ++c)
		polygonize(c->index, inSettings);
}


// calculate index into cubetable
const unsigned int impCubeVolume::calculateCornerMask(const unsigned int& x, const unsigned int& y, const unsigned int& z){
	const unsigned int index(cubeindex(x,y,z));
	return ((cubes[index].value < surfacevalue) ? LBF : 0)
		+ ((cubes[index+1].value < surfacevalue) ? RBF : 0)
		+ ((cubes[index+w_1].value < surfacevalue) ? LTF : 0)
		+ ((cubes[index+1+w_1].value < surfacevalue) ? RTF : 0)
		+ ((cubes[index+w_1xh_1].value < surfacevalue) ? LBN : 0)
		+ ((cubes[index+1+w_1xh_1].value < surfacevalue) ? RBN : 0)
		+ ((cubes[index+w_1+w_1xh_1].value < surfacevalue) ? LTN : 0)
		+ ((cubes[index+1+w_1+w_1xh_1].value < surfacevalue) ? RTN : 0);
}


void impCubeVolume::crawl_nosort(unsigned int x, unsigned int y, unsigned int z, MicrocosmSaverSettings *inSettings){
	// quit if this cube has been done
	const unsigned int ci(cubeindex(x,y,z));
	cubedata& cube(cubes[ci]);
	if(cube.cube_frame == frame)
		return;

	findcornervalues(x, y, z, inSettings);
	const unsigned int mask(calculateCornerMask(x, y, z));

	// add this cube to list of crawled cubes, resizing vector if necessary
	const size_t tslsize(cubeIndices.size());
	if(currentCubeIndex == tslsize)
		cubeIndices.resize(tslsize + 1000);
	cubeIndices[currentCubeIndex++] = ci;

	// save index for polygonizing
	cube.mask = mask;

	// mark this cube as completed
	cube.cube_frame = frame;

	// crawl to adjacent cubes
	if(crawlDirections[mask][0] && x > 0)
		crawl_nosort(x-1, y, z, inSettings);
	if(crawlDirections[mask][1] && x < w-1)
		crawl_nosort(x+1, y, z, inSettings);
	if(crawlDirections[mask][2] && y > 0)
		crawl_nosort(x, y-1, z, inSettings);
	if(crawlDirections[mask][3] && y < h-1)
		crawl_nosort(x, y+1, z, inSettings);
	if(crawlDirections[mask][4] && z > 0)
		crawl_nosort(x, y, z-1, inSettings);
	if(crawlDirections[mask][5] && z < l-1)
		crawl_nosort(x, y, z+1, inSettings);
}


void impCubeVolume::crawl_sort(unsigned int x, unsigned int y, unsigned int z, MicrocosmSaverSettings *inSettings){
	// quit if this cube has been done
	const unsigned int ci(cubeindex(x,y,z));
	cubedata& cube(cubes[ci]);
	if(cube.cube_frame == frame)
		return;

	findcornervalues(x, y, z, inSettings);
	const int mask(calculateCornerMask(x, y, z));

	// add cube to list
	sortableCubes.push_back(sortableCube(ci));

	// save index for polygonizing
	cube.mask = mask;

	// mark this cube as completed
	cube.cube_frame = frame;

	// crawl to adjacent cubes
	if(crawlDirections[mask][0] && x > 0)
		crawl_sort(x-1, y, z, inSettings);
	if(crawlDirections[mask][1] && x < w-1)
		crawl_sort(x+1, y, z, inSettings);
	if(crawlDirections[mask][2] && y > 0)
		crawl_sort(x, y-1, z, inSettings);
	if(crawlDirections[mask][3] && y < h-1)
		crawl_sort(x, y+1, z, inSettings);
	if(crawlDirections[mask][4] && z > 0)
		crawl_sort(x, y, z-1, inSettings);
	if(crawlDirections[mask][5] && z < l-1)
		crawl_sort(x, y, z+1, inSettings);
}


// polygonize an individual cube
void impCubeVolume::polygonize(unsigned int index, MicrocosmSaverSettings *inSettings){
	// find index into cubetable
	const unsigned int mask(cubes[index].mask);

	unsigned int counter = 0;
	unsigned int nedges = triStripPatterns[mask][counter];
	unsigned int tri_ind;
	while(nedges != 0){
#if USE_TRIANGLE_STRIPS
		surface->addTriStripLength(nedges);
		for(unsigned int i=1; i<=nedges; ++i){
			switch(triStripPatterns[mask][counter+i]){
#else
		for(unsigned int i=1; i<nedges-1; ++i){
			for(unsigned int j=0; j<3; ++j){
				if((i % 2))
					tri_ind = counter + i + j;
				else
					tri_ind = counter + i + (2 - j);
				switch(triStripPatterns[mask][tri_ind]){
#endif
				// generate vertex position and normal data
				case 0:
					addVertexToSurface(2, index, inSettings);
					break;
				case 1:
					addVertexToSurface(1, index, inSettings);
					break;
				case 2:
					addVertexToSurface(1, index + w_1xh_1, inSettings);
					break;
				case 3:
					addVertexToSurface(2, index + w_1, inSettings);
					break;
				case 4:
					addVertexToSurface(0, index, inSettings);
					break;
				case 5:
					addVertexToSurface(0, index + w_1xh_1, inSettings);
					break;
				case 6:
					addVertexToSurface(0, index + w_1, inSettings);
					break;
				case 7:
					addVertexToSurface(0, index + w_1 + w_1xh_1, inSettings);
					break;
				case 8:
					addVertexToSurface(2, index + 1, inSettings);
					break;
				case 9:
					addVertexToSurface(1, index + 1, inSettings);
					break;
				case 10:
					addVertexToSurface(1, index + 1 + w_1xh_1, inSettings);
					break;
				case 11:
					addVertexToSurface(2, index + 1 + w_1, inSettings);
					break;
				}
#if USE_TRIANGLE_STRIPS
#else
			}
#endif
		}
		counter += (nedges + 1);
		nedges = triStripPatterns[mask][counter];
	}
}


// find value at all corners of this cube
void impCubeVolume::findcornervalues(unsigned int x, unsigned int y, unsigned int z, MicrocosmSaverSettings *inSettings){
	{
		cubedata& cube(cubes[cubeindex(x,y,z)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x+1,y,z)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x,y+1,z)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x+1,y+1,z)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x,y,z+1)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x+1,y,z+1)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x,y+1,z+1)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
	{
		cubedata& cube(cubes[cubeindex(x+1,y+1,z+1)]);
		if(cube.corner_frame != frame){
			cube.corner_frame = frame;
			cube.value = function(&(cube.x), inSettings);
		}
	}
}


float impCubeVolume::getXPlus1Value(unsigned int index, MicrocosmSaverSettings *inSettings){
	const unsigned int indexPlus1(index + 1);

	// compute new value if index is at the edge of the volume
	if((indexPlus1 % w_1) == 0){
		float* pos = &(cubes[index].x);
		pos[0] += cubewidth;
		const float value(function(pos, inSettings));
		pos[0] -= cubewidth;
		return value;
	}

	// return already computed value
	cubedata& cube(cubes[indexPlus1]);
	if(cube.corner_frame == frame)
		return cube.value;

	// compute new value
	cube.corner_frame = frame;
	cube.value = function(&(cube.x), inSettings);
	return cube.value;
}


float impCubeVolume::getYPlus1Value(unsigned int index, MicrocosmSaverSettings *inSettings){
	const unsigned int indexPlus1(index + w_1);

	// compute new value if index is at the edge of the volume
	if(indexPlus1 % w_1xh_1 < w_1){
		float* pos = &(cubes[index].x);
		pos[1] += cubewidth;
		const float value(function(pos, inSettings));
		pos[1] -= cubewidth;
		return value;
	}

	// return already computed value
	cubedata& cube(cubes[indexPlus1]);
	if(cube.corner_frame == frame)
		return cube.value;

	// compute new value
	cube.corner_frame = frame;
	cube.value = function(&(cube.x), inSettings);
	return cube.value;
}


float impCubeVolume::getZPlus1Value(unsigned int index, MicrocosmSaverSettings *inSettings){
	const unsigned int indexPlus1(index + w_1xh_1);

	// compute new value if index is at the edge of the volume
	if(indexPlus1 >= w_1xh_1xl_1){
		float* pos = &(cubes[index].x);
		pos[2] += cubewidth;
		const float value(function(pos, inSettings));
		pos[2] -= cubewidth;
		return value;
	}

	// return already computed value
	cubedata& cube(cubes[indexPlus1]);
	if(cube.corner_frame == frame)
		return cube.value;

	// compute new value
	cube.corner_frame = frame;
	cube.value = function(&(cube.x), inSettings);
	return cube.value;
}


// Here we compute a vertex position and normal and add it to the surface.
// If fastnormal is true, we use the difference between existing corner values as much as possible,
// only computing new values when necessary.  Many different combinations and blends of value
// differences were tried out.  The final algorithm used here is not only the simplest possible, but
// also the best looking.  Using more data to compute the normals always made the normals look worse.
void impCubeVolume::addVertexToSurface(const unsigned int& axis, const unsigned int& index, MicrocosmSaverSettings *inSettings){
	float data[6];

	// find position of vertex along this edge
	switch(axis){
		case 0:{  // x-axis
			if(cubes[index].x_vertex_frame == frame){
				// Position and normal have already been computed for this edge.
				surface->addIndex(cubes[index].x_vertex_index);
				return;
			}
			cubes[index].x_vertex_frame = frame;
			surface->addIndex(currentVertexIndex);
			cubes[index].x_vertex_index = currentVertexIndex;
			currentVertexIndex ++;
			// compute vertex position
			const float t((surfacevalue - cubes[index].value) / (cubes[index+1].value - cubes[index].value));
			data[3] = cubes[index].x + (cubewidth * t);
			data[4] = cubes[index].y;
			data[5] = cubes[index].z;
			if(fastnormals){
				// compute normal
				const float one_minus_t(1.0f - t);
				const float& val(cubes[index].value);
				const float& valp1(cubes[index+1].value);
				data[0] = one_minus_t * (val - valp1) + t * (valp1 - getXPlus1Value(index+1, inSettings));
				data[1] = one_minus_t * (val - getYPlus1Value(index, inSettings)) + t * (valp1 - getYPlus1Value(index+1, inSettings));
				data[2] = one_minus_t * (val - getZPlus1Value(index, inSettings)) + t * (valp1 - getZPlus1Value(index+1, inSettings));
				// For speed, do not normalize; use GL_NORMALIZE instead
				// Add this vertex to surface
				surface->addVertex(data);
				return;
			}
			break;
		}
		case 1:{  // y-axis
			if(cubes[index].y_vertex_frame == frame){
				// Position and normal have already been computed for this edge.
				surface->addIndex(cubes[index].y_vertex_index);
				return;
			}
			cubes[index].y_vertex_frame = frame;
			surface->addIndex(currentVertexIndex);
			cubes[index].y_vertex_index = currentVertexIndex;
			currentVertexIndex ++;
			// compute vertex position
			const float t((surfacevalue - cubes[index].value) / (cubes[index+w_1].value - cubes[index].value));
			data[3] = cubes[index].x;
			data[4] = cubes[index].y + (cubewidth * t);
			data[5] = cubes[index].z;
			if(fastnormals){
				// compute normal
				const float one_minus_t(1.0f - t);
				const float& val(cubes[index].value);
				const float& valp1(cubes[index+w_1].value);
				data[0] = one_minus_t * (val - getXPlus1Value(index, inSettings)) + t * (valp1 - getXPlus1Value(index+w_1, inSettings));
				data[1] = one_minus_t * (val - valp1) + t * (valp1 - getYPlus1Value(index+w_1, inSettings));
				data[2] = one_minus_t * (val - getZPlus1Value(index, inSettings)) + t * (valp1 - getZPlus1Value(index+w_1, inSettings));
				// For speed, do not normalize; use GL_NORMALIZE instead
				// Add this vertex to surface
				surface->addVertex(data);
				return;
			}
			break;
		}
		case 2:{  // z-axis
			if(cubes[index].z_vertex_frame == frame){
				// Position and normal have already been computed for this edge.
				surface->addIndex(cubes[index].z_vertex_index);
				return;
			}
			cubes[index].z_vertex_frame = frame;
			surface->addIndex(currentVertexIndex);
			cubes[index].z_vertex_index = currentVertexIndex;
			currentVertexIndex ++;
			// compute vertex position
			const float t((surfacevalue - cubes[index].value) / (cubes[index+w_1xh_1].value - cubes[index].value));
			data[3] = cubes[index].x;
			data[4] = cubes[index].y;
			data[5] = cubes[index].z + (cubewidth * t);
			if(fastnormals){
				// compute normal
				const float one_minus_t(1.0f - t);
				const float& val(cubes[index].value);
				const float& valp1(cubes[index+w_1xh_1].value);
				data[0] = one_minus_t * (val - getXPlus1Value(index, inSettings)) + t * (valp1 - getXPlus1Value(index+w_1xh_1, inSettings));
				data[1] = one_minus_t * (val - getYPlus1Value(index, inSettings)) + t * (valp1 - getYPlus1Value(index+w_1xh_1, inSettings));
				data[2] = one_minus_t * (val - valp1) + t * (valp1 - getZPlus1Value(index+w_1xh_1, inSettings));
				// For speed, do not normalize; use GL_NORMALIZE instead
				// Add this vertex to surface
				surface->addVertex(data);
				return;
			}
			break;
		}
	}

	// Slow but accurate normals.
	// These will be computed if fast normals were not computed above.
	// Find normal vector at vertex along this edge
	// First find normal vector origin value
	float* pos = &(data[3]);
	const float offset(cubewidth * 0.1f);
	const float val(function(pos, inSettings));
	// then find values at slight displacements and subtract
	pos[0] -= offset;
	data[0] = function(pos, inSettings) - val;
	pos[0] += offset;
	pos[1] -= offset;
	data[1] = function(pos, inSettings) - val;
	pos[1] += offset;
	pos[2] -= offset;
	data[2] = function(pos, inSettings) - val;
	pos[2] += offset;
	// For speed, do not normalize; use GL_NORMALIZE instead

	// Add this vertex to surface
	surface->addVertex(data);
}
