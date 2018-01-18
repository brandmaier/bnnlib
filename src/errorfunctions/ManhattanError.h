/*
 * MinkowskiError.h
 *
 *  Created on: Nov 24, 2009
 *      Author: brandmaier
 */

#ifndef MINKOWSKIERROR_H_
#define MINKOWSKIERROR_H_

#include <cmath>

struct MinkowskiError : ErrorFunction
{
	weight_t get_error_to_output(weight_t target, weight_t output)
	{
		return (output - target)>=0?+1:-1;
	}

	 weight_t get_error(weight_t target, weight_t output)
	 {
		 if (isnan(target)) return 0;
		 return fabs(output - target);
	 }

};

#endif /* MINKOWSKIERROR_H_ */
