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

#include <iostream>
#include <unistd.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>

#include "integral_histogram.h"
#include "hog_extractor.h"

void help_exit(const char *app_name)
{
	std::cout << "usage: " << app_name << " [options] \n"
		<< "\t-i \t input image\n"
		<< "\t-o \t descriptor file\n"
		<< "\t-k \t key point file, default:random sampling\n"
		<< "\t-d \t sampling density: 1/grid_size\n"
		<< "\t-p \t patch size\n"
		<< "\t-h \t display this message\n";
	exit(1);
}

/*
  extract_hog_features

  Author:      Karthik C Lakshmanan
  Description: Wrapper for HOG feature extractor. See void test() below for usage example
  Input:       const ublas::matrix<int> &bbox - 1x4 matrix that contains [x1,y1,x2,y2] where (x1,y1) and (x2,y2) 
                                                are the lower and upper vertices of the rectangular window
	       const char *fin                - Filename that contains image
  Output:      ublas::matrix<float> dscr      - 1xn matrix that contains HoG feature vector for the window
 */

ublas::matrix<float> extract_hog_features(const ublas::matrix<int> &bbox,  const char *fin)
{
  // Fix parameters. TODO: allow parameters to be passed in?
  IntegralHistogram::Param inthist_param;
  inthist_param.dirnum = 8;
  inthist_param.exp = 1;
  inthist_param.sigma = 0;
  inthist_param.htype = IntegralHistogram::directed;

  HOGExtractor::Param hog_param;
  hog_param.xgrid = 4;
  hog_param.ygrid = 4;
  hog_param.normalize = true;
  
  float kp_density = 1.0/8.0; // 1/grid_size                                                                                  
  int patch_size = 32;

  // Create Image and build Integral Histogram
  std::cout << "building int hist";
  Image im( fin );
  IntegralHistogram inthist(inthist_param);
  inthist.build(im);
  
  // Extract features using functions in hog_extractor.cpp
  HOGExtractor extr(hog_param);
  std::cout<< "reached here";
  ublas::matrix<float> dscr(1, hog_param.xgrid * hog_param.ygrid * inthist_param.dirnum);
  extr.extract(dscr, bbox, inthist);
  
  return dscr;
}

void test()
{
  std::cout << "In test\n";
  ublas::matrix<int> bbox(1, 4);
  int patch_size = 64;
  int x = 50;
  int y = 50;                                                                         
  bbox(0, 0) = x - patch_size/2;
  bbox(0, 1) = y - patch_size/2;
  bbox(0, 2) = x + patch_size/2;
  bbox(0, 3) = y + patch_size/2;
  const char* fin = "test.jpg";
  std::cout<< "calling extract hog features\n";
  ublas::matrix<float> dscr = extract_hog_features(bbox,fin);
  std::cout <<"dscr size is"<<dscr.size2();
  for (int i = 0; i < dscr.size2();++i)
    {
      std::cout<< dscr(0,i)<<"\n";
    }
}

int main( int argc, char *argv[] )
{
 
  test();
  return 0;
 
	IntegralHistogram::Param inthist_param;
	inthist_param.dirnum = 8;
	inthist_param.exp = 1;
	inthist_param.sigma = 0;
	inthist_param.htype = IntegralHistogram::directed;

	HOGExtractor::Param hog_param;
	hog_param.xgrid = 4;
	hog_param.ygrid = 4;
	hog_param.normalize = true;

	const char *fkp = 0;
	const char *fout = 0;
	const char *fin = 0;

	float kp_density = 1.0/8.0; // 1/grid_size
	int patch_size = 32;

	int c;
	while ((c = getopt(argc, argv, "i:o:k:d:p:h")) != -1)
	{
		switch (c)
		{
		case 'i':
			fin = optarg;
			break;
		case 'o':
			fout = optarg;
			break;
		case 'k':
			fkp = optarg;
			break;
		case 'd':
			kp_density = boost::lexical_cast<float>(optarg);
			break;
		case 'p':
			patch_size = boost::lexical_cast<int>(optarg);
			break;
		case 'h':
			help_exit(argv[0]);
			break;
		case '?':
		default:
			break;
		}
	}

	if (fin == 0)
	{
		help_exit(argv[0]);
	}

	Image im( fin );
	IntegralHistogram inthist(inthist_param);
	inthist.build(im);

	std::vector <int> kp_list[2];
	
	if (fkp != 0) //read keypoints from file
	{
		std::fstream kp_ins(fkp, std::ios::in);
		if (!kp_ins.is_open())
		{
			std::cerr << "can not open file: " << fkp << "\n";
			exit(-1);
		}
		while(true)
		{
			float x = std::numeric_limits<float>::quiet_NaN();
			float y = std::numeric_limits<float>::quiet_NaN();;
			kp_ins >> x >> y;
			if (x != x || y!=y) break;
			kp_list[0].push_back(static_cast<int>(x));
			kp_list[1].push_back(static_cast<int>(y));
		}
		kp_ins.close();
	}
	else //random sampling
	{
		float area = inthist.width() * inthist.height();
		int count = static_cast<int>(area * kp_density * kp_density);
		kp_list[0].resize(count);
		kp_list[1].resize(count);
		srand(time(0));
		for (int i = 0; i < count; ++i)
		{
			kp_list[0][i] = rand()%inthist.width();
			kp_list[1][i] = rand()%inthist.height();
		}
	}

	ublas::matrix<float> dscr(kp_list[0].size(), hog_param.xgrid * hog_param.ygrid * inthist_param.dirnum);
	ublas::matrix<int> bbox(kp_list[0].size(), 4);
	
	//std::cout << kp_list[0].size();

	for (unsigned int i = 0; i < kp_list[0].size(); ++i)
	{
		bbox(i, 0) = kp_list[0][i] - patch_size/2;
		bbox(i, 1) = kp_list[1][i] - patch_size/2;
		bbox(i, 2) = kp_list[0][i] + patch_size/2;
		bbox(i, 3) = kp_list[1][i] + patch_size/2;
	}
	for (int i = 0; i < kp_list[0].size();++i)
	  {
	    std::cout << bbox(i,0) <<"\t" << bbox(i,1) << "\t" << bbox(i,2) << "\t" << bbox(i,3)<<"\n";
	  }
	HOGExtractor extr(hog_param);
	extr.extract(dscr, bbox, inthist);

	std::streambuf *cout_buf_old = std::cout.rdbuf();
	std::fstream outs;
	if (fout != NULL && std::string(fout) != "/dev/stdout")
	{
		outs.open(fout, std::ios::out);
		if (!outs.is_open())
		{
			std::cerr << "can not open file: " << fout << "\n";
			exit(-1);
		}
		std::cout.rdbuf(outs.rdbuf());
	}
	std::cout << dscr.size2() << "\n" << dscr.size1() << "\n";
	for (unsigned int i = 0; i < kp_list[0].size(); ++i)
	{
		std::cout << kp_list[0][i] << " " << kp_list[1][i] << " " << patch_size << " ";
		for (unsigned int j = 0; j < dscr.size2(); ++j)
		{
			std::cout << dscr(i, j) << " ";
		}
		std::cout << "\n";
	}
	if (outs.is_open())
		outs.close();
	std::cout.rdbuf(cout_buf_old);

	//    inthist.save("foo.inthist");
	return 0;
}
