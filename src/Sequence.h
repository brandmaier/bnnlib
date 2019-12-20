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
	
	// copy constructor
	Sequence(Sequence &s2) {
	  name = s2.name;
	  training_begin = s2.training_begin;
	  training_end = s2.training_end;
	  validation_begin = s2.validation_begin;
	  validation_end = s2.validation_end;
	  test_begin = s2.test_begin;
	  test_end = s2.test_end;
	  
	  // copy vector using copy constructor
	  weight = std::vector<weight_t>(s2.weight);
	  // TODO: deep-copy of inputs and targets
	  for (unsigned int i=0; i < s2.target.size(); i++) {
	    std::vector<weight_t>* t = s2.target[i];
	    std::vector<weight_t>* tnew = new std::vector<weight_t>();
	    for (unsigned int j=0; j < t->size(); j++) {
	      tnew->push_back( (*t)[j] );
	    }
	    
	    std::vector<weight_t>* refi = s2.input[i];
	    std::vector<weight_t>* inew = new std::vector<weight_t>();
	    for (unsigned int j=0; j < refi->size(); j++) {
	      inew->push_back( (*refi)[j] );
	    }	    
	    
	    this->add(inew, tnew);
	    
	  }
	  
	}
	
	void add_from_array(double input[], double target[], unsigned int input_size, unsigned int target_size);

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

	Sequence();

	Sequence(weight_t inputs[], weight_t targets[], unsigned int num_samples);

	~Sequence();
	
	unsigned int size();

	unsigned int get_input_size();
	
	unsigned int get_target_size();

	std::vector<std::vector<weight_t>*>* get_targets_between(unsigned int start, unsigned int end);

	weight_t get_target_at(unsigned int index, unsigned int pos);
	weight_t get_input_at(unsigned int index, unsigned int pos);
	
	bool sanity_check();

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
