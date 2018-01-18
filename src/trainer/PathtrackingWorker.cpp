#include "PathtrackingWorker.h"
#include "PathtrackingTrainer.h"
#include <vector>
#include "../functions.h"

unsigned int PathtrackingWorker::WORKER_ID_COUNTER = 0;

PathtrackingWorker::PathtrackingWorker(PathtrackingTrainer* trainer, std::vector<weight_t>* weights, double lambda, double learning_rate)
{
	this->lambda = lambda;
	this->trainer = trainer;
	this->weights = weights;
	this->learning_rate = learning_rate;
	
	this->id = WORKER_ID_COUNTER++;
}

void PathtrackingWorker::iterate()
{
	network = trainer->network;
	
	std::cout << "Worker #" << this->id << " l="<< lambda << "iterates" << endl;
	print_vector(this->weights);	
	network->set_all_weights( weights );
	std::cout << "trainerror = " << train() << endl;
	this->weights = network->get_all_weights();
	print_vector(this->weights);
}

weight_t PathtrackingWorker::train()
{
	weight_t error = 0.0;
	trainer->trainer->reset();
	// trainiere mit valley 
	trainer->trainer->learning_rate = learning_rate * (1-lambda);
	std::cout << "LR = " << trainer->trainer->learning_rate << endl;
	for (unsigned int j=0; j < trainer->sequenceset_valley->size(); j++)
	{
	 Sequence* sequence = (*trainer->sequenceset_valley)[j];
	 // trainiere mit Ableitung (keine Gewichtsänderung)
	 trainer->trainer->_train_sequence(sequence, false);
	 error += (1-lambda)*network->get_sequence_error(sequence, false);
	}	


	
	network->reset(true);
	
	
	// trainiere mit top
	trainer->trainer->reset();
	trainer->learning_rate = learning_rate * lambda;
	for (unsigned int j=0; j < trainer->sequenceset_mountain->size(); j++)
	{
	 Sequence* sequence = (*trainer->sequenceset_mountain)[j];
	 // trainiere mit Ableitung (keine Gewichtsänderung)
	 trainer->trainer->_train_sequence(sequence, false);
  	 print_vector(network->get_all_partial_derivatives( ));
	 error += (lambda)*network->get_sequence_error(sequence, false);	 
	}		
 	print_vector(network->get_all_partial_derivatives( ));
	
	// change weights
	trainer->trainer->change_weights();
	
	return error;
}

bool PathtrackingWorker::has_converged()
{
	return false; //TODO
}
