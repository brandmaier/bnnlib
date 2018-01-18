/*
 * Architecture.h
 *
 *  Created on: Jan 18, 2010
 *      Author: brandmaier
 */

#ifndef ARCHITECTURE_H_
#define ARCHITECTURE_H_

/**
 * Architecture represents a certain network architecture, i.e. a
 * certain concept of network, a certain connectivity of nodes, together
 * with a recommended training architecture.
 *
 */

#include "../DataSet.h"
#include "../classification/AutoPredictor.h"

struct Architecture
{
	Network* network;
	Trainer* trainer;
	DataSet* dataset;

	Architecture(DataSet* dataset) {
		this->dataset = dataset;
	}

	// ~Architecture() {};

	virtual void init() {}

	virtual void train()
	{
		trainer->train(dataset->training_set,
				dataset->validation_set, 2000);

		//trainer->plot_errors();
	}

	virtual void test()
	{
		Sequence* sequence = dataset->test_set->get(0);
		AutoPredictor predictor(network);
		predictor.predict_and_plot(sequence,100,1);
		predictor.predict_and_plot(sequence,1000,200);
	}


};

#endif /* ARCHITECTURE_H_ */
