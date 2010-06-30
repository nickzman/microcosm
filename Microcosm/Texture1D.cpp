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


#ifdef WIN32
#include <windows.h>
#endif
#include <Microcosm/Texture1D.h>
#include <Rgbhsl/Rgbhsl.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>


/*extern float frameTime;
extern int dColorSpeed;*/


void Texture1D::init(int colorSpeed){
	for(unsigned int i=0; i<TEX_SIZE; i++){
		const unsigned int index = i * 4;
		mData[index] = rsRandi(256);
		mData[index+1] = rsRandi(256);
		mData[index+2] = rsRandi(256);
		mData[index+3] = rsRandi(256);
	}

	for(int i=0; i<NUM_TEX_COEFF; i++){
		mCoeffPhase[i] = rsRandf(RS_PIx2);
		mCoeffRate[i] = (rsRandf(0.002f) + 0.002f) * float(colorSpeed);
	}

	glGenTextures(1, &mTexId);
	bind();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, 4, TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)mData);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//gluBuild1DMipmaps(GL_TEXTURE_1D, 4, TEXSIZE, GL_RGBA, GL_UNSIGNED_BYTE, (void *)mData);
}


void Texture1D::update(float frameTime){
	// Exponent greater than 1.0 keeps color channels close to grey for longer.
	// This gives us more greys and browns and less primaries, secondaries, and blacks.
	// Crank the exponent too high and the colors will just start to look muted in a boring way.
	//const float exp = 2.0f;

	for(int i=0; i<NUM_TEX_COEFF; i++){
		mCoeffPhase[i] += mCoeffRate[i] * frameTime;
		if(mCoeffPhase[i] >= RS_PIx2)
			mCoeffPhase[i] -= RS_PIx2;
		mCoeff[i] = rsCosf(mCoeffPhase[i]);
	}

	for(int i=0; i<NUM_COLORS; i++){
		const int c = i * 6;

		// First, choose random color in hsl color space
		// Cycle hue in different directions and at different rates to keep things sufficiently random
		float hue;
		switch(i%3){
		case 0:
			hue = mCoeffPhase[c] * 2.0f / RS_PIx2;
			break;
		case 1:
			hue = 2.0f - (mCoeffPhase[c] * 2.0f / RS_PIx2);
			break;
		case 2:
			hue = mCoeffPhase[c] * 3.0f / RS_PIx2;
			break;
		}
		// Saturation and luminosity are bonehead simple.
		const float sat = mCoeff[c+1] * 0.5f + 0.5f;
		const float lum = mCoeff[c+2] * 0.5f + 0.5f;
		// The resulting colors here are a lot of earth tones, off whites, and pastels.  Ick

		// Convert to rgb color space
		float r, g, b;
		hsl2rgb(hue, sat, lum, r, g, b);

		// Randomize a little more (now the color is *really* random)
		// Set this constant too high and there will be too many pure primaries and secondaries.
		const float rgb_variance = 0.5f;
		r += mCoeff[c+3] * rgb_variance;
		g += mCoeff[c+4] * rgb_variance;
		b += mCoeff[c+5] * rgb_variance;
		if(r < 0.0f) r = 0.0f;
		if(r > 1.0f) r = 1.0f;
		if(g < 0.0f) g = 0.0f;
		if(g > 1.0f) g = 1.0f;
		if(b < 0.0f) b = 0.0f;
		if(b > 1.0f) b = 1.0f;

		mColor[i].set(r, g, b, 1.0f);
	}

	// Fade between 3 colors
	rsVec4 color;
	for(unsigned int i=0; i<TEX_SIZE/2; i++){
		const float t = float(i) / float(TEX_SIZE/2);
		color = mColor[0] * (1.0f - t) + mColor[1] * t;
		const unsigned int index = i * 4;
		mData[index] = (unsigned char)(255.999f * color[0]);
		mData[index+1] = (unsigned char)(255.999f * color[1]);
		mData[index+2] = (unsigned char)(255.999f * color[2]);
		mData[index+3] = (unsigned char)(255.999f * color[3]);
	}
	for(unsigned int i=0; i<TEX_SIZE/2; i++){
		const float t = float(i) / float(TEX_SIZE/2);
		color = mColor[1] * (1.0f - t) + mColor[2] * t;
		const unsigned int index = (i+TEX_SIZE/2) * 4;
		mData[index] = (unsigned char)(255.999f * color[0]);
		mData[index+1] = (unsigned char)(255.999f * color[1]);
		mData[index+2] = (unsigned char)(255.999f * color[2]);
		mData[index+3] = (unsigned char)(255.999f * color[3]);
	}

	// Fade between 4 colors
	/*for(unsigned int i=0; i<TEX_SIZE/3; i++){
		const float t = float(i) / float(TEX_SIZE/3);
		color = mColor[0] * (1.0f - t) + mColor[1] * t;
		const unsigned int index = i * 4;
		mData[index] = (unsigned char)(255.999f * color[0]);
		mData[index+1] = (unsigned char)(255.999f * color[1]);
		mData[index+2] = (unsigned char)(255.999f * color[2]);
		mData[index+3] = (unsigned char)(255.999f * color[3]);
	}
	for(unsigned int i=TEX_SIZE/3; i<2*TEX_SIZE/3; i++){
		const float t = float(i-TEX_SIZE/3) / float(TEX_SIZE/3);
		color = mColor[1] * (1.0f - t) + mColor[2] * t;
		const unsigned int index = i * 4;
		mData[index] = (unsigned char)(255.999f * color[0]);
		mData[index+1] = (unsigned char)(255.999f * color[1]);
		mData[index+2] = (unsigned char)(255.999f * color[2]);
		mData[index+3] = (unsigned char)(255.999f * color[3]);
	}
	for(unsigned int i=2*TEX_SIZE/3; i<TEX_SIZE; i++){
		const float t = float(i-(2*TEX_SIZE/3)) / float(TEX_SIZE-(2*TEX_SIZE/3));
		color = mColor[2] * (1.0f - t) + mColor[3] * t;
		const unsigned int index = i * 4;
		mData[index] = (unsigned char)(255.999f * color[0]);
		mData[index+1] = (unsigned char)(255.999f * color[1]);
		mData[index+2] = (unsigned char)(255.999f * color[2]);
		mData[index+3] = (unsigned char)(255.999f * color[3]);
	}*/

	// Update OpenGL texture object
	bind();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, TEX_SIZE);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, (void *)mData);
}


//void Texture1D::interpColor(float(i) / float(TEX_SIZE), color);


void Texture1D::bind(){
	glBindTexture(GL_TEXTURE_1D, mTexId);
}
