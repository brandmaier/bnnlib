/*
 * MSDTask.h
 *
 *  Created on: Jan 7, 2010
 *      Author: brandmaier
 */

#ifndef MSDTASK_H_
#define MSDTASK_H_

#include "Task.h"
#include "../networks/LSTMNetwork.h"
#include "../criteria/ZeroErrorCriterion.h"
#include "../misc/ParameterSet.h"
#include "../trainer/CGTrainer.h"

struct MSDTask : Task
{
	ErrorFunction* err_func;
	ParameterSet parameter_set;
	DataSet* dataset;

	double test_error;

	MSDTask() :
		//Task("datasets/nmsd-train-F30-0-10.dat","msd.bnn")
		Task("datasets/msd-train-F50-0-10.dat","msd.bnn")
	{
		err_func =
				new ThresholdSquaredErrorFunction(0.5);

		dataset = sequence_set->toDataSet(0.7, 0.15);


		parameter_set.set("network", 1);
	}

	void init()
	{


		/*if (parameter_set.get_int("network") == 0) {
			status("Stacked Slow");
			network = NetworkFactory::create_stacked_slow(1,num,hid_size, hid_slow, 1);
		} else {
			status("Recurrent");
			network = NetworkFactory::createRecurrentNetwork(1, Node::TANH_NODE, num, hid_size, Node::LINEAR_NODE, 1);
		}*/
		network = NetworkFactory::createCopyRestoreLSTM(1, 10, 1);

		network->error_function = err_func;

		trainer = new CGTrainer(network);
				//new ImprovedRPropTrainer(network);
		trainer->add_callback( new ReportCallback(), 500);

		trainer->add_callback(
				new ZeroErrorFailCallback(err_func),100
		);

		trainer->add_abort_criterion(
				new ZeroErrorCriterion( err_func )
			,20
		);



	}

	void train()
	{
		trainer->train( dataset->training_set, dataset->validation_set, 1000);
		network->save(network_name);

		test_error = network->evaluate_error(sequence_set);

		stringstream statusmsg;

		double err = network->evaluate_error(sequence_set);
		statusmsg << "Total Error: " << err << " epochs: " << trainer->total_iterations;

		status(statusmsg.str());
	}

	void test()
	{
		network->error_function = err_func;

		// choose Sequence
		Sequence* sequence = sequence_set->get(0);

		// plot activations over time
		network->activate( sequence );
		GnuplotGenerator::plot_all_activations(network, sequence );
		GnuplotGenerator::plot_input_output_target(network, sequence);

		// generate animation
		vector<Node*> outputs;
		for (unsigned int i=0; i < network->out_size; i++) {
			outputs.push_back( network->nodes[network->out_offset+i] );
		}
		vector<vector<weight_t>*>* data = network->collect_node_outputs(outputs);
		GnuplotGenerator::animate(data, &sequence->input, 1);

		// test sequence and print result
		test_error = network->evaluate_error(sequence_set);
				//network->test_sequence(sequence_set);



	}


};

#endif /* MSDTASK_H_ */
