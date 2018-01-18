/*
 * SlowNetwork.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: brandmaier
 */

#include "SlowNetwork.h"


SlowNetwork::SlowNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size,double factor)
{
	unsigned int layer_sizes[] = {hid_size};
	init(in_size,1, layer_sizes, out_size,factor);
}

SlowNetwork::SlowNetwork(unsigned int num_layers, unsigned int layer_sizes[], double factor)
{
	init(1,num_layers, layer_sizes,1, factor);
}

void SlowNetwork::init(unsigned int in_size, unsigned int num_layers, unsigned int layer_sizes[], unsigned int out_size, double factor)
{
	//bool with_bias = true;
	nodetype hid_type = Node::TANH_NODE;
	nodetype out_type = Node::LINEAR_NODE;


	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	this->add_ensemble(bias);

	Ensemble* input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	this->add_ensemble(input_ensemble);
	Ensemble* hidden[num_layers];
	unsigned int total_hid_size = 0;
	for (unsigned int i=0; i < num_layers; i++)
	{
		hidden[i] = new RecurrentEnsemble(hid_type, layer_sizes[i]);
		this->add_ensemble(hidden[i]);
		total_hid_size += layer_sizes[i];
	}
	Ensemble* output_ensemble = new FeedforwardEnsemble(out_type, out_size);
	this->add_ensemble(output_ensemble);


	// connect ensembles

	this->connect_ensembles(in, hidden[0], true);
	for (unsigned int i=1; i < num_layers; i++)
	{
		for (unsigned int j=0; j < i; i++)
		{
			this->connect_ensembles(hidden[j], hidden[i], true);
		}
		this->connect_ensembles(bias, hidden[i], true);
	}
	this->connect_ensembles(hidden[num_layers-1], out, true);

	this->connect_ensembles(bias, hidden[0], true);
	this->connect_ensembles(bias, out, true);



	zero_target_node = new PseudoOutputNode();
	this->add_node(zero_target_node);

	Ensemble* hidden0 = hidden[0];
	for (unsigned int i=0; i < hidden0->nodes.size(); i++) {
		Connection* sc1 = this->connect( hidden0->nodes[i],
				zero_target_node, true);
		Connection* sc2 = this->connect( hidden0->nodes[i],
				zero_target_node, false);

		//double factor =  0.0; //0.001;
		sc1->freeze_weight = true;
		sc1->weight = i*factor;
		sc2->freeze_weight = true;
		sc2->weight = i*factor;
	}

	//sc->set_identity_and_freeze();


	this->sanity_allows_deadends = true;

	sort_nodes(in, out);

	this->print_offsets();
}
