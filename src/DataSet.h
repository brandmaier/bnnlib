/*
 * DataSet.h
 *
 *  Created on: Nov 18, 2009
 *      Author: brandmaier
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "SequenceSet.h"
#include "Sequence.h"

struct SequenceSet;

struct DataSet
{
 public:
	DataSet();
	DataSet(SequenceSet* sequence_set);
	DataSet(SequenceSet* training_set,SequenceSet* validation_set,SequenceSet* test_set);
	//DataSet(SequenceSet* sequence_set, double training, double validation);

	SequenceSet* training_set;
	SequenceSet* validation_set;
	SequenceSet* test_set;

	unsigned int get_input_size();
	unsigned int get_target_size();

	SequenceSet* get_training_set();
	SequenceSet* get_validation_set();
	SequenceSet* get_test_set();

	void save(string filename);
	void load(string filename);

};

#endif /* DATASET_H_ */
