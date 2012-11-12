#ifndef HOG_WRAPPERS_H
#define HOG_WRAPPERS_H

#include <iostream>
#include <unistd.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>

#include "integral_histogram.h"
#include "hog_extractor.h"


ublas::matrix<float> extract_hog_features(const ublas::matrix<int> &bbox,  const char *fin);



#endif
