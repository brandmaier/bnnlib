#ifndef __GENETIC_TRAINER_H_
#define __GENETIC_TRAINER_H_

#include "../Network.h"
#include <vector>
#include "ChromosomePool.h"
#include "Trainer.h"

struct GeneticTrainer
{
	float mutation_probability;
	float mutation_mu;
	float mutation_sigma;
	
	ChromosomePool* pool;
	
	GeneticTrainer(Network* network);
	~GeneticTrainer();
	
	vector<float>* train(SequenceSet* set, unsigned int iterations);
	
	Network* network;
	Trainer* trainer;
	

	
};

#endif
