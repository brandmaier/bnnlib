/*
 * PSOTrainer.h
 *
 *  Created on: Dec 10, 2009
 *      Author: brandmaier
 */

#ifndef PSOTRAINER_H_
#define PSOTRAINER_H_

#include "Trainer.h"
#include "../Network.h"
#include "../SequenceSet.h"

struct Particle
{
	vector<weight_t> velocity;
	vector<weight_t> position;
	vector<weight_t> local_best_position;
	double error;
	double local_best_error;
	Network* network;

	Particle(Network* network)
	{
		this->network = network;
		from_network();
		velocity = vector<weight_t>( position.size() );
		local_best_position = vector<weight_t>(position.size());
		for (unsigned int i=0; i < position.size(); i++)
		{
			local_best_position[i] = position[i];
		}
	}

	void from_network()
	{

		vector<weight_t>* temp = network->get_all_weights();
		position = vector<weight_t>(temp->size());
		for (unsigned int i=0; i < temp->size();i++)
		{
			position[i] = (*temp)[i];
		}
		delete temp;
	}

	void to_network()
	{
		network->set_all_weights( &position );
	}

	void update_position()
	{
		for (unsigned int i=0; i < velocity.size();i++)
		{
			position[i] += velocity[i];
		}
	}

	void evaluate(SequenceSet* set)
	{
		to_network();
		error = network->evaluate_error(set);
	}
};

struct PSOTrainer : Trainer
{
	unsigned int num_particles;
	vector<Particle*> particles;

	PSOTrainer(Network* network, unsigned int num_particles);

	void train(SequenceSet* sequenceset,SequenceSet* testset,  unsigned int iterations);

	void change_weight(Trainable* c)
	{
		// NOOP
	}

};

#endif /* PSOTRAINER_H_ */
