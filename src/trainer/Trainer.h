/*
 * Trainer.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef TRAINER_H_
#define TRAINER_H_

#include "../Sequence.h"
#include "../SequenceSet.h"
#include "../callbacks/CallbackHandler.h"
#include "../Network.h"
#include "../criteria/Criterion.h"
#include "../errorfunctions/ErrorFunction.h"
#include "../Trainable.h"

struct Criterion;
struct ErrorFunction;
struct Network;
struct CallbackHandler;

struct Trainer
{
	Trainer();
	virtual ~Trainer() {

		abort_criteria.clear();
		callbacks.clear();
	}

	Network* network;
	SequenceSet* sequenceset;
	SequenceSet* validation_set;
	weight_t learning_rate, learning_rate_discount;
	bool batch_learning;
	weight_t current_error;
	weight_t previous_error;

	weight_t weight_decay;
	bool weight_decay_enabled;

	bool decorrelation_enabled;
	vector<Node*> decorrelation_nodes;
	double decorrelation_alpha;

	unsigned int total_iterations;

	weight_t momentum;


	vector<Criterion*> abort_criteria;
	vector<CallbackHandler*> callbacks;
	vector<unsigned int> callback_every;
	vector<unsigned int> abort_criterion_every;
	vector<unsigned int> criterion_min_epochs;

	std::vector<weight_t> error_train;
	std::vector<weight_t> error_validation;

	ErrorFunction* error_function;

	weight_t lowest_error;

	time_t training_start_time;

	string name;
	/**
	 * run calculation of derivatives for a single sequence.
	 *
	 * /!\ It is not recommended to call this method directly. If it is necessary,
	 * make sure to call appropriate methods earlier (e.g. trainer->reset() )
	 */
	virtual void  _train_sequence(Sequence* sequences, bool change_weights);
	virtual void  train_sequenceset(SequenceSet* sequences);

	weight_t get_root_mean_sequence_error();
	weight_t get_total_error();
	weight_t get_normalized_root_mean_sequence_error();
	weight_t get_normalized_root_mean_element_error();

	void plot_errors();

	/**
	 * cmp. Proben1 definitions by Prechelt
	 */
	weight_t get_squared_error_percentage();
	/**
	 * cmp. Proben1 definitions by Prechelt
	 */
	weight_t get_mean_squared_error();

	virtual void change_weights();
	virtual void change_weight(Trainable* c)=0;

	void train(SequenceSet* sequences, unsigned int iterations);
	virtual void train(SequenceSet* sequenceset,SequenceSet* testset,  unsigned int iterations);
	void train(DataSet* dataset,  unsigned int iterations);

//	unsigned int train_until_convergence(SequenceSet* training_set, SequenceSet* validation_set);
//	unsigned int train_until_convergence(SequenceSet* training_set, SequenceSet* validation_set, unsigned int minimum_steps);

//	void train_sequence_after_each_step(Sequence* sequence);
//	void train_sequenceset_after_each_step(SequenceSet* sequenceset);

	/*weight_t check_gradient(Sequence* sequence);
	weight_t check_gradient(Sequence* sequence, weight_t epsilon);
	weight_t check_gradient(Sequence* sequence, weight_t epsilon, bool verbose);
*/

	virtual void backward(std::vector<weight_t>* target);
	//virtual void backward_zero();

	virtual void reset();

	void add_callback(CallbackHandler* cb, unsigned int steps);
	void add_abort_criterion(Criterion* cr, unsigned int steps);
	void add_abort_criterion(Criterion* cr, unsigned int steps, unsigned int min_steps);

	void remove_abort_criterion(Criterion* cr);
	void remove_callback(CallbackHandler* cr);

	virtual string get_name();

	weight_t get_current_training_error() {
		return this->error_train[this->error_train.size()-1];
	}

	weight_t get_current_validation_error() {
		return this->error_validation[this->error_validation.size()-1];
	}

};

#endif /* TRAINER_H_ */
