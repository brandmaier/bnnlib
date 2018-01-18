/*
 * PickBestGeneralizingNetwork.h
 *
 *  Created on: 04.05.2010
 *      Author: andreas
 */

#ifndef PICKBESTGENERALIZINGNETWORK_H_
#define PICKBESTGENERALIZINGNETWORK_H_

#include "CallbackHandler.h"
#include <float.h>


struct PickBestGeneralizingNetwork : CallbackHandler
{
	vector<weight_t>* weights;
	weight_t best_error;

	PickBestGeneralizingNetwork()
	{
		weights = NULL;
		best_error = FLT_MAX;	//TODO unsauber
	}

	~PickBestGeneralizingNetwork()
	{
		if (weights != NULL)
		{
			delete weights;
		}
	}

	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{

		if (trainer->get_current_validation_error() < best_error)
		{
			best_error = trainer->get_current_validation_error();
			if (weights != NULL) delete weights;
			weights = net->get_all_weights();
		}

	}

	vector<weight_t>* get_weights()
		{
		return weights;
		}

};

#endif /* PICKBESTGENERALIZINGNETWORK_H_ */
