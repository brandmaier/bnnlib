/*
 *  Node.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 18.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __NODE_H__
#define __NODE_H__

#include<vector>
#include "../Connection.h"
#include "../functions.h"
//#include "../ensembles/Ensemble.h"
#include <string>
#include <sstream>
#include "../Trainable.h"

// forward declaration of Connection
struct Connection;
struct Ensemble;

using namespace std;

typedef int nodetype;

struct Node : Trainable {


	static const nodetype GENERIC_NODE = 0;
	static const nodetype TANH_NODE = 1;
	static const nodetype SIGMOID_NODE = 2;
	static const nodetype LINEAR_NODE = 3;
	static const nodetype LSTM_NODE = 4;
	static const nodetype PI_NODE = 5;
	static const nodetype BIAS_NODE = 6;
	static const nodetype PRODUCT_NODE = 7;
	static const nodetype THRESHOLD_NODE = 8;
	static const nodetype LN_NODE = 9;
	static const nodetype CONDITIONAL_BIAS_NODE = 10;
	static const nodetype SCALED_TANH_NODE = 11;
	static const nodetype SCALED_SIGMOID_NODE = 12;
	static const nodetype STEEP_SIGMOID_NODE = 13;
	static const nodetype PRODUCTBIASED_NODE = 14;
	static const nodetype SIN_NODE = 15;
	static const nodetype TSIN_NODE = 16;
	static const nodetype UNIFORM_NOISE_NODE = 17;
	static const nodetype GAUSSIAN_NODE = 18;
	static const nodetype ONE_SHOT_BIAS_NODE = 19;
	static const nodetype ALTERNATING_BIAS_NODE = 20;
	static const nodetype GAUSSIAN_NOISE_NODE = 21;
	static const nodetype PSEUDO_OUTPUT_NODE = 22;
	static const nodetype MATCHING_OUTPUT_NODE = 23;
	static const nodetype RELU_NODE = 24;

	static const int GENERIC = 0;
	static const int INPUT = 1;
	static const int OUTPUT = 2;
	static const int LSTM_CEC = 3;
	static const int LSTM_INPUT_GATE = 4;
	static const int LSTM_FORGET_GATE = 5;
	static const int LSTM_OUTPUT_GATE = 6;

	std::vector<Connection*> incoming_connections;
	std::vector<Connection*> outgoing_connections;

	std::vector<weight_t> inbuf;
	std::vector<weight_t> actbuf;
	
	std::vector<weight_t> delta;
	
	int id;
	string name;
	
	int functional_type;
	
	double sparsity_prior;

	Ensemble* ensemble;
	
	weight_t (*activation_function)(weight_t);
	weight_t (*derivative_activation_function)(weight_t);
	
	Node();
	virtual ~Node();
	
	virtual void activate(unsigned int timestep);
	virtual void reserve(unsigned int timesteps);
	virtual void grow_buffer(unsigned int size);
	virtual void reset_buffer(unsigned int timestep);

	Connection* get_outgoing_connection(unsigned int i) {
		return outgoing_connections[i];
	}

	Connection* get_incoming_connection(unsigned int i) {
		return incoming_connections[i];
	}
	
	virtual void add_input(weight_t input, weight_t weight, unsigned int time);
	virtual weight_t get_input_to_weight_derivative(Connection* connection, unsigned int time);
	virtual weight_t get_input_to_activation_derivative(Connection* connection, unsigned int time);
	virtual weight_t get_activation_function_derivative(unsigned int time);

	virtual const nodetype get_type() = 0; //{ return(0); };

	void cut_all_links();

	bool operator<(Node &other)  {
		//std::cout << " sort " << fitness << " vs " << other.fitness << "\n";
		return id < other.id;
	}

	virtual string to_string();

	virtual string get_label()
	{
		stringstream s;
		s << name << " [#" << id << "]";
		return s.str();
	}

	bool has_outgoing_forward_connections()
	{
		for (unsigned int j = 0; j < outgoing_connections.size(); j++)
		{
			if (outgoing_connections[j]->forward) return true;
		}
		return false;
	}
	
	bool has_sparsity_prior() {
	  return (sparsity_prior!=0.0);
	}
	
	double get_sparsity_prior() {
	  return (sparsity_prior);
	}
	
	std::vector<weight_t> get_activations() {
	  return(actbuf);
	}
	
	
	  
};

#endif
