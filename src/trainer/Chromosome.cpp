#include "Chromosome.h"
#include <vector>
#include "../functions.h"
#include <iostream>
#include "stdlib.h"
#include <algorithm>

Chromosome::Chromosome(unsigned int size)
{
	dna = vector<weight_t>(size);		
	fitness = 0.0f;
	mutable_dna = vector<bool>(size);
	for (unsigned int i=0; i < size; i++) mutable_dna[i]=true;
}
	
	
Chromosome* Chromosome::clone()
{
	Chromosome* clone = new Chromosome( this->size() );
	
	std::copy( this->dna.begin(), this->dna.end(), clone->dna.begin() );
	std::copy( this->mutable_dna.begin(), this->mutable_dna.end(), clone->mutable_dna.begin());
	clone->fitness = fitness;
	
	return clone;	
}	
/**
 *  implements a simple two-point crossover procedure
 **/
Chromosome* Chromosome::breed(Chromosome* chromosome_b)
{
	// = new vector<float>( chromosome_a->size() );
	Chromosome* breed = this->clone();
	
	for (unsigned int j=0; j < 3; j++) {
	
	unsigned int index1 = rand(0, size()-1 );		
	unsigned int index2 = rand(index1+1, size() );

/*	for (unsigned int i=0; i < index1; i++)
	{
		(*breed)[i] = (*this)[i];
	}*/
	for (unsigned int i=index1; i < index2; i++)
	{
		(*breed)[i] = (*chromosome_b)[i];
	}
	/*for (unsigned int i=index2; i < size(); i++)
	{
		(*breed)[i] = (*this)[i];
	}*/

	}
	
	return breed;
}
	
void Chromosome::mutate(float mutation_probability, float mutation_mu, float mutation_sigma) 
{
	for (unsigned int i=0; i < size(); i++)
	{
		if (this->mutable_dna[i])
		 if (rand_uniform_01() < mutation_probability)
		 {
			float delta = rand_gauss(mutation_mu, mutation_sigma);
			//std::cout << "mutate "<< i<< " by delta="<<delta<<endl;
			(*this)[i] += delta;
		 }	
	}		
}

unsigned int Chromosome::size()
{
	return dna.size();
}

void Chromosome::randomise()
{
	for (unsigned int i=0; i < dna.size(); i++)
	{
		if (mutable_dna[i]) {
			float v = rand()/(RAND_MAX+1.0)*0.4-0.2;
		 	dna[i] = v;
		}
	}	
}

void Chromosome::from_network(Network* network)
{
	unsigned int num_weights = network->get_num_weights();
	dna.reserve(num_weights);
	unsigned int index = 0;
	for (unsigned int i=0; i < network->size; i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++)
		{
			dna[index] = network->nodes[i]->outgoing_connections[j]->weight;
			mutable_dna[index] = ! network->nodes[i]->outgoing_connections[j]->freeze_weight;
			index++;
		}
	}
}

void Chromosome::print()
{
	for (unsigned int i=0; i < size(); i++)
	std::cout << dna[i] << "\t";
	std::cout << endl;
}
