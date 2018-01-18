#ifndef __CHROMOSOMEPOOL_
#define __CHROMOSOMEPOOL_

#include "Chromosome.h"
#include <vector>
#include "../Network.h"
#include "../SequenceSet.h"

struct ChromosomePool
{
	vector<Chromosome*> pool; 
	
	vector<Chromosome*>::iterator begin()
	{
		return pool.begin();
	}

	vector<Chromosome*>::iterator end()
	{
		return pool.end();
	}
	
	ChromosomePool(Network* network);
	~ChromosomePool();
	
	void populate(unsigned int size);
	
	void evaluate(SequenceSet* sequence);
	
	void add(Chromosome* chromosome);
	Chromosome* get(unsigned int index);
	
	Network* network;
	
	unsigned int size() { return pool.size(); }
	
	void reduce_to(unsigned int size);
};

#endif /*CHROMOSOMEPOOL_*/
