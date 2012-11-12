//////////////////////////////////////////////////////////////////////////////////////////////////
// Author:		Liang-Liang He
// Version:		0.1
// Date:		June 12, 2009 
// Description: This file is used to compute integral histogram.         
//
// Copyright(C) 2009 OpenPR 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this
//       list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, 
//       this list of conditions and the following disclaimer in the documentation 
//       and/or other materials provided with the distribution.
//     * Neither the name of the OpenPR nor the names of its contributors may
//       be used to endorse or promote products derived from this software without 
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//////////////////////////////////////////////////////////////////////////////////////////////////


#include "integral_histogram.h"
#include "image.h"

IntegralHistogram::IntegralHistogram(hist_type htype, int dirnum, float exp, float sigma)
{
	_param.htype = htype;
	_param.dirnum = dirnum;
	_param.exp = exp;
	_param.sigma = sigma;

	_inthist = NULL;
}

IntegralHistogram::IntegralHistogram(Param &param): _param(param)
{
	_inthist = NULL;
}

IntegralHistogram::~IntegralHistogram()
{
	delete[] _inthist;
	_inthist = NULL;
}

//calculate integral histogram for img, img will be modified
void IntegralHistogram::build( Image & img )
{
	const float PI = std::atan2(0, -1);
	_width = static_cast<int>(img.dimx());
	_height = static_cast<int>(img.dimy());
	_inthist = new float[_width*_height*_param.dirnum];
	img.blur( _param.sigma );

	//[-1 0 1], [-1 0 1]';
	Image hmask(3, 1), vmask(1, 3);
	hmask(0, 0) = vmask(0, 0) = -1;
	hmask(1, 0) = vmask(0, 1) = 0;
	hmask(2, 0) = vmask(0, 2) = 1;

	Image vmasked = img.get_convolve( vmask );
	Image &hmasked = img.convolve( hmask );

	Image angle(_width, _height);
	if (_param.htype == directed)
		for (int i = 0; i < _width*_height; ++i) angle[i] = std::atan2(vmasked[i], hmasked[i]) + PI;
	else //undirected
		for (int i = 0; i < _width*_height; ++i) {angle[i] = std::atan(vmasked[i] / (hmasked[i] + eps)) + PI/2;}
		
	hmasked.mul(hmasked) += vmasked.get_mul(vmasked);
	Image &mod = hmasked;
	if( (_param.exp - 1) < eps )
	  mod.sqrt();
	else if( (_param.exp - 2) < eps )
	  while(0);
	else
	  mod.pow( _param.exp/2 );
	
	float theta = PI / _param.dirnum;
	if (_param.htype == directed) theta *= 2;

	for (int i = 0; i < _width*_height; ++i)
	{
		float *hist = _inthist + i*_param.dirnum;
		for (int idir = 0; idir < _param.dirnum; ++idir) hist[idir] = 0;

		int ileft = static_cast<int>( angle[i] / theta );
		int iright = ileft + 1;

		float a = angle[i] - ileft*theta;
		float b = iright*theta - angle[i];

		hist[ileft % _param.dirnum] = b*b*mod[i] / (a*a + b*b);
		hist[iright % _param.dirnum] = a*a*mod[i] / (a*a + b*b);
	}

	for (int j = 1; j < _width; ++j) //1st row
	{
		float *hist = _inthist + j*_param.dirnum;
		for (int idir = 0; idir < _param.dirnum; ++idir) 
		{
			hist[idir] += hist[idir - 1]; 
		}
	}

	float *current_row_inthist = new float[_param.dirnum];
	for (int i = 1; i < _height; ++i)
	{
	  for (int idir = 0; idir < _param.dirnum; ++idir) current_row_inthist[idir] = 0;
	  for (int j = 0; j < _width; ++j)
	  {
		  float *hist = _inthist + (i*_width + j)*_param.dirnum;
		  for (int idir = 0; idir < _param.dirnum; ++idir) 
		  {
			  current_row_inthist[idir] += hist[idir];
			  hist[idir] = _inthist[((i - 1)*_width + j)*_param.dirnum + idir] + current_row_inthist[idir]; 
		  }
	  }
	}
}

//load saved integral histogram from file system
IntegralHistogram &IntegralHistogram::load( const char *fn )
{
	std::fstream fin;
	fin.open( fn, std::ios::in|std::ios::binary);
	if (fin.good())
	{
		fin.read(reinterpret_cast<char *>(&_width), sizeof(_width))		\
			.read(reinterpret_cast<char *>(&_height), sizeof(_height))	\
			.read(reinterpret_cast<char *>(&_param.htype), sizeof(_param.htype))		\
			.read(reinterpret_cast<char *>(&_param.dirnum), sizeof(_param.dirnum))	\
			.read(reinterpret_cast<char *>(_inthist), _width*_height*_param.dirnum*sizeof(_inthist[0]));
	}
	else
		throw std::runtime_error( boost::str(boost::format("Failed to open %1%") % fn ) );
	fin.close();

	return *this;
}

//save integral histogram to file system
IntegralHistogram &IntegralHistogram::save( const char *fn )
{
	std::fstream fout;
	fout.open( fn, std::ios::out|std::ios::binary);
	if (fout.good())
	{
		fout.write(reinterpret_cast<char *>(&_width), sizeof(_width))		\
			.write(reinterpret_cast<char *>(&_height), sizeof(_height))	\
			.write(reinterpret_cast<char *>(&_param.htype), sizeof(_param.htype))		\
			.write(reinterpret_cast<char *>(&_param.dirnum), sizeof(_param.dirnum))	\
			.write(reinterpret_cast<char *>(_inthist), _width*_height*_param.dirnum*sizeof(_inthist[0]));
	}
	else
		throw std::runtime_error( boost::str(boost::format("Failed to open %1%") % fn ) );
	fout.close();

	return *this;
}

