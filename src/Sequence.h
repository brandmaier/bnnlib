#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <vector>
#include <cassert>
#include "functions.h"
#include <iostream>

struct Sequence
{
	string name;

	std::vector<std::vector<weight_t>*> target;
	std::vector<std::vector<weight_t>*> input;	
	
	std::vector<weight_t> weight;

	unsigned int training_begin;
	unsigned int training_end;
	unsigned int validation_begin;
	unsigned int validation_end;
	unsigned int test_begin;
	unsigned int test_end;

	void set_training_range(unsigned int training_begin, unsigned int training_end)
	{
		this->training_begin = training_begin;
		this->training_end = training_end;
	}

	void set_validation_range(unsigned int validation_begin, unsigned int validation_end)
	{
		this->validation_begin = validation_begin;
		this->validation_end = validation_end;
	}

	void set_test_range(unsigned int test_begin, unsigned int test_end)
	{
		this->test_begin = test_begin;
		this->test_end = test_end;
	}

	Sequence() {

	};

	Sequence(weight_t inputs[], weight_t targets[], unsigned int num_samples);

	~Sequence();
	
	unsigned int size() { assert(target.size()==input.size()); return target.size();}

	unsigned int get_input_size() { assert(input.size() > 0); return input[0]->size(); }
	unsigned int get_target_size() { assert(target.size() > 0); return target[0]->size(); }

	std::vector<std::vector<weight_t>*>* get_targets_between(unsigned int start, unsigned int end);


	std::vector<weight_t>* get_input(unsigned int index);
	std::vector<weight_t>* get_target(unsigned int index);
	double get_weight(unsigned int index);

	unsigned int get_maximum_index(unsigned int time);

	bool load_from_file(string filename);
	
	void add(std::vector<weight_t>* input, std::vector<weight_t>* target);

	void split_in_training_test_set(double fraction);
	void split_in_training_validation_test_set(double fraction_train, double fraction_val);

	Sequence* embed(unsigned int n);

	void append(Sequence* sequence);

	/**
	 * loads a sequence from a text-file and converts it
	 * into a Sequence for prediction.
	 *
	 */
	void load_raw_sequence_from_file(string filename);
};

#endif /*SEQUENCE_H_*/
