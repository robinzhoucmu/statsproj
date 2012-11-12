//////////////////////////////////////////////////////////////////////////////////////////////////
// Author:		Liang-Liang He
// Version:		0.1
// Date:		June 12, 2009 
// Description: This file is used to extract HOG features.         
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

//
//Reference
//N. Dalal and B. Triggs, “Histograms of oriented gradients for human detection,” 
//in IEEE Computer Society Conference on Computer Vision and Pattern Recognition, 2005., vol. 1, 2005.
//


#include "hog_extractor.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/format.hpp>
#include <stdexcept>

using namespace boost::numeric;

//extract HOG features from a group of key points
void HOGExtractor::extract(ublas::matrix<float> &dscr_list, const ublas::matrix<int> &bbox_list, const IntegralHistogram &inthist)
{
	unsigned int length = _param.xgrid * _param.ygrid * inthist.dirnum();
	if (dscr_list.size1() != bbox_list.size1() || dscr_list.size2() != length)
		throw std::runtime_error( boost::str(
					boost::format("Allocate proper memory for dscr_list [%1% %2%] != [%3% %4%]") % dscr_list.size1() % dscr_list.size2() % dscr_list.size2() % length) );

	ublas::matrix<float>::iterator1 idscr_list;
	ublas::matrix<int>::const_iterator1 ibbox_list;
	ublas::matrix<int>::const_iterator1 ibbox_list_end(bbox_list.end1());
	ublas::vector<int> bbox_grids( _param.xgrid * _param.ygrid * 4 );
	for ( ibbox_list = bbox_list.begin1(), idscr_list = dscr_list.begin1(); ibbox_list != ibbox_list_end; ++ibbox_list, ++idscr_list)
	{
		ublas::matrix<int>::const_iterator2 ibbox = ibbox_list.begin();
		float x0 = std::max( *ibbox++, 0 );
		float y0 = std::max( *ibbox++, 0 );
		float x1 = std::min( *ibbox++, inthist.width() - 1 );
		float y1 = std::min( *ibbox++, inthist.height() - 1 );
		if ( x1 - x0 < _param.xgrid || y1 - y0 < _param.ygrid )
			throw std::runtime_error( boost::str(boost::format("Grid number should be less than bbox width/height [%1% %2% %3% %4%]/[%5% %6%]") % x0 % y0 % x1 % y1 % _param.xgrid % _param.ygrid) );

		float xstep = (x1 - x0 + 1) / _param.xgrid;
		float ystep = (y1 - y0 + 1) / _param.ygrid;
		for (int i = 0; i < _param.xgrid; ++i)
			for (int j = 0; j < _param.ygrid; ++j)
			{
				bbox_grids( (i * _param.ygrid + j)*4 + 0) = static_cast<int>(x0 + xstep*i);    //x0 
				bbox_grids( (i * _param.ygrid + j)*4 + 1) = static_cast<int>(y0 + ystep*j);    //y0 
				bbox_grids( (i * _param.ygrid + j)*4 + 2) = static_cast<int>(x0 + xstep*(i + 1) - 1);    //x1 
				bbox_grids( (i * _param.ygrid + j)*4 + 3) = static_cast<int>(y0 + ystep*(j + 1) - 1);    //y1 
			}
		inthist.get_hist< ublas::matrix<float>::iterator2, ublas::vector<int>::const_iterator>( idscr_list.begin(), bbox_grids.begin(), bbox_grids.end(), _param.normalize);
	}
}

//extract HOG feature from one key point
void HOGExtractor::extract(ublas::vector<float> &dscr,  const ublas::vector<int> &bbox, const IntegralHistogram &inthist)
{
	unsigned int length = _param.xgrid * _param.ygrid * inthist.dirnum();
	if (dscr.size() != length)
		throw std::runtime_error( boost::str(boost::format("Allocate proper memory for dscr [%1%] != [%2%]") % dscr.size() % length) );

	inthist.get_hist< ublas::vector<float>::iterator, ublas::vector<int>::const_iterator>( dscr.begin(), bbox.begin(), bbox.end() );

	ublas::vector<int> bbox_grids( _param.xgrid * _param.ygrid * 4 );
	ublas::vector<int>::const_iterator ibbox = bbox.begin();
	float x0 = std::max( *ibbox++, 0 );
	float y0 = std::max( *ibbox++, 0 );
	float x1 = std::min( *ibbox++, inthist.width() - 1 );
	float y1 = std::min( *ibbox++, inthist.height() - 1 );

	if ( x1 - x0 < _param.xgrid || y1 - y0 < _param.ygrid )
		throw std::runtime_error( boost::str(boost::format("Grid number should be less than bbox width/height [%1% %2% %3% %4%]/[%5% %6%]") % x0 % y0 % x1 % y1 % _param.xgrid % _param.ygrid) );

	float xstep = (x1 - x0 + 1) / _param.xgrid;
	float ystep = (y1 - y0 + 1) / _param.ygrid;
	for (int i = 0; i < _param.xgrid; ++i)
		for (int j = 0; j < _param.ygrid; ++j)
		{
			bbox_grids( (i * _param.ygrid + j)*4 + 0) = static_cast<int>(x0 + xstep*i);    //x0 
			bbox_grids( (i * _param.ygrid + j)*4 + 1) = static_cast<int>(y0 + ystep*j);    //y0 
			bbox_grids( (i * _param.ygrid + j)*4 + 2) = static_cast<int>(x0 + xstep*(i + 1) - 1);    //x1 
			bbox_grids( (i * _param.ygrid + j)*4 + 3) = static_cast<int>(y0 + ystep*(j + 1) - 1);    //y1 
		}
	inthist.get_hist< ublas::vector<float>::iterator, ublas::vector<int>::const_iterator>( dscr.begin(), bbox_grids.begin(), bbox_grids.end(), _param.normalize);
}
