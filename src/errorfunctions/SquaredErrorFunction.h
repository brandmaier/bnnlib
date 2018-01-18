/*
 * SquaredErrorFunction.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */

#ifndef SQUAREDERRORFUNCTION_H_
#define SQUAREDERRORFUNCTION_H_

#include "ErrorFunction.h"

struct SquaredErrorFunction : ErrorFunction
{
	SquaredErrorFunction();

	weight_t get_error_to_output(weight_t target, weight_t output);

	 weight_t get_error(weight_t target, weight_t output);

};

#endif /* SQUAREDERRORFUNCTION_H_ */
