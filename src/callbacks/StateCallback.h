/*
 * StateCallback.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */
#include "CallbackHandler.h"

struct StateCallback : CallbackHandler
{
	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		cout << "Initial States" << endl;
		print_vector( net->get_all_initialstates());
	}

};
