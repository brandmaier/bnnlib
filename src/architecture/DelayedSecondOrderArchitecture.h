/*
 * DelayedSecondOrderArchitecture.h
 *
 *  Created on: Jan 18, 2010
 *      Author: brandmaier
 */

#ifndef DELAYEDSECONDORDERARCHITECTURE_H_
#define DELAYEDSECONDORDERARCHITECTURE_H_

#include "Architecture.h"
#include "../Network.h"
#include "../ensembles/FeedforwardEnsemble.h"
#include "../ensembles/DelayEnsemble.h"
#include "../ensembles/SecondOrderEnsemble.h"

struct DelayedSecondOrderArchitecture : Architecture
{
	DelayedSecondOrderArchitecture(DataSet* dataset) : Architecture(dataset)
		{
			init();
		}

	//~DelayedSecondOrderArchitecture() {

//	}

	virtual void init()
	{
		unsigned int hid_size = 20;

		bool with_delay = true;

		network = new Network();

		FeedforwardEnsemble* input = new FeedforwardEnsemble(Node::LINEAR_NODE, dataset->get_input_size());
		FeedforwardEnsemble* output = new FeedforwardEnsemble(Node::LINEAR_NODE, dataset->get_target_size());

		Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

		DelayEnsemble* delay;
		DelayEnsemble* delay2;
		if (with_delay) {
			unsigned int delay_time = 1;
			delay = new DelayEnsemble(input, delay_time);
			delay2 = new DelayEnsemble(input, delay_time+1);
			network->add_ensemble(delay);
			network->add_ensemble(delay2);
		}

		Ensemble* hidden = new RecurrentEnsemble(Node::TANH_NODE, hid_size);

		vector<Node*> nodes;
		nodes.insert( nodes.end(), input->nodes.begin(),input->nodes.end() );
		if (with_delay) {
			nodes.insert( nodes.end(), delay->nodes.begin(),delay->nodes.end() );
			nodes.insert( nodes.end(), delay2->nodes.begin(),delay2->nodes.end() );

		}

		SecondOrderEnsemble* second = new SecondOrderEnsemble(&nodes, &hidden->nodes);

	//	network->connect_ensembles(input, hidden, true);

		network->connect_ensembles(bias, output, true);
		network->connect_ensembles(bias, hidden, true);
		network->connect_ensembles(hidden, output, true);

		network->add_ensemble(input);

		network->add_ensemble(hidden);
		network->add_ensemble(second);
		network->add_ensemble(output);
		network->add_ensemble(bias);

		network->sort_nodes(input, output);


		trainer = /*new BackpropTrainer(network);
		trainer->learning_rate = 0.001;
		trainer->momentum = 0.99;*/
				new ImprovedRPropTrainer(network);
		trainer->add_abort_criterion(new ConvergenceCriterion(),1);
		trainer->add_callback( new ReportCallback(), 25);
		trainer->add_callback( new SaveNetworkCallback("temp"), 50);

		cout << "built network, inp=" << network->nodes[1]->name << endl;

		//network->print_offsets();
		//network->export_to_dot_graph("test");

	}



};

#endif /* DELAYEDSECONDORDERARCHITECTURE_H_ */
