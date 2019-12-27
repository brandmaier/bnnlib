/*
 *  Network.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 18.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <vector>
#include "nodes/Node.h"
#include "Sequence.h"
#include "ensembles/Ensemble.h"
#include "SequenceSet.h"
#include "functions.h"
#include <string>
#include <map>
#include <set>
#include "callbacks/CallbackHandler.h"
#include "errorfunctions/ErrorFunction.h"
#include "errorfunctions/SquaredErrorFunction.h"
#include "errorfunctions/CrossEntropyErrorFunction.h"
#include "misc/Jacobian.h"

struct CallbackHandler;
struct Ensemble;

/**
 * \brief base class representing a neural network
 *
 * This class holds references to all nodes and indirectly to all connections between them.
 */

using namespace std;

struct Network {

	static const bool FORWARD = true;
	static const bool BACKWARD = false;
//	static const int BACKPROP = 0;
//	static const int RPROP = 1;
	
//	map<int, Node*> node_map;
	
//	int training_method;

	//unsigned int seed;
	Ensemble* input_ensemble;
	Ensemble* output_ensemble;

	std::vector<Node*> nodes;
	std::vector<Ensemble*> ensembles;
	//unsigned int total_connections;
	unsigned int in_size, hid_size, out_size, bias_size, size;
	unsigned int in_offset, hid_offset, out_offset, bias_offset;
	
	unsigned int washout_time;

	unsigned int timestep;

	bool use_initial_states;

	string description;
//	bool use_fahlmann_correction;
	
	//unsigned int buffer_pointer;

	//weight_t current_error;
	//weight_t previous_error;
	//SequenceSet* sequenceset;

	ErrorFunction* error_function;

	bool verbose;
	
	Network();
	Network(string filename);


	bool sanity_allows_deadends;

	/**
	 * removes all Ensembles and then all remaining nodes (that are not associated with Ensembles) from the Network.
	 */
	virtual ~Network();
	
	void init();

//	void set_seed(unsigned int seed);

	static Connection* connect(Node* node1, Node* node2, bool forward);
	static Connection* connect(Node* node1, Node* node2, bool forward, weight_t value);
	
//	weight_t get_total_error();
//	weight_t get_mean_sequence_error();

	void _activate(std::vector<weight_t>* input);
	void activate(Sequence* sequence);
//	void backward(std::vector<weight_t>* target);
//	void change_weights();
	void reset();
	void reset(bool keep_derivatives);
	void reset_gamma(double value);
	void debug();
	
	unsigned int get_num_nodes() { return(this->nodes.size()); }

	/*
	 * returns a vector of activations of all output nodes at the given time.
	 */
	std::vector<weight_t>* get_output(unsigned int time);
	std::vector<std::vector<weight_t>*>* get_outputs_until(unsigned int time);
	std::vector<std::vector<weight_t>*>* get_outputs_between(unsigned int start, unsigned int end);
	std::vector<std::vector<weight_t>*>* get_last_outputs(unsigned int time);

	void add_output_to_input_feedback(bool trainable);

	void export_to_dot_graph(string filename);
	string to_dot_graph();
	
	void reinitialise();

	void reserve( unsigned int buffer_size );

//	void set_initial_states(bool state);
	void print_offsets();

	void hessian();
	Jacobian* jacobian_numerical(Sequence* seq, double epsilon);
	Jacobian* jacobian_backprop(Sequence* seq);
	Jacobian* jacobian_backprop(SequenceSet* sequence_set);
//	void  train_sequence(Sequence* sequences, bool change_weights);
//	void  train_sequence(SequenceSet* sequences);
	weight_t test_sequence(SequenceSet* sequences);
	weight_t test_sequence(Sequence* sequence);
	weight_t test_sequence(SequenceSet* sequences, bool verbose);
	//weight_t test_sequence_wta(SequenceSet* sequences, bool verbose);

	double evaluate_training_error(SequenceSet* set);
	double evaluate_validation_error(SequenceSet* validation_set);
	double evaluate_test_error(SequenceSet* validation_set);
	double evaluate_training_error(SequenceSet* set, ErrorFunction* error_function);
	double evaluate_validation_error(SequenceSet* validation_set, ErrorFunction* error_function);
	double evaluate_test_error(SequenceSet* validation_set, ErrorFunction* error_function);
	double evaluate_error(SequenceSet* set);
	double evaluate_error(SequenceSet* set, ErrorFunction* error_function);
	vector<double>* evaluate_error_per_sequence(SequenceSet* set, ErrorFunction* error_function);

	weight_t get_percent_correct_binary_decision(SequenceSet* sequenceset, int threshold);
	
/*	std::vector<weight_t>* train(SequenceSet* sequences, unsigned int iterations);
	std::vector<weight_t>* train(SequenceSet* sequenceset,SequenceSet* testset,  unsigned int iterations);
	std::vector<weight_t>* train(SequenceSet* sequenceset,SequenceSet* testset,  unsigned int iterations, unsigned int callback_after_steps, CallbackHandler* callback_handler);
*/
	bool has_cycle();
	bool has_cycle_rec(Node* parent, map<Node*, unsigned int>* nodehash);	

	/**
	 * Orphans are all hidden and output nodes that do not have
	 * any incoming connections.
	 */
	bool has_orphans();
	bool has_deadends();

	bool is_sorted_correctly();

	int remove_secluded_nodes();

	vector<Node*>* get_potential_output_nodes();
	vector<Node*>* get_potential_input_nodes();

/*	std::vector<std::vector<weight_t>*>* auto_predict(vector<weight_t>* in,unsigned int timesteps,  int mapping[], Sequence* sequence, unsigned int seq_offset);
	std::vector<std::vector<weight_t>*>* auto_predict(vector<weight_t>* in,unsigned int timesteps,  int mapping[]);
	std::vector<std::vector<weight_t>*>* auto_predict(vector<weight_t>* in,unsigned int timesteps);
	*/
	bool sanity_check(bool verbose);
	
	weight_t get_sequence_error(Sequence* sequence, bool verbose);
	weight_t get_sequence_error(Sequence* sequence, unsigned int start, unsigned int end, bool verbose);
	weight_t get_sequence_error(Sequence* sequence, unsigned int start, unsigned int end, ErrorFunction* error_function, bool verbose);

	weight_t get_mean_weight();

	void add_wta_output_ensemble(Ensemble* input_ensemble, Ensemble* output_ensemble);
	void add_linear_output_ensemble(Ensemble* input_ensemble, Ensemble* output_ensemble);
	
	void perturb_weights(weight_t energy);

	Connection* find_connection(Node* node_a, Node* node_b);

	void freeze_all_weights(bool freeze);

//	void grow_network_by_ensemble();

	int pluck_connections(weight_t chance);

	weight_t check_gradient(Sequence* sequence);
	weight_t check_gradient(Sequence* sequence, weight_t epsilon);
	weight_t check_gradient(Sequence* sequence, weight_t epsilon, bool verbose);

	
	weight_t calculate_SSE(std::vector<std::vector<weight_t>*>* a, std::vector<std::vector<weight_t>*>* b);

	vector<weight_t>* get_all_initialstates();
	vector<weight_t>* get_all_partial_derivatives();
	vector<weight_t>* get_all_previous_partial_derivatives();
	vector<weight_t>* get_all_weights();
	void set_all_weights(vector<weight_t>* weights);
	
	int get_num_weights();
	int get_num_trainable_weights();

	vector<Node*>* get_nodes_with_name(string name);
	vector<string> get_node_names();
	string get_node_name(unsigned int i);

	void add_ensemble(Ensemble* ensemble);
	void add_ensemble(Ensemble* ensemble, bool add_all_nodes_to_network);

	void remove_ensemble(Ensemble* ensemble);	
	void connect_ensembles(Ensemble* ensemble_from, Ensemble* ensemble_to, bool forward);
	void connect_ensembles(Ensemble* ensemble_from, Ensemble* ensemble_to, bool forward, weight_t weight);
	
	void sort_nodes(vector<Node*>* in, vector<Node*>* out);
	void sort_nodes();
	void sort_nodes(Ensemble* in, Ensemble* out);
	void sort_nodes_deprecated(vector<Node*>* in, vector<Node*>* out);
	void _sort_nodes_rec(Node* node, set<Node*>* tagged,  int* id, vector<Node*>* nodes);

	void randomize();
	void randomize(bool randomize_bias_weights);
	void randomize(bool randomize_bias_weights, double scaling);
	void randomize_initial_states();
	
	void normalize();

	//double get_timeseries_generalization_error(Sequence* sequence, unsigned int context_length);

	vector<Ensemble*>* get_ensembles_by_type(unsigned int ensemble_type);
	
	vector<vector<weight_t>*>* activate_and_return_activations(Sequence* sequence);

	vector<vector<weight_t>*>* collect_node_outputs(vector<Node*> outputs);

	void analysis();
	
	void save(string filename);
	void load(string filename);
	void save_binary(string filename);
	void load_binary(string filename);
	
	void grow_buffer(unsigned int length);
	
	void add_node(Node* node);
	void remove_node(Node* node);
	

};

#endif
