/*
 * DataSet.cpp
 *
 *  Created on: Nov 18, 2009
 *      Author: brandmaier
 */

#include "DataSet.h"

	DataSet::DataSet()
	{

	}

	DataSet::DataSet(SequenceSet* sequence_set)
	{
		this->training_set = new SequenceSet(*sequence_set);
		this->test_set = new SequenceSet(*sequence_set);
		this->validation_set = new SequenceSet(*sequence_set);
	}

	DataSet::DataSet(SequenceSet* training_set,SequenceSet* validation_set,SequenceSet* test_set)
	{
		this->training_set = training_set;
		this->validation_set = validation_set;
		this->test_set = test_set;
	}


	/*DataSet::DataSet(SequenceSet* sequence_set, double training, double validation)
	{
		this->training_set = new SequenceSet(*sequence_set);
		this->test_set = new SequenceSet(*sequence_set);
		this->validation_set = new SequenceSet(*sequence_set);

		unsigned inta = sequence_set
		this->training_set->set_training_begin(0);
		this->training_set->set_training_end(a);


	}
*/

	unsigned int DataSet::get_input_size() {
		return training_set->input_size;
	}

	unsigned int DataSet::get_target_size() {
		return training_set->target_size;
	}

	void DataSet::save(string filename)
	{
		stringstream filename_training;
		filename_training << filename << "-training.dat";
		stringstream filename_validation;
		filename_validation << filename << "-validation.dat";
		stringstream filename_test;
		filename_test << filename << "-test.dat";

		training_set->save_to_file( filename_training.str() );
		validation_set->save_to_file( filename_validation.str() );
		test_set->save_to_file( filename_test.str() );

	}

	void DataSet::load(string filename)
	{
		stringstream filename_training;
		filename_training << filename << "-training.dat";
		stringstream filename_validation;
		filename_validation << filename << "-validation.dat";
		stringstream filename_test;
		filename_test << filename << "-test.dat";

		training_set = new SequenceSet( filename_training.str() );
		validation_set = new SequenceSet( filename_validation.str() );
		test_set = new SequenceSet( filename_test.str() );

	}

	SequenceSet* DataSet::get_training_set() {
		return training_set;
	}

	SequenceSet* DataSet::get_validation_set() {
		return validation_set;
	}

	SequenceSet* DataSet::get_test_set() {
		return test_set;
	}
