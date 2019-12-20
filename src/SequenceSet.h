#ifndef SEQUENCESET_H_
#define SEQUENCESET_H_

#include <vector>
#include <string>
//#include <strstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include "Sequence.h"
#include <algorithm>
#include <cmath>
#include "DataSet.h"
#include <sys/stat.h>
#include <map>

struct DataSet;

using namespace std;

struct SequenceSet 
{
	static std::string UNDEF_CHARACTER;
	
	static const int FILE_NOT_FOUND_EXCEPTION = 1;
	
	std::vector<Sequence*> set; // central storage of all sequences
	
	string name;

	bool input_is_normalized;
	bool target_is_normalized;
	std::vector<weight_t>* target_means;
	std::vector<weight_t>* target_stds;
	std::vector<weight_t>* input_means;
	std::vector<weight_t>* input_stds;
	
	weight_t target_min, target_max, input_min, input_max;
	weight_t original_target_min, original_target_max, original_input_min, original_input_max;

	unsigned int target_size;
	unsigned int input_size;
	unsigned int element_count;
	
	SequenceSet();
	SequenceSet(Sequence* sequence);
	SequenceSet(const std::string &filename);
	SequenceSet(const char* filename);
	SequenceSet(SequenceSet &sequence_set);
	SequenceSet(unsigned int num);
	
	~SequenceSet();
	
	unsigned int get_input_size()
	{
		return input_size;
	}

	unsigned int get_target_size()
	{
		return target_size;
	}

	void init();

	void add_sequence(Sequence* sequence);
	void add_copy_of_sequence(Sequence* sequence);
	void add_sequence_set(SequenceSet* sequence_set);
	
	void load_from_file(std::string filename);
	static DataSet* load_from_proben_file(string filename);
	static DataSet* load_from_compml_file(string filename);
	
	static SequenceSet* _load_compml_sequence(string filename, unsigned int num_labels, unsigned num_features, map<string, unsigned int> label_map);

	void save_to_file(string filename);
	
	void find_minmax();
	
	unsigned int size();

	
	void clear()
	{
		this->set.clear();	
	}
	
	Sequence* operator[](unsigned int index)
	{
		assert(index < this->size());
		return this->set[index];	
	}
	
	Sequence* get(unsigned int index)
	{
	  if (index >= 0 && index < this->size())
		  return this->set[index];
	  else {
	    return NULL;
	  }
	}

	void apply_normalization(vector<weight_t>* input_means, vector<weight_t>* input_stds, vector<weight_t>* target_means, vector<weight_t>* target_stds);
	void apply_denormalization(vector<weight_t>* input_means, vector<weight_t>* input_stds, vector<weight_t>* target_means, vector<weight_t>* target_stds);

	void normalize_scale_and_clip(unsigned int standard_deviations);
	
	void normalize(SequenceSet* other_set);
	void normalize();
	void normalize(bool input, bool target);

	void denormalize();
	void denormalize(SequenceSet* other_set);
	
	void denormalize_prediction(vector<vector<weight_t>*>* prediction);

	void scale(double factor);
	
	void scale_to_original();

	void scale_to_minmax(double min, double max);
	void scale_input_to_minmax(double new_min, double new_max);

	SequenceSet* create_subset(unsigned int index1, unsigned int index2);
	SequenceSet* create_subset_without(unsigned int index1, unsigned int index2);


	void shuffle();
	
	/**
	 *  not tested don't use this ;)
	 * 
	 * */
	void add_timeseries_from_file(string filename)
	{
		ifstream ifs(filename.c_str());
		string line;
	
		if (!ifs)
		{
			throw FILE_NOT_FOUND_EXCEPTION;
		}
	
		Sequence* cur_seq = new Sequence();

		// read input
		vector<weight_t>* input = new vector<weight_t>;
 		istringstream iss( line );
		copy( istream_iterator<weight_t>( iss ), istream_iterator<weight_t>(), back_inserter( *input ) );				

		unsigned int element_count = 0;
	
		while (!ifs.eof()) {
			getline(ifs, line);

			//print_vector(input);
			if ((line == "") || (line.substr(0,1)=="!")) continue;

			// read input
			vector<weight_t>* target = new vector<weight_t>;
 			istringstream iss( line );
			copy( istream_iterator<weight_t>( iss ), istream_iterator<weight_t>(), back_inserter( *target ) );				

			cur_seq->add(input, target);
			
			print_vector(input);
			print_vector(target);
			cout << "*" << endl;
			
			input = target;	
			
			element_count++;
			
		}		
		
		if (cur_seq->size() != 0)
			set.push_back( cur_seq );
		
		//cout << "Loaded sequences #" << seq_count << " and 
		cout << "added elements #" << element_count << endl;
	
		ifs.close();		
		
	}
	
	void print_sequences();

	static inline string get_property_value(string line)
	{
	      	vector<string> tokens;
	      	string sep = std::string("=");
	        split(line,sep,tokens);
	        return tokens[1];
	}

	SequenceSet* embed(unsigned int n);
//	SequenceSet* embed(unsigned int k, unsigned int n);

	DataSet* toDataSet(double precent_training, double percent_validation);
	DataSet* toDataSet(double precent_training, double percent_validation, bool shuffle);
	DataSet* toBalancedDataSet(double precent_training, double percent_validation);

	void set_all_as_training()
	{
		for (unsigned int i=0;i<size();i++)
		{
			get(i)->training_begin = 0;
			get(i)->training_end = get(i)->size();
		}
	}

	/**
	 * mark the complete length of all contained sequences
	 * as validation sequences
	 */
	void set_all_as_validation()
	{
		for (unsigned int i=0;i<size();i++)
		{
			cout << "set as validation " << get(i)->size() << endl;
			get(i)->validation_begin = 0;
			get(i)->validation_end = get(i)->size();
		}
	}

	/***
	 * set the index where the training region of the sequence starts
	 */
	void set_training_begin(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->training_begin = value;
	}

	/***
	 * set the index where the training region of the sequence ends
	 */
	void set_training_end(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->training_end = value;
	}

	/***
	 * set the index where the validation region of the sequence ends
	 */
	void set_validation_begin(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->validation_begin = value;
	}

	void set_validation_end(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->validation_end = value;
	}

	void set_test_begin(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->test_begin = value;
	}

	void set_test_end(unsigned int value)
	{
		for (unsigned int i=0;i<size();i++)
			get(i)->test_end = value;
	}
};

#endif /*SEQUENCESET_H_*/
