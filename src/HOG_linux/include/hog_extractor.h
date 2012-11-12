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


#ifndef HOG_EXTRACTOR_H
#define HOG_EXTRACTOR_H

#include "integral_histogram.h"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

using namespace boost::numeric;

class HOGExtractor
{
	public:
		struct Param
		{
			int xgrid;
			int ygrid;
			bool normalize;
			Param(int xg = 4, int yg = 4, bool n = true):xgrid(xg),ygrid(yg),normalize(n){}
		};

		HOGExtractor(Param &param):_param(param){};
		~HOGExtractor(){}
		void extract(ublas::matrix<float> &dscr_list, const ublas::matrix<int> &bbox_list, const IntegralHistogram &inthist);
		void extract(ublas::vector<float> &dscr, const ublas::vector<int> &bbox, const IntegralHistogram &inthist);

	private:
		Param _param;
};

#endif //HOG_EXTRACTOR_H
