/*
 * TentmapTask.h
 *
 *  Created on: Jan 31, 2010
 *      Author: brandmaier
 */

#ifndef TENTMAPTASK_H_
#define TENTMAPTASK_H_

#include "Task.h"
#include "../misc/ParameterSet.h"
#include "../DataSet.h"
#include "../Network.h"

struct TentmapTask : Task
{
	ParameterSet parameter_set;
	DataSet* dataset;

	TentmapTask() : Task("datasets/tentmap.dat", "tentmap.bnn")
	{
		parameter_set.set("slowness", 0.001);
		sequence_set->normalize();
		dataset = sequence_set->toDataSet(0.5,0.25);
	}

	void init()
	{
		//unsigned int in_size = 1;
		//unsigned int hid_size = 10;
		//unsigned int out_size = 1;

		stringstream msg;
		msg << "Slowness at " << parameter_set.get_double("slowness");
		message(msg.str());
		unsigned int hid_size[] = {10,10,10};
		//unsigned int hid_size[] = {5,5,5};
		//unsigned int hid_slow[] = {0,0.001,0.01};
		unsigned int hid_slow[] = {0.01,0.1,1};

		unsigned int num = 3;
		if (parameter_set.get_bool("slowness")) {
			cout << "w slow" << endl;
			network =
					NetworkFactory::create_stacked_slow(1,num,hid_size, hid_slow, 1);
		} else {
			cout << "w/o slow" << endl;
		network =
		//		NetworkFactory::create_stacked_slow(1,3,hid_size, hid_slow, 1);
			NetworkFactory::createRecurrentNetwork(1,Node::TANH_NODE,num,hid_size, Node::LINEAR_NODE,1);
		}
		//new SlowNetwork(in_size, 20, out_size, parameter_set.get_double("slowness"));

//			network->add_wta_output_ensemble( network->ensembles[1], network->ensembles[network->ensembles.size()-1] );


		trainer = //new BackpropTrainer(network);
				new ImprovedRPropTrainer(network);
		//trainer->learning_rate = 0.0001;
		trainer->add_callback( new ReportCallback(), 50);
		trainer->add_callback( new SaveNetworkCallback("tentmap"), 50);

//		ErrorFunction* err_func = new WinnerTakesAllErrorFunction();
//			network->error_function = err_func;
		//trainer->add_callback( new ErrorEvaluationCallback(err_func), 10);
		//network->export_to_dot_graph("tent");

		trainer->add_abort_criterion(
				new GeneralizationLoss(dataset->validation_set,5),10, 500
		);
		trainer->add_abort_criterion(
				new ConvergenceCriterion(),1
		);

	}

	void train()
	{
		trainer->train( dataset->training_set, dataset->validation_set, 2000);
		network->save(network_name);

		stringstream title;
		title << "Error during training" << parameter_set.get_double("slowness");

		//GnuplotGenerator::plot_vectors(title.str(), &trainer->error_train, "training set",
		//		&trainer->error_validation, "validation set");
	}

	void test()
	{

		for (unsigned int i=0; i < 4; i++) {
			Sequence* sequence = dataset->test_set->get(i);

			AutoPredictor predictor(network, new DummyTransferFunction() );
			predictor.predict_and_plot( sequence, sequence->size(), 1);
		}

	}

};

#endif /* TENTMAPTASK_H_ */
