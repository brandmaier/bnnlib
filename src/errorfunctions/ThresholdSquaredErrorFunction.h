/*
 * ThresholdSquaredErrorFunction.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */

#ifndef THRESHOLDSQUAREDERRORFUNCTION_H_
#define THRESHOLDSQUAREDERRORFUNCTION_H_

#include "ErrorFunction.h"
#include <cmath>

struct ThresholdSquaredErrorFunction : ErrorFunction
{
	double threshold;

	ThresholdSquaredErrorFunction(double threshold)
	{
		this->threshold = threshold;
	}

	weight_t get_error_to_output(weight_t target, weight_t output)
	{
		return output - target;
	}

	 weight_t get_error(weight_t target, weight_t output)
	 {
		if (isnan(target)) return 0;
		return (fabs(target-output) > threshold)?1:0;
	 }

};

#endif /* THRESHOLDSQUAREDERRORFUNCTION_H_ */
