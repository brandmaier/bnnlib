/*
 * PSOTrainer.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: brandmaier
 */

#include "PSOTrainer.h"
#include "ImprovedRPropTrainer.h"

PSOTrainer::PSOTrainer(Network* network, unsigned int num_particles)
{
	this->network = network;
	this->num_particles = num_particles;


}

void PSOTrainer::train(SequenceSet* sequenceset,SequenceSet* testset,  unsigned int iterations)
{

	Trainer* trainer = new ImprovedRPropTrainer(network);

	// create Particles
	for (unsigned int i=0; i < num_particles; i++)
	{
		network->randomize();
		trainer->train(sequenceset, 100);
		Particle* particle = new Particle(network);
		particles.push_back( particle );
	}

	for (unsigned int i=0; i < sequenceset->size(); i++)
	{
		network->reserve( sequenceset->get(i)->size());
	}

	// evaluate all particles
	for (unsigned int p=0; p < num_particles; p++)
		particles[p]->evaluate(sequenceset);

	// find global best
	unsigned int best = 0;
	for (unsigned int p=1; p < num_particles; p++)
		if (particles[p]->error < particles[best]->error)
			best = p;

	double omega = 0.95;
	double c1 = 0.07;
	double c2 = .09;

	vector<weight_t> global_best( particles[best]->position.size());
	for (unsigned int k=0; k < global_best.size();k++)
		global_best[k] = particles[best]->position[k];
	weight_t global_best_error = particles[best]->error;

	for (unsigned int i=0; i < iterations; i++)
	{
		//cout << "Iteration " << i << endl;

		for (unsigned int p=0; p < num_particles; p++)
		{
			Particle* particle = particles[p];

			for (unsigned int j=0; j < particle->position.size();j++)
			{
				double r1 = rand_uniform_01();
				double r2 = rand_uniform_01();

				particle->velocity[j] = omega*particle->velocity[j]+
						c1 * r1 * (-particle->position[j]+particle->local_best_position[j]) +
						c2 * r2 * (-particle->position[j]+global_best[j]);

			}

			particle->update_position();
			particle->evaluate(sequenceset);

			// update local best
			if (particle->error <= particle->local_best_error)
			{
				particle->local_best_error = particle->error;
				for (unsigned int k=0; k < particle->position.size();k++)
					particle->local_best_position[k] = particle->position[k];
			}

			// update global best
			if (particle->error < global_best_error)
			{
				cout << "Best Error: " << particle->error << " was: " << particles[best]->error << " nr:" << best << endl;

				global_best_error = particle->error;
				for (unsigned int k=0; k < global_best.size();k++)
					global_best[k] = particle->position[k];
			}


		}


	}
}
