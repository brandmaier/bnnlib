/*
 * RecursiveThermometricErrorFunction.h
 *
 *  Created on: 20.04.2010
 *      Author: andreas
 */

#ifndef RECURSIVETHERMOMETRICERRORFUNCTION_H_
#define RECURSIVETHERMOMETRICERRORFUNCTION_H_

struct RecursiveThermometricErrorFunction
{
	/**
	 * returns the derivative of the ErrorFunction to the
	 * activation y_i of output unit i.
	 *
	 * In case of the common SquaredError-Function the
	 * result is (y_i-t_i).
	 **/
	virtual weight_t get_error_to_output(weight_t target, weight_t output)
	{
		return output - target;
	}

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

	virtual vector<weight_t>* get_error_to_output(vector<weight_t>* targets, vector<weight_t>* outputs)
	{
		vector<weight_t>* errors = new vector<weight_t>(targets->size());

		bool feedback = true;
		for (unsigned int i=0; i < targets->size();i++)
		{
			if (feedback)	(*errors)[i] = get_error((*targets)[i], (*outputs)[i]);
			else (*errors)[i] = 0;

			if ((*targets)[i] == 0) {
				feedback = false;
			}
		}

		return errors;
	}

};
#endif /* RECURSIVETHERMOMETRICERRORFUNCTION_H_ */
