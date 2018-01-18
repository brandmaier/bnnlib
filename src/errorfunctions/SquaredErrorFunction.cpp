/*
 * SquaredErrorFunction.cpp
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */
#include "SquaredErrorFunction.h"

	SquaredErrorFunction::SquaredErrorFunction() {

	}

	weight_t SquaredErrorFunction::get_error_to_output(weight_t target, weight_t output)
	{
		return output - target;
	}

	 weight_t SquaredErrorFunction::get_error(weight_t target, weight_t output)
	 {
		 if (isnan(target)) return 0;

		 return (output - target) * (output - target);
	 }
