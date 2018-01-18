/*
 * LineSearch.cpp
 *
 *  Created on: Dec 22, 2009
 *      Author: brandmaier
 */

#include "LineSearch.h"

LineSearch::LineSearch(Network* network,vector<weight_t>* direction) {
	this->network = network;
	this->direction = direction;

}

LineSearch::~LineSearch() {

}

void LineSearch::train(SequenceSet* sequenceset, unsigned int iterations)
{
	//cout << "performing line search";

	// evaluate points a,b,c
	vector<weight_t>* original_weights = network->get_all_weights();

	// initialise variables
	double alpha = 0.01;
	//bool dir = true;

	double E_a = evaluate(original_weights, 0, sequenceset);
	double alpha_a = 0.0001;
	//cout << "alpha=0 E=" << E_a << endl;


	best_alpha = alpha_a;

	bool cont = true;
	int c = 0;
	unsigned int c_max = 10;
	while(cont)
	{
		double alpha_test = alpha_a*1.9;
		double E_b = evaluate(original_weights, alpha_test, sequenceset);
		//cout << c << ".:" <<" alpha="<<alpha_test<<" E=" << E_b << endl;

		if (isnan(E_b)) {
			warning("Error has become NaN!");
			break;
		}

		if (E_b > E_a) {
			alpha_a *= 0.5;
		} else {
			// continue
			alpha_a = alpha_test;
		}

		best_alpha = alpha;
		//cout << "new alpha: " << alpha_a << endl;

		c++;

		if (c>c_max)
			break;
	}

	network->set_all_weights(original_weights);
}

double LineSearch::evaluate(vector<weight_t>* weights, double alpha, SequenceSet* sequenceset)
{
//	cout << "evaluate" << alpha << "," << weights->size() <<  endl;
	vector<weight_t> w(weights->size());
	for (unsigned int i=0; i < weights->size(); i++)
	{
		w[i] = (*weights)[i] + alpha*(*direction)[i];
		//cout << w[i]<<"=" << (*weights)[i] << "+"<< alpha<<"*"<<alpha*(*direction)[i]<<endl;
	}
	//cout << endl;
	//cout << "set" << endl;
	this->network->set_all_weights(&w);
	//cout << "eval" << endl;
	double result = this->network->evaluate_error(sequenceset);
//	cout << "return" << endl;

	return result;
}

void LineSearch::change_weight(Trainable* conn)
{
	error("DEAD CODE ERROR");
}
