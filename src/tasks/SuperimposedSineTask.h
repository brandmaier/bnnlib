/*
 * SuperimposedSineTask.h
 *
 *  Created on: Jan 7, 2010
 *      Author: brandmaier
 */

#ifndef SUPERIMPOSEDSINETASK_H_
#define SUPERIMPOSEDSINETASK_H_

#include "Task.h"
#include "../nodes/Node.h"

struct SuperimposedSineTask : Task
{
	SuperimposedSineTask() : Task("datasets/superimposedsin.dat","superimposedsin.bnn")
	{

	}

	void init()
	{
		unsigned int in_size = 1;
		unsigned int hid_size = 5;
		unsigned int out_size = 1;
		network = new LSTMNetwork(in_size,hid_size,out_size
				);


		trainer = new ImprovedRPropTrainer(network);
		trainer->add_callback( new ReportCallback(), 100);
	}

	void train()
	{
		trainer->train( sequence_set, 1000);
		network->save(network_name);
	}

	void test()
	{
		network->test_sequence(sequence_set, true);
	}

};

#endif /* SUPERIMPOSEDSINETASK_H_ */
