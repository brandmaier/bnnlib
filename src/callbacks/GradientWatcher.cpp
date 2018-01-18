/*
 * GradientWatcher.cpp
 *
 *  Created on: Nov 4, 2009
 *      Author: brandmaier
 */
#include "GradientWatcher.h"

	GradientWatcher::GradientWatcher()
	{

	}

	void GradientWatcher::callback(Network* net,Trainer* trainer, SequenceSet* set)
	{

		double delta = net->check_gradient(set->get(0),10e-8,false);

		cout << "Gradient Delta " << delta << endl;

	}
