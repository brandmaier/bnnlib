/*
 * WinnerTakesAllErrorFunction.h
 *
 *  Created on: Dec 15, 2009
 *      Author: brandmaier
 */

#ifndef WINNERTAKESALLERRORFUNCTION_H_
#define WINNERTAKESALLERRORFUNCTION_H_

#include "ErrorFunction.h"

struct WinnerTakesAllErrorFunction : ErrorFunction
{

	weight_t get_error_to_output(weight_t target, weight_t output)
	{
		return output - target;
	}

	 weight_t get_error(weight_t target, weight_t output)
	 {
		 //return (output - target) * (output - target);
		 error("This method is not implemented for WinnerTakesAllError-functions!");
		 return -1;
	 }

	weight_t get_error(vector<weight_t>* targets, vector<weight_t>* outputs)
	{
			//double error = 0;
			int max_t_index = -1;
			int max_y_index = 0;
			for (unsigned int i=0; i < targets->size(); i++)
			{
				if (!isnan((*targets)[i])) {
					if (max_t_index == -1)  { max_t_index=i; }
					else if ((*targets)[i] > (*targets)[max_t_index]) max_t_index=i;
				}
				//if ((*targets)[i] > (*targets)[max_t_index]) max_t_index=i;
				if ((*outputs)[i] > (*outputs)[max_y_index]) max_y_index=i;
			}

			//cout << max_t_index << " "<<max_y_index << endl;

			// allow NaN target winner as "no error"! TODO is this always correct?
			if (isnan((*targets)[max_y_index])) {
				return 0;
			}

			if (max_t_index==-1) return 0;

			return (max_t_index==max_y_index)?0:1;
			//return error;
	}


};

#endif /* WINNERTAKESALLERRORFUNCTION_H_ */
