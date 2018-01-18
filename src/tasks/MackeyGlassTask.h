/*
 * MackeyGlassTask.h
 *
 *  Created on: Jan 26, 2010
 *      Author: brandmaier
 */

#ifndef MACKEYGLASSTASK_H_
#define MACKEYGLASSTASK_H_

#include "../trainer/CGTrainer.h"

struct MackeyGlassTask : Task
{
	DataSet* dataset;

	MackeyGlassTask() : Task("datasets/mackey-glass-5000.dat", "mackeyglass.bnn")
	{
		//sequence_set->normalize();
		//dataset = sequence_set->toDataSet(0.5,0.25);
		sequence_set->get(0)->split_in_training_validation_test_set(0.7,0.15);
		dataset = new DataSet(sequence_set);
	}

	void init()
	{
		//unsigned int hid_size[] = {10,10,10,10};
		//unsigned int hid_size[] = {5,5,5};
		//unsigned int hid_slow[] = {0,0.001,0.01};
		//unsigned int hid_slow[] = {0.001,0.01,0.1,1};
		//unsigned int num = 4;
	//	network = NetworkFactory::create_stacked_slow(1,num,hid_size, hid_slow, 1);
		network = new LSTMNetwork(1, 20, 1);
		trainer = //new BackpropTrainer(network);
				new ImprovedRPropTrainer(network);
			//new CGTrainer(network);
		//trainer->learning_rate = 0.0001;
		trainer->add_callback( new ReportCallback(), 50);
		trainer->add_callback( new SaveNetworkCallback("mackeyglass"), 50);

		Criterion* criterion = new ErrorBelowCriterion(
				new SquaredErrorFunction(),2500*0.00001
		);

		trainer->add_abort_criterion(criterion,1);

	}

	void train()
	{
		trainer->train( sequence_set, 5000);
		network->save(network_name);

		//stringstream title;
	//	title << "Error during training" << parameter_set.get_double("slowness");

		//GnuplotGenerator::plot_vectors(title.str(), &trainer->error_train, "training set",
		//		&trainer->error_validation, "validation set");
	}

	void test()
	{
		Sequence* sequence =sequence_set->get(0);

		AutoPredictor predictor(network, new DummyTransferFunction() );
		predictor.predict_and_plot( sequence, sequence->size(), 1);

	//Sequence* sequence =sequence_set->get(0);

		AutoPredictor predictor2(network, new LinearTransferFunction() );
		predictor2.predict_and_plot( sequence, sequence->size(), 1000);
	}

};

#endif /* MACKEYGLASSTASK_H_ */
