/*
 * WinnerTakesAllClassificator.h
 *
 *  Created on: 20.04.2010
 *      Author: andreas
 */

#ifndef WINNERTAKESALLCLASSIFICATOR_H_
#define WINNERTAKESALLCLASSIFICATOR_H_

#include "ClassificatorInterface.h"

class WinnerTakesAllClassificator : ClassificatorInterface
{
public:

	unsigned int predict(vector<weight_t>* output)
	{
		unsigned int max_index=0;
		for (unsigned int j=1; j < output->size(); j++)
		{
			if ((*output)[max_index] < (*output)[j])
			{
				max_index = j;
			}
		}
		return max_index;
	}

	WinnerTakesAllClassificator() {}

	~WinnerTakesAllClassificator()
	{

	}

};

#endif /* WINNERTAKESALLCLASSIFICATOR_H_ */
