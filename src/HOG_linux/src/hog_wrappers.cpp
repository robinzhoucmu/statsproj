#include "integral_histogram.h"
#include "hog_extractor.h"
#include "hog_wrappers.h"

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

