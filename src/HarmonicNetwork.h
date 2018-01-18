/*
 * HarmonicNetwork.h
 *
 *  Created on: Jan 11, 2010
 *      Author: brandmaier
 */

#ifndef HARMONICNETWORK_H_
#define HARMONICNETWORK_H_

#include "Network.h"
#include "ensembles/Ensemble.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "ensembles/HarmonicGeneratorEnsemble.h"

struct HarmonicNetwork : Network
{
	FeedforwardEnsemble* hidden;
	FeedforwardEnsemble* bias;
	FeedforwardEnsemble* out;

	unsigned int generator_size;

	vector<Ensemble*> generators;

	HarmonicNetwork(unsigned int in_size, unsigned int num_generators, unsigned int hid_size, unsigned int out_size)
	{

		//this->use_initial_states = true;

		generator_size = 3;

		bias = new FeedforwardEnsemble(Node::ONE_SHOT_BIAS_NODE,1);
		this->add_ensemble(bias);


		hidden = new FeedforwardEnsemble(Node::LINEAR_NODE, hid_size);
		this->add_ensemble(hidden);
		out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
		this->add_ensemble(out);



		for (unsigned int i=0; i < num_generators; i++) {
			/*Ensemble* generator = new HarmonicGenerator(Node::TANH_NODE, generator_size);
					//new RecurrentEnsemble(Node::TANH_NODE, 3);
			this->add_ensemble(generator);
			generators.push_back(generator);
			this->connect_ensembles(bias, generator, true);
			this->connect_ensembles(generator, hidden, true);
			*/
			add_generator();
		}


		this->connect_ensembles(bias, hidden, true);
		this->connect_ensembles(hidden, out, true);

		//vector<Node*> empty;
		//this->sort_nodes(&empty, out->nodes);

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
		Ensemble* generator = new HarmonicGenerator(Node::TANH_NODE, generator_size);
		this->add_ensemble(generator);
		generators.push_back(generator);
		this->connect_ensembles(bias, generator, true);
		this->connect_ensembles(generator, hidden, true);

		cout << "SORT" << endl;

		vector<Node*> empty;
		this->sort_nodes(&empty, &out->nodes);

		this->print_offsets();

	/*	this->hid_offset-=this->in_size;
		this->hid_size+=this->in_size;
		this->in_size=0;

		this->print_offsets();*/
	}

};

#endif /* HARMONICNETWORK_H_ */
