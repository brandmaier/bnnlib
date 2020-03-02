/*
 * Trainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "Trainer.h"
#include "../GnuplotGenerator.h"
#include "CorrelationDerivatives.h"
#include "../functions.h"
#include <numeric>

#undef VERBOSE_BACKPROP
#undef SAFE_MODE
#undef DEBUG_DURING_TRAINING

#undef ERROR_TRUNCATION


Trainer::Trainer() {

	
		this->batch_learning = false;
		this->weight_decay = 0.0f;
		this->weight_decay_enabled = false;
		this->decorrelation_enabled = false;
		this->decorrelation_alpha = 0.1;
		this->learning_rate = 0.0001;
		this->learning_rate_discount = 1-(0.00001);
		this->momentum = 1.0;
		this->sparsity_beta = 0.1;

		//this->error_function = new SquaredErrorFunction();
		this->sequenceset = NULL;
		this->validation_set = NULL;

		initialize();		
	}

void Trainer::train(SequenceSet* sequenceset,  unsigned int iterations)
{
	return train(sequenceset, sequenceset, iterations);
}

void Trainer::train(DataSet* dataset,  unsigned int iterations)
{
	return train(dataset->training_set, dataset->validation_set, iterations);
}

string Trainer::get_name()
{
	return "Generic Trainer";
}

void Trainer::prepare_step()
{
  return;
}


void Trainer::train(SequenceSet* sequenceset,SequenceSet* validation_set,  unsigned int iterations)
{
		if (!network->sanity_check(false)) {
			network->sanity_check(true);
			error("network did not pass sanity check!");
		}


		this->sequenceset = sequenceset;
		this->validation_set = validation_set;

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

		assert( sequenceset->input_size == validation_set->input_size);
		assert( sequenceset->input_size == validation_set->input_size);

		assert( abort_criteria.size() == abort_criterion_every.size());
		assert( callbacks.size() == callback_every.size());


		unsigned int i_every = 0;

		//bool verbose_training = true;

		training_start_time = time(0);


		error_train.reserve(iterations);
		if (sequenceset != validation_set)
			error_validation.reserve(iterations);

		for (unsigned int i=0; i < iterations; i++, i_every++)
		{
		  this->prepare_step();

			//  [1] perform weight changes, adapt learning rate
			this->train_sequenceset(sequenceset);
			this->learning_rate *= learning_rate_discount;

			// [2] evaluate errors
			weight_t mse = current_error; //get_mean_squared_error();
			error_train.push_back( mse );
			if (sequenceset != validation_set) {
				error_validation.push_back( network->evaluate_validation_error( validation_set ) );
			}

			lowest_error = min(mse, lowest_error);


			 for (unsigned int j=0; j < callbacks.size(); j++)
			 {
				if (i % callback_every[j] == 0) callbacks[j]->callback(network, this, sequenceset);
			 }


			// loop through all abort criteria
			for (unsigned int k=0; k < abort_criteria.size(); k++)
			{
				if (criterion_min_epochs[k] > i) continue;

				if ((i%abort_criterion_every[k]==0) && (abort_criteria[k])->finished(network, this, sequenceset)) {
					return;
				}
			}

		}


		//delete error_test;

		return;
}



/**
 * Run training of a single epoch using a SequenceSet
 * 
 * First, the trainer is reset.
 * For batch learning, the
 *  network is reset once, then all Sequences are run trough
 *  the network, gradients computed and then
 *  a single weight change takes place
 * 
 */
void Trainer::train_sequenceset(SequenceSet* sequences)
{
	reset();

	if (batch_learning)
	{
		network->reset();
		for (unsigned int i=0; i < sequences->size(); i++)
		{
			network->reset(true);
			this->_train_sequence( (*sequences)[i], false );
		}
		this->change_weights();

	} else {

		sequences->shuffle();
		for (unsigned int i=0; i < sequences->size(); i++)
		{
			network->reset();
			this->_train_sequence( (*sequences)[i], true );
		}
	}

	total_iterations++;
}


void Trainer::_train_sequence(Sequence* sequence, bool change_weights)
{


	assert(sequence->size() > 0);
	assert(network->size > 0);



	// reserve buffer for maximum time
	network->reserve( sequence->size() );

//	unsigned int total_t = sequence->training_end - sequence->training_begin;

/*	//TODO reset deltas
	for (unsigned int i=0; i < network->nodes.size(); i++)
		{
			for (unsigned int t=0; t < total_t; t++)
			{
				//cout << (*nodes)[i]->delta[t]<<"\t";
				(network->nodes)[i]->delta[t] = 0.0;
			}
		}
		//cout << endl;
*/


	//cout << "network at " << network->timestep << " sending sequence length : " << sequence->size() << endl;

	//cout << "training sequence from "  << sequence->training_begin << " until " << sequence->training_end << " from a total of " << sequence->size() << endl;

	for (unsigned int i= sequence->training_begin; i < sequence->training_end; i++)
	{
		network->_activate( sequence->get_input(i) );
	}
	//cout << "network at t=" << network->timestep << endl;

#ifdef DEBUG_DURING_TRAINING
	network->debug();
#endif

	if ( decorrelation_enabled ) {
		assert( decorrelation_nodes.size() > 0);
		CorrelationDerivatives cd(network, &decorrelation_nodes);
		cd.calculate_derivatives( decorrelation_alpha );
	}

	assert(sequence->size() > 0);

	for (int i = (int)sequence->training_end-1; i >= (int)sequence->training_begin; i--)
	{
		//std::cout << "backwards " << i<< "/" << sequence->size() << " " << sequence->target.size() << " " << (*sequence->get_target(i))[0] << "\n";
		backward( sequence->get_target(i) );
	}


	if (network->use_initial_states) {
		//backward_zero();
		vector<weight_t> target( network->out_size);
		for (unsigned int i=0; i < network->out_size; i++)
		{
			target[i] = ( NAN );
		}
		backward( &target );
		assert(network->timestep==0);

		for (unsigned int i=0; i < network->size; i++) {
			network->nodes[i]->derivative -=
					network->nodes[i]->delta[0];
		}

	}



	if (change_weights)
		this->change_weights();

}

void Trainer::plot_errors()
{

	if (this->error_validation.size() == 0) {
		GnuplotGenerator::plot_vector("Training Error", &this->error_train);
	} else {

	GnuplotGenerator::plot_vectors("Training Errors", &this->error_train,
			"Training Set",
			&this->error_validation, "Validation Set");
	}

}

// this is not really a reset but just a
// preparation before an epoch is started
void Trainer::reset()
{
	previous_error = current_error; //TODO: check this
	current_error = 0;
}

void Trainer::initialize()
{
  
  this->total_iterations = 0;
  this->previous_error = 0;
  this->current_error = 0;
  
  this->lowest_error = INFINITY;
  
  error_train.clear();
}

weight_t Trainer::get_total_error() {
	return current_error;
}

weight_t Trainer::get_root_mean_sequence_error()
{
	return sqrt(current_error/sequenceset->size());
}

weight_t Trainer::get_normalized_root_mean_sequence_error()
{
	return get_root_mean_sequence_error() / (sequenceset->target_max-sequenceset->target_min);
}

weight_t Trainer::get_normalized_root_mean_element_error()
{
	return sqrt(current_error/sequenceset->element_count) / (sequenceset->target_max-sequenceset->target_min);

}

weight_t Trainer::get_squared_error_percentage()
{
	assert(!isnan(sequenceset->target_max));
	assert(!isnan(sequenceset->target_min));
	return 100*(sequenceset->target_max-sequenceset->target_min)/sequenceset->element_count* current_error;
}

weight_t Trainer::get_mean_squared_error()
{
	//#assert(sequenceset != NULL);
	if (sequenceset == NULL) {
		warning("Sequenceset is not specified!");
		return -1;
	}

	return current_error/sequenceset->size();
}

/**
 calculate partial derivatives for all weights
 **/
void Trainer::backward(std::vector<weight_t>* target) {

	unsigned int timestep = network->timestep;

	assert(timestep > 0);

	if ((*target).size() != network->out_size)
	{
		stringstream errormsg;
		errormsg << "Target size of dataset ("<<  (*target).size()
				<<") does not match number of output nodes of network ("
				<< network->out_size << ")";
		error(errormsg.str());
	}

	// update current error
	vector<weight_t>* output = network->get_output( timestep-1 );
	current_error += network->error_function->get_error( target, output );
	delete output;


/*	for (unsigned int i=0; i < network->out_size; i++)
	{
		if (!isnan((*target)[i])) {
		 weight_t tempmult = (network->nodes[network->out_offset+i]->actbuf[timestep-1]
		                       - (*target)[i] );
		 current_error += tempmult*tempmult;

		current_error += network->error_function->get_error( (*target)[i]  ,
				network->nodes[network->out_offset+i]->actbuf[timestep-1]);
		} else {
			// if target is not defined, do not alter error
		}
	}
*/

#ifdef SAFE_MODE
	assert(timestep > 0);
	assert( (*target).size() == network->out_size);
#endif

	// calculate deltas for all nodes but the inputs (in reversed order, starting with output units)

	// calculate delta-values for all output nodes
	int j=0;
	for (unsigned int i=network->out_offset; i < network->size; i++, j++)
	{
		Node* node = network->nodes[i];
#ifdef SAFE_MODE
		assert(node->actbuf.size() >= timestep-1);
#endif

		if (isnan((*target)[j])) {
			node->delta[timestep-1] += 0;	//+=
		} else {
			node->delta[timestep-1] += node->get_activation_function_derivative(timestep-1)
					* (node->actbuf[timestep-1] - (*target)[j]) ;	//+=
		}

#ifdef VERBOSE_BACKPROP
	cout << "Delta (at output) #"<< node->name <<": " << node->delta[timestep-1]
	   << " activation was: " << node->actbuf[timestep-1]
	  << " deriv=" << node->get_activation_function_derivative(timestep-1)
	   << " t="<< (*target)[j] <<endl;
#endif
	}

	// for all hidden units
	for (unsigned int i=network->size-network->out_size-1; i >= network->hid_offset; i--)
	{


		// fetch deltas from all outgoing connections
#ifdef VERBOSE_BACKPROP
		std::cout << i << ".\033[34mcalc delta sum for " << network->nodes[i]->name << "_t"<<(timestep-1) <<"\033[0m connections:"<< network->nodes[i]->outgoing_connections.size() <<"\n";
#endif
		weight_t delta_sum = 0.0;
		Node* node = network->nodes[i];
		for (unsigned int j=0; j < node->outgoing_connections.size(); j++)
		{
			Connection* conn = node->outgoing_connections[j];
			if (conn->forward) {
					delta_sum += conn->to->delta[network->timestep-1] * conn->to->get_input_to_activation_derivative(conn, timestep-1);
#ifdef VERBOSE_BACKPROP
					std::cout<< " |- added delta ["<< conn->to->name <<"_t"<< (timestep-1)<<"] " << conn->to->delta[timestep-1]<< " * " << conn->to->get_input_to_activation_derivative(conn, timestep-1) <<   "[delta * d_input/d_act ] node_type:"<< conn->to->get_type() << endl;
#endif

/*#ifdef SAFE_MODE
		if (isinf(delta_sum)) {
			cout << node->name << " at time="<<network->timestep<< ": "<< conn->to->delta[network->timestep-1] << "*" << conn->to->get_input_to_activation_derivative(conn, timestep) << endl;
		}
#endif
*/
			} else {
				if (network->timestep < conn->to->delta.size()) {

#ifdef ERROR_TRUNCATION
					if ((conn->to->get_type() != Node::SIGMOID_NODE) && (conn->to->name!="input"))
#endif
					delta_sum += conn->to->delta[timestep] * conn->to->get_input_to_activation_derivative(conn, timestep);



#ifdef VERBOSE_BACKPROP
					std::cout<< " |- added future delta ["<< conn->to->name <<"_t"<< (timestep)<<"] " << conn->to->delta[timestep]<< " * " << conn->to->get_input_to_activation_derivative(conn, timestep) << "[delta * d_input/d_act]" << endl;
#endif

				} else {
#ifdef VERBOSE_BACKPROP
					std::cout<< " |- skipping future delta " <<  endl;
#endif
				}
			}


		}
		
		// from
		// http://ufldl.stanford.edu/tutorial/unsupervised/Autoencoders/
		// but let each node has its own sparsity prior
    if (node->has_sparsity_prior()) {
      if (node->actbuf.size() > 0) {
       // get mean activation
       weight_t rho_hat = accumulate(node->actbuf.begin(),
                                           node->actbuf.end(),
                                           0.0) / node->actbuf.size();
       weight_t rho = node->sparsity_prior;
       if (rho_hat!=0 && rho_hat!=1)
         delta_sum += this->sparsity_beta*(-rho/rho_hat+(1-rho)/(1-rho_hat));
      }
    }

		node->delta[timestep-1] +=	//+=
				node->get_activation_function_derivative(timestep-1)
				* delta_sum;


#ifdef SAFE_MODE
		if (isnan(node->delta[timestep-1])) {
			cout << "Delta has become NaN!"<< node->get_label();
					cout << " actbuf:" << node->actbuf[timestep-1];
					cout << " at time : " << (timestep-1) << endl;
			//print_vector(network->get_all_initialstates());
			cout << "Delta sum: " << delta_sum << " derivative: "<< node->derivative_activation_function(network->nodes[i]->actbuf[timestep-1]) << endl;
			network->save("error.bnn");
			exit(0);
		}
		if (isinf(node->delta[timestep-1])) {
			cout << "Delta has become INF!"<<  network->nodes[i]->get_label()
<< " actbuf:" << network->nodes[i]->actbuf[timestep-1] <<endl;
			cout << "Delta sum: " << delta_sum << " derivative: "<< network->nodes[i]->derivative_activation_function(network->nodes[i]->actbuf[timestep-1]) << endl;
			network->save("error.bnn");
			exit(0);
		}
#endif

#ifdef VERBOSE_BACKPROP

		cout << " | ==" << node->delta[timestep-1];
		cout << " (delta_sum=" << delta_sum << ",deriv="
		 <<node->get_activation_function_derivative(timestep-1)
			<<	" of x=" << node->actbuf[timestep-1] <<")"<<endl;
#endif

	}

	// calculate derivatives for all weights
#ifdef VERBOSE_BACKPROP
	message("Derivatives");
#endif

if (weight_decay_enabled) {
	for (unsigned int i=0; i < network->size; i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
			Connection* conn = network->nodes[i]->outgoing_connections[j];

			if (conn->forward) {
 				conn->derivative -= ( conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep-1]  +  weight_decay*conn->weight );
			} else {
				if (timestep < conn->to->delta.size()) {
 				 conn->derivative -= ( conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep] +  weight_decay*conn->weight );
				}
			}

#ifdef VERBOSE_BACKPROP
			std::cout << "delta_w" << network->nodes[i]->name << "->"
			 << network->nodes[i]->outgoing_connections[j]->to->name << ":"
			 << network->nodes[i]->outgoing_connections[j]->derivative <<
			 "  inp_to_weight deriv=" << network->nodes[i]->get_input_to_weight_derivative(conn,timestep-1)<<
			 " delta=" << network->nodes[i]->outgoing_connections[j]->to->delta[timestep-1] << endl;
#endif

		}
	}

} else {
	for (unsigned int i=0; i < network->size; i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
			Connection* conn = network->nodes[i]->outgoing_connections[j];

			if (conn->forward) {
 				conn->derivative -= (conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep-1]  );
 				                                                                                                                                                                                                    //+  weight_decay*nodes[i]->outgoing_connections[j]->weight );
			} else {
				if (timestep < conn->to->delta.size()) {
 				 conn->derivative -= ( conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep] );
 				                                                                                                                                                                                                          //+  weight_decay*nodes[i]->outgoing_connections[j]->weight );
				}
			}

#ifdef VERBOSE_BACKPROP
			std::cout << "derivative for " << network->nodes[i]->get_label() << "->"
			 << network->nodes[i]->outgoing_connections[j]->to->get_label() << ": deriv="
			 << network->nodes[i]->outgoing_connections[j]->derivative << "  inp_to_weight deriv=" <<
			 network->nodes[i]->get_input_to_weight_derivative(conn,timestep-1)<< " conn->to->delta="
			 << network->nodes[i]->outgoing_connections[j]->to->delta[timestep-1] << endl;
#endif

		}
	}



}

	network->timestep--;
}

/*
void Trainer::backward_zero()
{
	assert( network->use_initial_states);
	assert( network->timestep == 1 );

	unsigned int timestep = network->timestep;

	// for all hidden units
	for (unsigned int i=network->size-network->out_size-1; i >= network->in_offset; i--)
	{


		// fetch deltas from all outgoing connections
#ifdef VERBOSE_BACKPROP
		std::cout << i << ".\033[34mcalc delta sum for " << network->nodes[i]->name << "_t"<<(timestep-1) <<"\033[0m connections:"<< network->nodes[i]->outgoing_connections.size() <<"\n";
#endif
		weight_t delta_sum = 0.0;
		Node* node = network->nodes[i];
		for (unsigned int j=0; j < node->outgoing_connections.size(); j++)
		{
			Connection* conn = node->outgoing_connections[j];
			if (conn->forward) {
					delta_sum += conn->to->delta[network->timestep-1] * conn->to->get_input_to_activation_derivative(conn, timestep-1);
#ifdef VERBOSE_BACKPROP
					std::cout<< " |- added delta ["<< conn->to->name <<"_t"<< (timestep-1)<<"] " << conn->to->delta[timestep-1]<< " * " << conn->to->get_input_to_activation_derivative(conn, timestep-1) <<   "[delta * d_input/d_act ] node_type:"<< conn->to->get_type() << endl;
#endif

			} else {
				if (network->timestep < conn->to->delta.size()) {
					delta_sum += conn->to->delta[timestep] * conn->to->get_input_to_activation_derivative(conn, timestep);



#ifdef VERBOSE_BACKPROP
					std::cout<< " |- added future delta ["<< conn->to->name <<"_t"<< (timestep)<<"] " << conn->to->delta[timestep]<< " * " << conn->to->get_input_to_activation_derivative(conn, timestep) << "[delta * d_input/d_act]" << endl;
#endif

				} else {
#ifdef VERBOSE_BACKPROP
					std::cout<< " |- skipping future delta " <<  endl;
#endif
				}
			}


		}

		node->delta[timestep-1] =
				//nodes[i]->derivative_activation_function(nodes[i]->actbuf[timestep-1])
				node->get_activation_function_derivative(timestep-1)
				* delta_sum;		//BUG get_last_input_derivative() mglw. size zu groß! prealloziert...

#ifdef SAFE_MODE
		if (isnan(network->nodes[i]->delta[timestep-1])) {
			cout << "@node " << network->nodes[i]->get_label() << " time=" << (timestep-1) << endl;
			cout << "backward_zero(): Delta has become NaN!"<< endl;
			cout << "Delta sum: " << delta_sum << " derivative: "<< network->nodes[i]->derivative_activation_function(network->nodes[i]->actbuf[timestep-1]) << " inbuf:" <<network->nodes[i]->inbuf[timestep-1] <<endl;
			exit(0);
		}
		if (isinf(network->nodes[i]->delta[timestep-1])) {
			cout << "@node " << network->nodes[i]->get_label() << endl;
			cout << "backward_zero(): Delta has become INF!"<< endl;
			cout << "Delta sum: " << delta_sum << " derivative: "<< network->nodes[i]->derivative_activation_function(network->nodes[i]->actbuf[timestep-1]) << endl;
			exit(0);
		}
#endif

#ifdef VERBOSE_BACKPROP
		std::cout << " | ==" << network->nodes[i]->delta[timestep-1] <<
				" (delta_sum=" << delta_sum << ",deriv="<<network->nodes[i]->derivative_activation_function(network->nodes[i]->actbuf[timestep-1]) <<
				" of x=" << network->nodes[i]->actbuf[timestep-1] <<")" << endl;
#endif

	}





	for (unsigned int i=0; i < network->size; i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++) {
			Connection* conn = network->nodes[i]->outgoing_connections[j];

			if (conn->forward) {
 				conn->derivative -= (conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep-1]  );
 				                                                                                                                                                                                                    //+  weight_decay*nodes[i]->outgoing_connections[j]->weight );
			} else {
				if (timestep < conn->to->delta.size()) {
 				 conn->derivative -= ( conn->to->get_input_to_weight_derivative(conn,timestep-1) * conn->to->delta[timestep] );
 				                                                                                                                                                                                                          //+  weight_decay*nodes[i]->outgoing_connections[j]->weight );
				}
			}

#ifdef VERBOSE_BACKPROP
			std::cout << "delta_w" << network->nodes[i]->get_label() << "->"
			 << network->nodes[i]->outgoing_connections[j]->to->get_label() << ": deriv("<< i <<")="
			 << network->nodes[i]->outgoing_connections[j]->derivative << "  inp_to_weight deriv=" <<
			 network->nodes[i]->get_input_to_weight_derivative(conn,timestep-1)<< " conn->to->delta="
			 << network->nodes[i]->outgoing_connections[j]->to->delta[timestep-1] << endl;
#endif

		}

	}

	network->timestep--;

}
*/

void Trainer::change_weights()
{
	for (unsigned int i=0; i < network->size; i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* c = network->nodes[i]->outgoing_connections[j];

			change_weight(c);

		}

	}


	if (network->use_initial_states) {
		// added new component for learning initial state
		for (unsigned int j=0; j < sequenceset->size(); j++)
		{
			for (unsigned int i=0; i < network->size;i++ )
			{
				change_weight(network->nodes[i] );
			}
		}
	}
}


void Trainer::add_callback(CallbackHandler* cb, unsigned int steps)
{
	assert(steps > 0);
	if (steps == 0) {
		warning("Could not add abort callback! 0 steps is impossible!");
		return;
		}
	callbacks.push_back( cb );
	callback_every.push_back( steps );
}

void Trainer::add_abort_criterion(Criterion* cr, unsigned int steps, unsigned int min_steps)
{
	assert(steps > 0);
	if (steps == 0) {
		warning("Could not add abort criterion! 0 steps is impossible!");
		return;
	}
	abort_criteria.push_back( cr );
	abort_criterion_every.push_back( steps );
	criterion_min_epochs.push_back( min_steps );
}

void Trainer::remove_abort_criterion(Criterion* cr)
{
	unsigned int p = find(abort_criteria.begin(), abort_criteria.end(), cr)-abort_criteria.begin();
	abort_criteria.erase( abort_criteria.begin()+p );
	abort_criterion_every.erase( abort_criterion_every.begin()+p );
}

void Trainer::remove_callback(CallbackHandler* cr)
{
	unsigned int p = find(callbacks.begin(), callbacks.end(), cr) - callbacks.begin();
	callbacks.erase( callbacks.begin()+p );
	callback_every.erase( callback_every.begin()+p );
}

void Trainer::add_abort_criterion(Criterion* cr, unsigned int steps)
{
	add_abort_criterion(cr, steps, 0);
}

