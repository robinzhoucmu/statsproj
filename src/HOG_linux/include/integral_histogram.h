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


#ifndef INTIGRAL_HISTOGRAM_H
#define INTIGRAL_HISTOGRAM_H

#include "image.h"

#include <boost/detail/iterator.hpp>
#include <boost/format.hpp>
#include <stdexcept>
#include <fstream>

class IntegralHistogram
{
	public:
		enum hist_type{undirected = 0, directed = 1};
		struct Param
		{
			hist_type htype;
			int dirnum;
			float exp; //weight factor, default 1;
			float sigma;
			Param(hist_type h = directed, int d = 8, float e = 1, float s = 0):htype(h),dirnum(d),exp(e),sigma(s){}
		};

		IntegralHistogram(hist_type htype = directed, int dirnum = 8, float exp = 1, float sigma = 0);
		IntegralHistogram(Param &param);
		~IntegralHistogram();
		void build(Image &img);  //img will be modified

		template<typename OutputIterator, typename InputIterator>
		inline void get_hist(OutputIterator hist_begin, InputIterator bbox_begin, InputIterator bbox_endi, bool normalize = true) const;
		inline const float *get_inthist(int x, int y){ return _inthist + (y*_width + x)*_param.dirnum; }
		inline int dirnum() const { return _param.dirnum; }
		inline int width() const { return _width; }
		inline int height() const { return _height; }
		IntegralHistogram &load( const char *fn );
		IntegralHistogram &save( const char *fn );

	private:
		float *_inthist;
		int _height;
		int _width;
		static const float eps = 1e-10;
		Param _param;
};

template<typename OutputIterator, typename InputIterator>
inline void IntegralHistogram::get_hist(OutputIterator hist_begin, InputIterator bbox_begin, InputIterator bbox_end, bool normalize) const
{
	typedef typename boost::detail::iterator_traits<InputIterator>::reference reference;
	while( bbox_begin != bbox_end )
	{
		reference x0 = *bbox_begin++;
		reference y0 = *bbox_begin++;
		reference x1 = *bbox_begin++;
		reference y1 = *bbox_begin++;

		if ( x0 < 0 || y0 < 0 || x0 > x1 || y0 > y1 || x1 >= _width || y1 >= _height)
			throw std::runtime_error( boost::str(boost::format("Invalid index [%1% %2% %3% %4%]") % x0 % y0 % x1 % y1) );

		float sum = 0;
		OutputIterator hist = hist_begin;
		for (int i = 0; i < _param.dirnum; ++i, ++hist_begin)
		{
			*hist_begin = _inthist[(y1*_width + x1)*_param.dirnum + i];
			if( x0 > 0 )
				*hist_begin -= _inthist[(y1*_width + x0 - 1)*_param.dirnum + i];
			if( y0 > 0 )
				*hist_begin -= _inthist[( (y0-1)*_width + x1 )*_param.dirnum + i];
			if( x0 >0 && y0 >0 )
				*hist_begin += _inthist[( (y0-1)*_width + x0 - 1 )*_param.dirnum + i];

			if( normalize )
				sum += *hist_begin;
		}
		if (normalize)
		{
			for (int i = 0; i < _param.dirnum; ++i, ++hist) { *hist /= (sum + eps); }
		}
	}
}
#endif //INTIGRAL_HISTOGRAM_H
