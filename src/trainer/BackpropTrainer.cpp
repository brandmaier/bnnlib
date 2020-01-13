/*
 * BackpropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "BackpropTrainer.h"

#include <vector>
#include "../functions.h"
#include "../SequenceSet.h"
#include "../Sequence.h"


using namespace std;

BackpropTrainer::BackpropTrainer() : Trainer()
{
  // NOOP
}

BackpropTrainer::BackpropTrainer(Network* network) : Trainer()
{
	this->network = network;
}

void BackpropTrainer::change_weight(Trainable* conn)
{
			if (conn->freeze_weight == true){
				conn->previous_derivative = conn->derivative;
				conn->derivative = 0.0;
				conn->previous_weight_change = 0.0;
				return;
			}

			weight_t change = momentum*learning_rate*conn->derivative
					+ (1.0-momentum)*conn->previous_weight_change;

			conn->weight += change;
			conn->previous_weight_change = change;
			conn->previous_derivative = conn->derivative;
			conn->derivative = 0.0;

}
