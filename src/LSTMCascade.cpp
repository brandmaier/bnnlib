/*
 * LSTMCascade.cpp
 *
 *  Created on: Oct 5, 2009
 *      Author: brandmaier
 */

#include "LSTMCascade.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "nodes/Node.h"
#include <vector>
#include "ensembles/LSTMForgetEnsemble.h"
#include "trainer/Trainer.h"
#include "trainer/BackpropTrainer.h"

LSTMCascade::LSTMCascade(SequenceSet* sequence)
{
	this->sequenceset = sequence;

	max_growth = 4;
	max_epochs = 1000;

	learning_rate = 0.0001;
	momentum = 0.999;
}

void LSTMCascade::train()
{
	net = new Network();
	Trainer* trainer = new BackpropTrainer(net);

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,
			1);
	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE,
			this->sequenceset->input_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE,
			this->sequenceset->target_size);

	Ensemble* hid  = new LSTMForgetEnsemble(true, Node::LINEAR_NODE);

	vector<Ensemble*> hiddens = vector<Ensemble*>();
	hiddens.push_back(hid);

	// add nodes to network
	net->add_ensemble(bias);
	net->add_ensemble(in);
	net->add_ensemble(hid);
	net->add_ensemble(out);

	// initial connection matrix
	net->connect_ensembles(bias, out, true);
	net->connect_ensembles(bias, hid, true);
	net->connect_ensembles(in, out, true);
	net->connect_ensembles(in, hid, true);
	net->connect_ensembles(hid, out, true);
	net->connect_ensembles(hid, hid, false);



	// sort nodes
	net->sort_nodes();
	net->export_to_dot_graph("cascade");

	// train

	cout << "built network with size:" << this->sequenceset->input_size << "->" << this->sequenceset->target_size << " total:" << net->size <<endl;
	for (unsigned int i=0;i < max_growth; i++)
	{
		// train
		//cout << "train" << endl;
		trainer->learning_rate = learning_rate;
		trainer->momentum = momentum;
		trainer->add_callback(this, 100);
		trainer->train(sequenceset, sequenceset, max_epochs);

		std::vector<weight_t>* errors = &trainer->error_train;
		cout << (*errors)[errors->size()-1] << endl;

		if (i >= (max_growth-1)) break;  //yes, I know....

		// freeze all connections to the last hidden ensemble;
		vector<Node*>* input_nodes =  hiddens[hiddens.size()-1]->get_inputs();
		for (unsigned int j=0; j < input_nodes->size(); j++)
		{

			for (unsigned int k=0; k < (*input_nodes)[j]->incoming_connections.size(); k++)
			{
//				cout << "fixed weight from " << (*input_nodes)[j]->incoming_connections[k]->from->name << " to " <<
//						(*input_nodes)[j]->incoming_connections[k]->to->name << " at " << (*input_nodes)[j]->incoming_connections[k]->weight << endl;
				(*input_nodes)[j]->incoming_connections[k]->freeze_weight = true;

			}
		}

		// grow

		//Ensemble* hid = new FeedforwardEnsemble(Node::TANH_NODE, 1);
		Ensemble* hid = new LSTMForgetEnsemble(true, Node::LINEAR_NODE);
		hiddens.push_back( hid );
		net->add_ensemble( hid );

		// add connections
		net->connect_ensembles(in, hid, true);
		net->connect_ensembles(hid, out, true);
		net->connect_ensembles(bias, hid, true);
		for (unsigned int j=0; j < hiddens.size()-1; j++)
			net->connect_ensembles(hiddens[j], hid, true);
		net->connect_ensembles(hid, hid, false);

		// sort
		//cout << "sort again"<<endl;
		net->sort_nodes();
		//cout << "sorted"<<endl;
		net->export_to_dot_graph("cascade");


		delete errors;
	}

	//net->export_to_dot_graph();

	delete trainer;

	cout << "Done" << endl;
}

void LSTMCascade::callback(Network* net, Trainer* trainer, SequenceSet* set)
{
	//cout << "callback" << endl;
}

Network* LSTMCascade::get_network()
{
	return net;
}
