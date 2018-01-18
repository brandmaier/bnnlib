/*
 * CrossValidationError.cpp
 *
 *  Created on: 15.04.2010
 *      Author: andreas
 */

#include "CrossValidationError.h"
#include "../criteria/EarlyStopping.h"


CrossValidationError::CrossValidationError(Network* network, SequenceSet* sequence_set) {
	this->network = network;
	this->sequence_set = sequence_set;
	 train_with_validation_set = true;
}


vector<double>* CrossValidationError::evaluate(Trainer* trainer,
		ErrorFunction* err_func, unsigned int runs, unsigned int fold)
{

	vector<double>* error = new vector<double>(fold);

	assert(sequence_set->size() > fold);

	int f_size = sequence_set->size() / fold;
	int f_remainder = sequence_set->size() % fold;
	int f_offset = 0;


	for (unsigned int f=0; f < fold; f++)
	{

		unsigned int from = (int)f_offset;
		unsigned int cur_size = f_size;
		if (f_remainder > 0) {
			f_remainder--;
			cur_size++;
		}
		unsigned int to = f_offset+cur_size;
		SequenceSet* training = sequence_set->create_subset_without(from, to);

		DataSet* dataset_training = NULL;
		if (train_with_validation_set) {
			dataset_training = training->toBalancedDataSet(0.8,0.2);
		}

		SequenceSet* validation = sequence_set->create_subset(from, to);
		if (verbose) {
			cout << "Training without " << from << " until " << to << " size: "<< training->size() <<"/"<< validation->size() << endl;
		}

		network->randomize();

		if (train_with_validation_set) {
			//GeneralizatonLoss* gl = new GeneralizationLoss(dataset_training->validation_set, 5);
			EarlyStopping* gl = new EarlyStopping();
			trainer->add_abort_criterion(gl,1);
			trainer->train(dataset_training, runs);
			trainer->remove_abort_criterion(gl);
			delete gl;
		} else {
			trainer->train(training, runs);
		}

		(*error)[f] = ( network->evaluate_error(validation, err_func) );

		f_offset += cur_size;

		if (verbose)
			cout << "run " << f << ": " << (*error)[f] << endl;
	}

	// mean error
	double mean_error = mean(error);
	double std_error = sqrt(var(error));

	print_vector(error);
	cout << "Mean Error: " << mean_error << " +- " << std_error << endl;

	return(error);
}
