#include "GeneticTrainer.h"	
#include <vector>
#include "../Network.h"
#include "ChromosomePool.h"
#include <time.h>
#include "BackpropTrainer.h"

using namespace std;	
	
GeneticTrainer::GeneticTrainer(Network* network)
{
		this->network = network;
		trainer = new BackpropTrainer(network);
		
		mutation_probability = 0.1;
		mutation_mu = 0.0;
		mutation_sigma = 0.05;
		
		pool = new ChromosomePool(network);
		pool->populate(100);
		
		std::cout << "ChromosomePool instantiated" << endl;
}

GeneticTrainer::~GeneticTrainer()
{
	delete pool;	
}
	
vector<float>* GeneticTrainer::train(SequenceSet* set, unsigned int iterations)
{

	time_t starttime = time(0);

	unsigned int p_size = pool->size();

	unsigned int best = sqrt(p_size);

	pool->evaluate(set);	

	float last_best = 0.0f;
	unsigned int p_counter=0;
	
	vector<float>* sse_errors = new vector<float>();
	sse_errors->reserve(iterations);

	for (unsigned int i=0; i < iterations; i++)
	{
		//std::cout << "breed"<<endl;
		// breed 
		for (unsigned int j=0; j < best; j++)
		{
			for (unsigned int k=0; k < best; k++)
			{
				Chromosome* offspring = pool->get(j)->breed(pool->get(k));
				offspring->mutate(mutation_probability, mutation_mu, mutation_sigma);
				
				network->set_all_weights( &offspring->dna );
				//network->train(set, 1);
				trainer->train(set, 1);
				offspring->from_network( network );
				
				pool->add( offspring );
			}			
		}
	
		//std::cout << "eval" << endl;
	
		pool->evaluate(set);	
		//std::cout << "reduce" << endl;		
		pool->reduce_to(p_size);
	
		float sse = (1.0f/pool->get(0)->fitness)-1;
		std::cout << i << "th step. current best " << pool->get(0)->fitness << "(SSE=" << sse << ")" 
		 << " elapsed time= " <<((time(0)-starttime)/60.0) << "min" << endl;
	
		if (abs(last_best - pool->get(0)->fitness) < 10e-3) 
		{
			p_counter++;
		} else {
			p_counter = 0;	
		}
		
		if (p_counter >= 3){
			std::cout << "Plateau reached!" <<endl;
			break;	
		}
	
		(*sse_errors)[i] = pool->get(0)->fitness;
	
		last_best = pool->get(0)->fitness;
		// evaluate
		//pool->evaluate(set);				
	}
	
	network->export_to_dot_graph("genetic");
	
	// set network to best candidate
	network->set_all_weights( &pool->get(0)->dna );

	return sse_errors;

}

