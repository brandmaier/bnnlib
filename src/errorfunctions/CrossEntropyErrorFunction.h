/*
 * CrossEntropyErrorFunction.h
 *
 *  Created on: Nov 17, 2009
 *      Author: brandmaier
 */

#ifndef __CROSSENTROPYERRORFUNCTION_H_
#define __CROSSENTROPYERRORFUNCTION_H_

#include <cmath>
#include "ErrorFunction.h"

struct CrossEntropyErrorFunction : ErrorFunction
{
	weight_t get_error_to_output(weight_t target, weight_t output)
	{
		return output - target;
	}

	 weight_t get_error(weight_t target, weight_t output)
	 {
		 if (isnan(target)) return 0;
		 return target*log(output)+ (1-target)*log(output);
	 }

};

#endif /* CROSSENTROPYERRORFUNCTION_H_ */
