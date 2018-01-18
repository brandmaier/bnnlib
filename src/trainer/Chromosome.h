#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

#include <vector>
#include <iostream>
#include "../Network.h"

using namespace std;

struct Chromosome
{
	
	Chromosome();
	Chromosome(unsigned int size);
		
	vector<weight_t> dna;
	vector<bool> mutable_dna;
	float fitness;
	
	float get_fitness() { return fitness; }
	
	Chromosome* breed(Chromosome* chromosome_b);
	void mutate(float mutation_probability, float mutation_mu, float mutation_sigma);
	void randomise();
	
	void from_network(Network* network);
	
	Chromosome* clone();
	
	unsigned int size();
	
	void print();
	
	weight_t &operator[](unsigned int i) {
		return dna[i];	
	}
	
	bool operator==(Chromosome &other)  {
		return get_fitness() == other.get_fitness();	
	}
	
	bool operator>(Chromosome &other)  {
		return get_fitness() > other.get_fitness();	
	}	
	
	bool operator>=(Chromosome &other)  {
		return get_fitness() >= other.get_fitness();	
	}	
	
	bool operator<(Chromosome &other)  {
		//std::cout << " sort " << fitness << " vs " << other.fitness << "\n"; 
		return fitness < other.fitness;	
	}	
	
	bool operator<(const Chromosome &other) const  {
		return fitness < other.fitness;	
	}		
	
	bool operator<=(Chromosome &other)  {
		return get_fitness() <= other.get_fitness();	
	}		
	/*
	bool operator<(const Chromosome &me, const Chromosome &other)  {
		return me.get_fitness() < other.get_fitness();	
	}	*/	

		
};

#endif /*CHROMOSOME_H_*/
