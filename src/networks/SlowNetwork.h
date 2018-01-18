/*
 * SlowNetwork.h
 *
 *  Created on: Jan 25, 2010
 *      Author: brandmaier
 */

#ifndef SLOWNETWORK_H_
#define SLOWNETWORK_H_

#include "../Network.h"
#include "../Connection.h"
#include "../ensembles/FeedforwardEnsemble.h"
#include "../ensembles/RecurrentEnsemble.h"
#include "../nodes/PseudoOutputNode.h"
#include "../ensembles/Ensemble.h"

struct SlowNetwork : Network
{
	FeedforwardEnsemble* in;
	FeedforwardEnsemble* out;
	Node* zero_target_node;

	SlowNetwork(unsigned int in_size,unsigned int hid_size, unsigned int out_size, double factor);
	SlowNetwork(unsigned int num_layers, unsigned int layer_sizes[], double factor);

	void init(unsigned int in_size, unsigned int num_layers, unsigned int layer_sizes[], unsigned int out_size, double factor);
};

#endif /* SLOWNETWORK_H_ */
