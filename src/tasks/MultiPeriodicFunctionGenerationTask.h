/*
 * PeriodicFunctionGenerationTask.h
 *
 * The network receives a 1-out-of-n coding and has
 * to generate periodic patterns according to this input.
 *
 *  Created on: Jan 8, 2010
 *      Author: brandmaier
 */

#ifndef MULTIPERIODICFUNCTIONGENERATIONTASK_H_
#define MULTIPERIODICFUNCTIONGENERATIONTASK_H_

#include "Task.h"

struct MultiPeriodicFunctionGenerationTask : Task
{
	MultiPeriodicFunctionGenerationTask() : Task("datasets/pfg-cos.dat","pfg.bnn")
	{

	}

	void init()
	{
		network =
		//		NetworkFactory::create_stacked_slow(1,3,hid_size, hid_slow, 1);
			NetworkFactory::createRecurrentNetwork(1,Node::TANH_NODE,num,hid_size, Node::LINEAR_NODE,1);


		trainer = //new BackpropTrainer(network);
			new ImprovedRPropTrainer(network);
		//trainer->learning_rate = 0.0001;
		trainer->add_callback( new ReportCallback(), 50);
		trainer->add_callback( new SaveNetworkCallback("tentmap"), 50);
	}

	virtual void train()
	{

	}
};

#endif /* PERIODICFUNCTIONGENERATIONTASK_H_ */
