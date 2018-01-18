/*
 * AutoAssociationSequenceSet.h
 *
 *  Created on: Feb 5, 2010
 *      Author: brandmaier
 */

#ifndef AUTOASSOCIATIONSEQUENCE_H_
#define AUTOASSOCIATIONSEQUENCE_H_

#include "Sequence.h"

struct AutoAssociationSequence : Sequence
{

	AutoAssociationSequence(Sequence* sequence)
	{
		/*target = sequence->input;
		input = sequence->input;*/
		for (unsigned int i=0; i < sequence->size(); i++)
			this->add( sequence->input[i], sequence->input[i]);
	}


};

#endif /* AUTOASSOCIATIONSEQUENCESET_H_ */
