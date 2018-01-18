#ifndef __PATHTRACKINGTRAINER_H_
#define __PATHTRACKINGTRAINER_H_

#include "../Network.h"
#include "Trainer.h"

struct PathtrackingTrainer
{
	Network* network;
	SequenceSet* sequenceset_valley; // valley
	SequenceSet* sequenceset_mountain;	 //mountaintop
	Trainer* trainer;

	PathtrackingTrainer(Network* network, double learning_rate);
	
	void train(SequenceSet* sequenceset);
	void debug();
	
	double learning_rate;
	
	double step_size;
	unsigned int max_workers;
	double base_camp;
	unsigned int max_iterations;
};

#endif /*PATHTRACKINGTRAINER_H_*/
