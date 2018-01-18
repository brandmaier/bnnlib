/*
 * LSTMCascade.h
 *
 *  Created on: Oct 5, 2009
 *      Author: brandmaier
 */

#ifndef LSTMCASCADE_H_
#define LSTMCASCADE_H_

#include "Network.h"
#include "SequenceSet.h"
#include "callbacks/CallbackHandler.h"

struct LSTMCascade : CallbackHandler
{
	unsigned int max_growth;
	unsigned int max_epochs;

	weight_t learning_rate;
	weight_t momentum;

	Network* net;
	SequenceSet* sequenceset;

	LSTMCascade(SequenceSet* input);

	void train();

	Network* get_network();

	void callback(Network* net, Trainer* trainer, SequenceSet* sett);

};

#endif /* LSTMCASCADE_H_ */
