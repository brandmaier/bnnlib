/*
 * RecursiveThermometricClassificator.h
 *
 *  Created on: 20.04.2010
 *      Author: andreas
 */

#ifndef RECURSIVETHERMOMETRICCLASSIFICATOR_H_
#define RECURSIVETHERMOMETRICCLASSIFICATOR_H_

#include "ClassificatorInterface.h"

class RecursiveThermometricClassificator : ClassificatorInterface
{
public:
	/*
	RecursiveThermometricClassificator()
	{

	}*/

	~RecursiveThermometricClassificator() {};

	unsigned int predict(vector<weight_t>* output)
	{
		vector<float> p_greater(output->size());

		p_greater[0] = (*output)[0];
		float p_equal = 1-(*output)[0];
		if (p_equal > p_greater[0]) return 0;
		for (unsigned int i=1; i < output->size(); i++)
		{
			p_greater[i] = ((*output)[i])*p_greater[i-1];
			p_equal = (1-(*output)[i])*p_greater[i-1];
			if (p_equal > p_greater[i]) return i;
		}
		return output->size()-1;
	}

};

#endif /* RECURSIVETHERMOMETRICCLASSIFICATOR_H_ */
