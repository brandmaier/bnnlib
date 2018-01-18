/*
 * CGTrainer.cpp
 *
 *  Created on: 08.04.2010
 *      Author: andreas
 */

#include "CGTrainer.h"
#include "../Trainable.h"
#include "LineSearch.h"

#undef DEBUG

CGTrainer::CGTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->alpha = 0.01;
	this->tol = 10e-5;
	this->initial_delta = 0.1;
	this->restart_cg_every = network->get_num_weights();

	_stat_num_bracket_runs = 0;
	_stat_num_gss_runs = 0;
}

CGTrainer::~CGTrainer()
{

}

void CGTrainer::change_weight(Trainable* conn)
{
	// NOOP


	/*double d =  conn->derivative;

	conn->weight += d;
	conn->previous_weight_change = d;
	conn->previous_derivative = conn->derivative;
	conn->derivative = 0.0;
*/
}

//TODO at the moment this is ignoring the validationset
void CGTrainer::train(SequenceSet* sequence_set,SequenceSet* validation_set, unsigned int iterations)
{
	if (!network->sanity_check(false)) {
		network->sanity_check(true);
		error("network did not pass sanity check!");
	}


	this->sequenceset = sequence_set;
	this->validation_set = validation_set;

	assert(validation_set != NULL);

	if ( sequenceset->target_size != network->out_size)
	{
		stringstream err;
		err << "The number of output nodes (" << network->out_size
				<< ") does not match the number of targets ("
				<< sequenceset->target_size << ") in the dataset";
		error(err.str());
	}

	assert( sequenceset->input_size >= network->in_size);
	assert( sequenceset->target_size == network->out_size);

	if (sequenceset->input_size != network->in_size)
	{
		warning("Sequence input size does not match network size!");
	}

	assert( abort_criteria.size() == abort_criterion_every.size());
	assert( callbacks.size() == callback_every.size());
	// set previous gradient to zero for all Connections
	/*for (unsigned int i=0; i < network->size; i++) {
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
			 network->nodes[i]->outgoing_connections[j]->previous_derivative = 0.0;
		}
	}*/
	training_start_time = time(0);


	for (unsigned int iter=0; iter < iterations; iter++)
	{
		total_iterations++;

#ifdef DEBUG
		cout << "*"<<iter<<"*"<<endl;
#endif

		// determine gradient
		network->reset(true);
		for (unsigned int i=0; i < sequenceset->size(); i++)
		{
			network->reset(true);
			this->_train_sequence( (*sequenceset)[i], false );
		}


		// Fletcher-Reeves
		beta = 0;
		double beta_denom = 0;
		double beta_num = 0;
		for (unsigned int i=0; i < network->size; i++) {
			for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
				Connection* conn = network->nodes[i]->outgoing_connections[j];
				beta_num += conn->derivative*conn->derivative;
				beta_denom += conn->previous_derivative*conn->previous_derivative;
				//cout << conn->derivative << "," << beta_num << "," << beta_denom<< " betanum"<<endl;
			}
		}
		beta = beta_num/beta_denom;
		// for first iteration, just do Gradient Descent
		if (isinf(beta)) {
#ifdef DEBUG
			cout << "beta is inf" << endl;
#endif
			beta = 0.0;
		}
		if (isnan(beta)) {
			warning("Beta is NaN! Unknown error!");
			beta = 0.0;
			return;
		}

		// restart conjugate Gradients every steps
		if (iter % restart_cg_every == 0) beta = 0.0;



		vector<weight_t>* direction = new vector<weight_t>();
		direction->reserve( network->get_num_weights());
		for (unsigned int i=0; i < network->size; i++) {
			for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
				Connection* conn = network->nodes[i]->outgoing_connections[j];
				//cout <<"Connection:" << conn->derivative << ","<<beta<<","<< conn->previous_derivative<< endl ;
				double d = conn->derivative + beta*conn->previous_derivative; // conn->derivative is the negative gradient
				direction->push_back(d);
			}
		}

		// remember weights
		original_weights = network->get_all_weights();
		double cur_error = this->network->evaluate_error(sequenceset);

		// Bracketing
		double ax = 0, bx=0, cx=0;

		unsigned int max_bracketing_steps = 100;
		double delta = initial_delta;

		double delta_error = evaluate(direction, delta);

		if (delta_error < cur_error) {
		 for (unsigned int i=0; i < max_bracketing_steps; i++)
		 {
			 _stat_num_bracket_runs++;
			 double last_delta = delta;
			 double last_error = delta_error;
			 delta*=2;
			 double temp_error = evaluate(direction, delta);
			 if (temp_error > last_error) {
				 // Bracketed!
				 ax = 0; bx = last_delta; cx=delta;
				 break;
			 }
		 }
		} else {
			_stat_num_bracket_runs++;
			 for (unsigned int i=0; i < max_bracketing_steps; i++)
			 {
				 double last_delta = delta;
				 double last_error = delta_error;
				 delta*=0.5;
				 double temp_error = evaluate(direction, delta);
				 if (temp_error < last_error) {
					 // Bracketed!
					 ax = 0; cx = last_delta; bx=delta;
					 break;
				 }
			 }
		}

		double best_error;

		if (bx==0) {
			warning("Bracketing failed");
			alpha = delta;
			cout << " continue GD with alpha=" << delta << endl;
		} else {

			// restore weights
			this->network->set_all_weights(original_weights);

			// output
#ifdef DEBUG
			cout << "Bracketing yielded " << ax << "," << bx << ","<<cx<<endl;
#endif
			// ----

			double x0,x1,x2,x3,f1,f2;

			// Golden Section Search
			double C = (3-sqrt(5))/2.0;
			double R = 1-C;
			x0 = ax;
			x3 = cx;
			if (abs(cx-bx) > abs(bx-ax))
			{
				x1=bx;
				x2 = bx + C*(cx-bx);
			} else {
				x2 = bx;
				x1 = bx - C*(bx-ax);
			}
			f1 = evaluate(direction, x1);
			f2 = evaluate(direction, x2);

			unsigned int run=1;
			while ((run < 100) and (abs(x3-x0) > tol*(abs(x1)+abs(x2))))
			{
				_stat_num_gss_runs++;
#ifdef DEBUG
				cout << "GSS "<<run<<" |a-b| " << abs(x3-x0)<< "  x0="<<x0<<" x3="<<x3<<endl;
#endif
				if (f2 < f1) {
					x0 = x1;
					x1 = x2;
					x2 = R*x1 + C*x3;
					f1 = f2;
					f2 = evaluate(direction, x2);
				} else {
					x3 = x2;
					x2 = x1;
					x1 = R*x2 + C*x0;
					f2 = f1;
					f1 = evaluate(direction, x1);
				}
				run++;
			}

			double xmin=0; double fmin=0;
			if (f1 < f2) {
				xmin = x1;
				fmin = f1;
			}else {
				xmin = x2;
				fmin = f2;
			}
#ifdef DEBUG
			cout << "GSS result: alpha=" << xmin << ", error="<< fmin << endl;
#endif
			alpha = xmin;
			best_error = fmin;

		}
		// Line Search


/*		bool cont = true;
		unsigned int counter = 0;

		double last_alpha = 0;

		double best_alpha = 0;
		double best_error = cur_error;
#ifdef DEBUG
		cout << "** Linesearch with cur_error="<<cur_error<<endl;
#endif
		while (cont) {

			vector<weight_t> w(original_weights->size());
			for (unsigned int i=0; i < original_weights->size(); i++)
			{
				w[i] = (*original_weights)[i] + alpha*(*direction)[i];
			}

			this->network->set_all_weights(&w);
			double new_error = this->network->evaluate_error(sequenceset);
#ifdef DEBUG
			cout << "- trying " << alpha << " with " << new_error <<""<<endl;
#endif
			if (isnan(new_error)) {
				error("Error has become NaN!");

			}

			if (new_error <= best_error) {
				best_alpha = alpha;
				best_error = new_error;
				//print_vector(&w);
			}


			//double temp_alpha = last_alpha;
			last_alpha = alpha;
			if (new_error >= cur_error) {
				alpha = alpha*0.5;
			} else {
				alpha *= 1.2;
			}

			if (alpha == 0) {alpha = 0.1; last_alpha=0;} // just restart and smile

			counter++;
			if (counter > 30) break;


		}
*/
	//	message("after restore");
		this->network->set_all_weights(original_weights);
		//print_vector( weights );

//		alpha = best_alpha;

		if (abs(best_error-cur_error) < 10e-15) {
			cout << "* simple gradient step"<<endl;
			// simple Gradient Descent
			unsigned int k=0;
			for (unsigned int i=0; i < network->size; i++) {
				for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
					Connection* conn = network->nodes[i]->outgoing_connections[j];
					//cout <<"Connection:" << conn->derivative << ","<<beta<<","<< conn->previous_derivative<< endl ;
					double d = conn->derivative; //+ beta*conn->previous_derivative; // conn->derivative is the negative gradient
					(*direction)[k] = d;
							k++;
				}
			}
		} else {
			cout << "* alpha="<<alpha << " beta=" << beta << " error=" << best_error << endl;
		}

		//double alpha = 0.1;
#ifdef DEBUG
		cout << "step with alpha=" << alpha << ", beta="<<beta<< "  best error=" << best_error << endl;
#endif
		// change weights
		unsigned int k=0;
		for (unsigned int i=0; i < network->size; i++) {
			for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
				Connection* conn = network->nodes[i]->outgoing_connections[j];
			//	double d = conn->derivative * alpha;
				double d = (*direction)[k] * alpha;
				k++;
				//cout << "change: " << d << "  derivative="<< conn->derivative << endl;
				conn->weight += d;
				conn->previous_weight_change = d;
				conn->previous_derivative = conn->derivative;
				conn->derivative = 0.0;
			}
		}
#ifdef DEBUG

		message("after adaption");
		print_vector(network->get_all_weights());
		double new_error = this->network->evaluate_error(sequenceset);
		cout << "AT END OF RUN: " << new_error << endl;
	/*	if (new_error != best_error) {
			warning("Error values diverge!");
		}
		assert( new_error == best_error );*/
#endif

		weight_t mse = current_error; //get_mean_squared_error();
		error_train.push_back( mse );

		if (sequenceset != validation_set) {
			error_validation.push_back( network->evaluate_validation_error( validation_set ) );
		}

		lowest_error = min(mse, lowest_error);


		 error_train.push_back(best_error);

		 for (unsigned int j=0; j < callbacks.size(); j++)
		 {
			if (iter % callback_every[j] == 0) callbacks[j]->callback(network, this, sequenceset);
		 }


		// loop through all abort criteria
		for (unsigned int k=0; k < abort_criteria.size(); k++)
		{
			if (criterion_min_epochs[k] > iter) continue;

			if ((iter%abort_criterion_every[k]==0) && (abort_criteria[k])->finished(network, this, sequenceset)) {
				return;
			}
		}

	}
}


double CGTrainer::evaluate(vector<weight_t>* direction, double alpha)
{
//	cout << "evaluate" << alpha << "," << weights->size() <<  endl;
	vector<weight_t> w(original_weights->size());
	for (unsigned int i=0; i < original_weights->size(); i++)
	{
		w[i] = (*original_weights)[i] + alpha*(*direction)[i];
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


void CGTrainer::change_weights()
{

	// NOOP
}

