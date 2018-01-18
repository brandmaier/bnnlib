/*
 * HarmonicGateNetwork.h
 *
 *  Created on: Jan 12, 2010
 *      Author: brandmaier
 */

#ifndef HARMONICGATENETWORK_H_
#define HARMONICGATENETWORK_H_


#include "Network.h"
#include "ensembles/Ensemble.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "ensembles/HarmonicGeneratorEnsemble.h"
#include "ensembles/GateEnsemble.h"

struct HarmonicGateNetwork : Network
{
	FeedforwardEnsemble* input;
	FeedforwardEnsemble* hidden;
	FeedforwardEnsemble* bias;
	FeedforwardEnsemble* oneshotbias;
	FeedforwardEnsemble* out;

	unsigned int generator_size;

	vector<Ensemble*> generators;

	HarmonicGateNetwork(unsigned int in_size, unsigned int num_generators, unsigned int hid_size, unsigned int out_size)
	{

		generator_size = 3;

		bias = new FeedforwardEnsemble(Node::BIAS_NODE,1);
		this->add_ensemble(bias);

		oneshotbias = new FeedforwardEnsemble(Node::ONE_SHOT_BIAS_NODE,1);
		this->add_ensemble(oneshotbias);


		input = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
		this->add_ensemble(input);

		hidden = new FeedforwardEnsemble(Node::LINEAR_NODE, hid_size);
		this->add_ensemble(hidden);
		out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
		this->add_ensemble(out);

		input->name = "InputLayer";
		out->name = "OutputLayer";
		hidden->name = "HiddenLayer";

		//this->connect_ensembles(oneshotbias, hidden, true);
		this->connect_ensembles(hidden, out, true);

		for (unsigned int i=0; i < num_generators; i++) {
			/*Ensemble* generator = new HarmonicGenerator(Node::TANH_NODE, generator_size);
					//new RecurrentEnsemble(Node::TANH_NODE, 3);
			this->add_ensemble(generator);
			generators.push_back(generator);
			this->connect_ensembles(bias, generator, true);
			this->connect_ensembles(generator, hidden, true);*/
			add_generator();
		}




	//	this->sort_nodes(input, out);

		//this->hid_offset-=this->in_size;
		//this->in_size=0;
	}

	void freeze_all_generators()
	{
		for (unsigned int i=0; i < generators.size();i++)
		{
			for (unsigned int j=0; j < generators[i]->nodes.size(); j++)
			{
				Node* node = generators[i]->nodes[j];
				for (unsigned int k=0; k < node->outgoing_connections.size(); k++)
				{
					(node->outgoing_connections[k])->freeze_weight = true;
				}
			}
		}
	}

	void add_generator()
	{
		//this->print_offsets();
		HarmonicGenerator* generator = new HarmonicGenerator(Node::TANH_NODE, generator_size);
		GateEnsemble* gate = new GateEnsemble(Node::LINEAR_NODE);
		this->add_ensemble(generator);
		this->add_ensemble(gate);
		generators.push_back(generator);

		this->connect_ensembles(oneshotbias, generator, true);

		Connection* fixed = this->connect(generator->get_output(), gate->get_signal(), true);
		fixed->set_identity_and_freeze();

		cout << "Insize:" << in_size << endl;
		for (unsigned int i=0; i < input->nodes.size();i++)
		{

		this->connect( input->nodes[i], gate->get_gate(), true);
		}
		this->connect((*bias->get_inputs())[0], gate->get_gate(), true);

		this->connect_ensembles(gate, hidden, true);

		sort_nodes(input, out);


		//this->print_offsets();
	}

};




#endif /* HARMONICGATENETWORK_H_ */
