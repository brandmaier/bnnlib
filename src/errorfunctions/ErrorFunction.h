/*
 * ErrorFunction.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */

#ifndef __ERRORFUNCTION_H_
#define __ERRORFUNCTION_H_

#include "../functions.h"
#include <vector>

using namespace std;

struct ErrorFunction
{
	ErrorFunction() {};
	virtual ~ErrorFunction() {};

	/**
	 * returns the derivative of the ErrorFunction to the
	 * activation y_i of output unit i.
	 *
	 * In case of the common SquaredError-Function the
	 * result is (y_i-t_i).
	 **/
	virtual weight_t get_error_to_output(weight_t target, weight_t output) =0;

	/**
	 * get the total error between a single output and target
	 **/
	virtual weight_t get_error(weight_t target, weight_t output) =0;

	/**
	 * return error for a set of outputs and targets, e.g.
	 * a set of output nodes and teacher signals at a single timestep
	 * in a recurrent network.
	 */
	virtual weight_t get_error(vector<weight_t>* targets, vector<weight_t>* outputs)
	{
		double error = 0;
		for (unsigned int i=0; i < targets->size(); i++)
		{
			if (!isnan((*targets)[i]))
			error += get_error((*targets)[i], (*outputs)[i]);
		}
		return error;
	}

};

#endif /* ERRORFUNCTION_H_ */
