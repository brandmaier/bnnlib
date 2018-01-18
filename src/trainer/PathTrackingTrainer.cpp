#include "PathtrackingTrainer.h"
#include "PathtrackingWorker.h"
#include <vector>
#include "../Network.h"
#include "BackpropTrainer.h"

PathtrackingTrainer::PathtrackingTrainer(Network* network, double learning_rate)
{
	this->network = network;	
	this->trainer = new BackpropTrainer(network);
	this->learning_rate = learning_rate;
	
	max_workers = 3;
	max_iterations = 10;
}

void PathtrackingTrainer::train(SequenceSet* sequenceset)
{
	assert(learning_rate > 0);
	this->sequenceset_mountain = sequenceset;
	
	std::cout << "Start training lr="<< learning_rate << endl;
	
	// guess parameters for E_valley
	std::cout << "generate initial weights" << endl;
	network->randomize();
	vector<weight_t>* weights = network->get_all_weights();
	// generate targets for E_valley
	std::cout << "generate initial sequence" << endl;
	sequenceset_valley = new SequenceSet();
	for (unsigned int j=0; j < sequenceset_mountain->size(); j++)
	{
	 std::cout << "." << endl;
	 Sequence* sequence = (*sequenceset_mountain)[j];
	 Sequence* temp_seq = new Sequence();
	 network->activate(sequence);
	 
	 for (unsigned int i=0; i < sequence->size(); i++) {
	 	vector<weight_t>* input = sequence->get_input(i);
	 	vector<weight_t>* target = network->get_output(i);
	 	temp_seq->add(input, target);
	 }
	 sequenceset_valley->add_sequence( temp_seq );
	}	
	
	std::cout << "saving sequence " << endl;
	sequenceset_valley->save_to_file("temp.dat");
	
	vector<PathtrackingWorker*> workers;
	workers.reserve(max_workers);
	// spawn first worker
	PathtrackingWorker* w1 = new PathtrackingWorker(this, weights, 0.5, learning_rate);
	double base_camp = 0.0;
	double last_spawned_worker = 0.5;

	workers.push_back( w1);
	
	// -- iterate --  (main loop)
	for (unsigned int i=0; i < max_iterations; i++) {
	// iterate all workers (one step of training for each)
		for (unsigned int j=0; j < workers.size(); j++)
			workers[j]->iterate();
			
	// check if any worker has converged
		for (unsigned int j=0; j < workers.size()-1; j++)
		{
			if (workers[j]->has_converged()) {
				// delete all workers but this one TODO
				std::vector<PathtrackingWorker*>::iterator it;
				for (it = workers.begin(); it < workers.end(); it++)
				{
					delete *it;
					workers.erase(it);	
				}
				
				// move Basecamp
				base_camp = workers[j]->lambda;
				last_spawned_worker = base_camp;
			}	
		}	
	// if basecamp has converged, move it by delta, delete all other workers
		if (workers[workers.size()-1]->has_converged())
		{
			std::cout << "Convergence reached in lambda=1"<<endl;
			break;	
		}
		
		if (workers.size() < max_workers)
		{
			// spawn worker
			double newlambda = (1.0-base_camp)/ (2 <<(workers.size()) );
			std::cout << "Spawn worker at l="<<newlambda<<endl;	
			last_spawned_worker = newlambda;
			PathtrackingWorker* newworker = new PathtrackingWorker(this, weights, newlambda, learning_rate);
			workers.push_back(newworker);
		}
	
	}
	
}

void PathtrackingTrainer::debug()
{
	
}
