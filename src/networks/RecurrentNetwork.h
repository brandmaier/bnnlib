/*
 * RecurrentNetwork.h
 *
 *  Created on: Mar 18, 2010
 *      Author: brandmaier
 */

#ifndef RECURRENTNETWORK_H_
#define RECURRENTNETWORK_H_

#include "../Network.h"

struct RecurrentNetwork : Network
{
//	Ensemble* in;
	Ensemble* hidden;
//	Ensemble* out;
	Ensemble* bias;

	RecurrentNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size)
	{
		input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
		hidden = new RecurrentEnsemble(Node::TANH_NODE, hid_size);
		output_ensemble= new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
		bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

		connect_ensembles(input_ensemble, hidden, true);
		connect_ensembles(hidden, output_ensemble, true);
		connect_ensembles(bias, output_ensemble, true);
		connect_ensembles(bias, hidden, true);

		//connect_ensembles(hidden, hidden, false); ensemble is already recurrent

		add_ensemble(input_ensemble);
		add_ensemble(hidden);
		add_ensemble(output_ensemble);
		add_ensemble(bias);

		sort_nodes(input_ensemble, output_ensemble);
	}
};

#endif /* RECURRENTNETWORK_H_ */
