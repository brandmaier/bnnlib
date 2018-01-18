/*
 * CombinedErrorFunction.h
 *
 *  Created on: Dec 27, 2009
 *      Author: brandmaier
 */

#ifndef COMBINEDERRORFUNCTION_H_
#define COMBINEDERRORFUNCTION_H_

struct CombinedErrorFunction : ErrorFunction
{
	vector<ErrorFunction*> error_functions;

	CombinedErrorFunction(ErrorFunction* error_function1, ErrorFunction* error_function2)
	{
		error_functions.push_back(error_function1);
		error_functions.push_back(error_function2);

	}

	CombinedErrorFunction(vector<ErrorFunction*> &error_functions)
	{
		for (unsigned int i=0; i <error_functions.size(); i++)
		{
			this->error_functions.push_back( error_functions[i] );
		}
	}

	weight_t get_error_to_output(weight_t target, weight_t output)
	{
		weight_t temp = 0.0;
		for (unsigned int i=0; i <error_functions.size(); i++)
		{
			temp += error_functions[i]->get_error_to_output(target, output);
		}
		return temp;

	}

	 weight_t get_error(weight_t target, weight_t output)
	 {
		 weight_t temp = 0.0;
		 for (unsigned int i=0; i < error_functions.size(); i++)
		 {
			 temp += error_functions[i]->get_error(target,output);
		 }
		 return temp;
	 }

};

#endif /* COMBINEDERRORFUNCTION_H_ */
