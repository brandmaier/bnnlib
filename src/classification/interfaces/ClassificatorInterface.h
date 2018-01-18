/*
 * ClassificatorInterface.h
 *
 *  Created on: 20.04.2010
 *      Author: andreas
 */

#ifndef CLASSIFICATORINTERFACE_H_
#define CLASSIFICATORINTERFACE_H_

#include <vector>

class ClassificatorInterface
{
public:

	virtual unsigned int predict(vector<weight_t>* output) = 0 ;

	virtual ~ClassificatorInterface() {};
};

#endif /* CLASSIFICATORINTERFACE_H_ */
