#include "ChromosomePool.h"
#include "../Network.h"
#include <algorithm>

struct compareChromosomes {
  bool operator ()(Chromosome *lhs, Chromosome *rhs) { return *lhs < *rhs; }
};


ChromosomePool::ChromosomePool(Network* network)
{
	this->network = network;	
}

ChromosomePool::~ChromosomePool()
{
	for (unsigned int i=0; i < pool.size(); i++)
	{
		delete pool[i];	
	}	
}

void ChromosomePool::evaluate(SequenceSet* sequence)
{
	for (unsigned int i=0; i < size(); i++)
	{
		//TODO careful here...
		//if (pool[i]->fitness == 0.0) {
		 network->set_all_weights( &(pool[i]->dna) );
		 float sse = network->test_sequence(sequence);
		 float fitness = 1.0f/(1.0f+sse);
		 pool[i]->fitness = fitness;
		//}
		//std::cout << "Testing c " << i << " fitness = " << fitness << " (sse=" << sse << ")"<<endl;	
	}	
	
	sort(pool.begin(), pool.end(), compareChromosomes());
	reverse(pool.begin(), pool.end());
}

void ChromosomePool::populate(unsigned int size)
{
	unsigned int c_size = network->get_num_weights();
	
	Chromosome* master =  new Chromosome(c_size);
	master->from_network(network);

	
	for (unsigned int i=0; i < size; i++)
	{
		Chromosome* chromosome = master->clone();		
		chromosome->randomise();
		add(chromosome);	
			
	}
	

	
	delete master;
}

void ChromosomePool::add(Chromosome* chromosome)
{
	pool.push_back( chromosome );	
}

Chromosome* ChromosomePool::get(unsigned int index)
{
	return pool[index];	
}

void ChromosomePool::reduce_to(unsigned int size)
{
	if (this->size() < size) return;
	
	for (unsigned int i=this->size()-1; i >= size; i--)
	{
		//std::cout << i << endl;
		//Chromosome* c = pool[i];
		pool.erase( pool.begin()+i );
		//delete c;	
	}	
}
