/*
 * metrics.hpp
 *
 *  Created on: 20 Apr 2016
 *      Author: papadoma
 */

#ifndef METRICS_HPP_
#define METRICS_HPP_

#include <opencv2/opencv.hpp>

using namespace cv;

class Quality_Metrics{
private:

public:
  double getMSSIM( const Mat& i1, const Mat& i2);
  double getPSNR(const Mat& I1, const Mat& I2);
};


#endif /* METRICS_HPP_ */
