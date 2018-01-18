#ifndef PATHTRACKINGWORKER_H_
#define PATHTRACKINGWORKER_H_

#include <vector>
#include "../functions.h"
#include "../Network.h"
#include "PathtrackingTrainer.h"

using namespace std;

struct PathtrackingWorker
{
	double lambda;
	Network* network;
	PathtrackingTrainer* trainer;
	vector<weight_t>* weights;
	double learning_rate;
	
	PathtrackingWorker(PathtrackingTrainer* trainer, std::vector<weight_t>* initial_weights, double lambda, double learning_rate);
	
	void iterate();
	weight_t train();
	
	bool has_converged();

	static unsigned int WORKER_ID_COUNTER;
	unsigned int id;
};

#endif /*PATHTRACKINGWORKER_H_*/
