/*
 *  Network.cpp
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 18.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 * 
 *  Networks constitute of input, hidden, output and bias nodes. The classical
 *  architecture of layers is dropped. However, Nodes can be grouped in Ensembles.  
 *  
 * 
 */


#undef VERBOSE_ACTIVATE
#undef DEBUG_DURING_TRAINING 
#undef SAFE_MODE

#define EXPLICIT_ZEROINPUTS
#undef WITH_INITIAL_STATE

//#define RECYCLE_MEMORY

#ifndef NETWORK
#define NETWORK

#include "Network.h"
#include "nodes/Node.h"
#include "nodes/LinearNode.h"
#include "nodes/BiasNode.h"
#include "Connection.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
#include "functions.h"
#include "SequenceSet.h"
#include "nodes/PiNode.h"
#include "nodes/SinNode.h"
#include "nodes/TimedSinNode.h"
#include <time.h>
#include "functions.h"
#include "ensembles/LSTMEnsemble.h"
#include "nodes/TanhNode.h"
#include <fstream>
#include "nodes/ProductNode.h"
#include "nodes/SigmoidNode.h"
#include "ensembles/Ensemble.h"
#include "ensembles/LSTMCounterEnsemble.h"
#include "ensembles/LSTMPlusEnsemble.h"
#include "ensembles/LSTMForgetEnsemble.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "ensembles/RecurrentEnsemble.h"
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include "ensembles/WinnerTakesAllEnsemble.h"
#include "GnuplotGenerator.h"
#include "trainer/BackpropTrainer.h"
#include "nodes/ConditionalBiasNode.h"
#include "nodes/ScaledSigmoidNode.h"
#include "nodes/SteepSigmoidNode.h"
#include "nodes/ScaledTanhNode.h"
#include "ensembles/GateEnsemble.h"
#include "errorfunctions/SquaredErrorFunction.h"
#include "ensembles/LinearMemoryEnsemble.h"
#include "ensembles/SecondOrderEnsemble.h"
#include "ensembles/HarmonicGeneratorEnsemble.h"
#include "nodes/OneShotBiasNode.h"
#include "nodes/PseudoOutputNode.h"
#include "ensembles/SlowEnsemble.h"
#include "nodes/MatchingOutputNode.h"
#include "ensembles/SubtractiveEnsemble.h"
#include "ensembles/LSTMCopyRestore.h"

Network::Network(string filename) {
	init();
	this->load(filename);
}

Network::Network() {
	init();	
}

Network::~Network() {

	// remove Ensembles (from last to first)
	if (ensembles.size()>0) {
	  for (unsigned int i=ensembles.size()-1; i > 0; i--)
	  {
		  remove_ensemble( ensembles[i] );
	  }
	}

	// remove remaining Nodes that were not part of ensembles

	while (size > 0)
	{
		this->remove_node( nodes[0] );
	}

	delete error_function;

}

void Network::init()
{
	//this->total_connections = 0;
	this->nodes.clear();

	this->verbose = false;
	this->size = 0;
	this->timestep = 0;	

	
	this->in_offset=0;
	this->in_size=0;
	this->bias_offset=0;
	this->bias_size=0;
	this->hid_offset=0;
	this->hid_size=0;
	this->out_offset=0;
	this->out_size=0;

	//this->training_method = BACKPROP;
	//std::cout << "Init network with size "<< this->nodes.size() << " and connections: "<< total_connections << "\n";

	this->description = "";

	this->washout_time = 0;
	
	use_initial_states = false;

	error_function = //new CrossEntropyErrorFunction();
	new SquaredErrorFunction();

	// internals
	sanity_allows_deadends = false;

	output_ensemble = NULL;
	input_ensemble = NULL;
}

void Network::reset_gamma(double value)
{
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			//nodes[i]->outgoing_connections[j]->derivative = 0.0;
			nodes[i]->outgoing_connections[j]->gamma = value;
		}

	}
}

/**
 * resets all buffers of the network's nodes, set the current timestep to zero
 * and clear all derivatives
 */
void Network::reset() {
	reset(false);	
}

/**
 * resets the buffers (input, activation and delta) of all nodes,
 * resets the current timestep to zero
 * if chosen, reset all derivatives
 */
void Network::reset(bool keep_derivatives)
{
	timestep = 0;
#ifdef SAFE_MODE
	assert(nodes.size() == size);
#endif	
	
#ifdef RECYCLE_MEMORY

	// clear all buffers
	if (!keep_derivatives)
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			//nodes[i]->outgoing_connections[j]->derivative = 0.0;
			nodes[i]->outgoing_connections[j]->reset_derivative();
		}

	}

	 if ((use_initial_states) or (washout_time > 0))
	 {
		 error("Initial States or Washout Time are not implemented for the Network mode RECYCLE_MEMORY");
	 }

	return;
#endif


	// clear all buffers
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		nodes[i]->inbuf.clear();
		nodes[i]->actbuf.clear();
		nodes[i]->delta.clear();
		if (!keep_derivatives)
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			//nodes[i]->outgoing_connections[j]->derivative = 0.0;
			nodes[i]->outgoing_connections[j]->reset_derivative();
		}

	}

	vector<weight_t>* input = new vector<weight_t>();
	for (unsigned int i=0; i < in_size;i++)
	{
		input->push_back(0.0);
	}
	for (unsigned int i=0; i < washout_time; i++)
	{
		this->_activate(input);
	}
	delete input;

	if (use_initial_states)
	{
		for (unsigned int i=0; i < nodes.size(); i++)
		{
#ifdef SAFE_MODE
			assert( nodes[i]->initial_state==nodes[i]->initial_state );
#endif
			if (!keep_derivatives)
				nodes[i]->reset_derivative();
			nodes[i]->reserve(1);
			nodes[i]->inbuf.push_back( nodes[i]->weight );
			nodes[i]->actbuf.push_back(0);
			nodes[i]->activate(0);
			nodes[i]->delta.push_back( 0 );
		}
		timestep++;
	}
}

Connection* Network::connect(Node* node1, Node* node2, bool forward, weight_t value) {
	Connection* c = Network::connect(node1, node2, forward);
//	c->weight =  rand() / (RAND_MAX + 1.) * 0.4 - 0.2; 
	c->weight = value;
	return c;
}

/**
 requires: node1 and node2 are already added to Nodelist
 **/
Connection* Network::connect(Node* node1, Node* node2, bool forward) {
	
	assert(node1!=NULL && node2!=NULL);
	
	if ((node1 == node2) && (forward))
	{
		std::cout << "Warning: self-forward connection is impossible! Continuing with recurrent connection." << endl;
		forward = false;
	}
	
	Connection* connection = new Connection(node1, node2, forward);
	
	(*node1).outgoing_connections.push_back(connection);
	(*node2).incoming_connections.push_back(connection);
	
//	std::cout << "connected " << (*node1).id << " to " << (*node2).id << " with weight=" << (*connection).weight <<"\n";
	return connection;
	//total_connections++;
}


Connection* Network::find_connection(Node* node_a, Node* node_b)
{
	for (unsigned int i=0; i < node_a->outgoing_connections.size(); i++)
	{
		if (node_a->outgoing_connections[i]->to == node_b) return node_a->outgoing_connections[i];
	}

	return NULL;
}

/**
 * get network output
 * TODO: not_efficient
 **/
std::vector<weight_t>* Network::get_output(unsigned int time) {
	
	std::vector<weight_t>* target = new std::vector<weight_t>();
	target->reserve(out_size);
	for (unsigned int i=0; i < this->out_size; i++) 
	{
	 target->push_back( this->nodes[this->out_offset+i]->actbuf[time] );
	}


	
	return target;
}

/**
 * 
 * 
 **/
void Network::activate(Sequence* sequence)
{
	//cout << "activate" << endl;
	this->reset();
	this->reserve(sequence->size());
	for (unsigned int i=0; i < sequence->size(); i++)
	{

#ifdef VERBOSE_ACTIVATE	
		std::cout << "---Activate Sequence " << i << endl << endl;
#endif		
		this->_activate( sequence->get_input(i) );	
	}
	
	//std::cout << "done activate sequence" << endl;
}

std::vector<std::vector<weight_t>*>* Network::get_outputs_until(unsigned int time)
{
	std::vector<std::vector<weight_t>*>* temp = new std::vector<std::vector<weight_t>*>();
	temp->reserve(time);
	
	for (unsigned int i=0; i < time; i++)
	{
		temp->push_back ( this->get_output(i) );
	}	

	return temp;

}

std::vector<std::vector<weight_t>*>* Network::get_last_outputs(unsigned int n)
{
	std::vector<std::vector<weight_t>*>* temp = new std::vector<std::vector<weight_t>*>();
	temp->reserve(n);

	for (unsigned int i=0; i < n; i++)
	{
		temp->push_back ( this->get_output( timestep-n+i ) );
	}

	return temp;

}

std::vector<std::vector<weight_t>*>* Network::get_outputs_between(unsigned int start, unsigned int end)
{
	std::vector<std::vector<weight_t>*>* temp = new std::vector<std::vector<weight_t>*>();
	temp->reserve(end-start);

	for (unsigned int i=start; i < end; i++)
	{
		temp->push_back ( this->get_output(i) );
	}

	return temp;

}

vector<vector<weight_t>*>* Network::activate_and_return_activations(Sequence* sequence)
{
	vector<vector<weight_t>*>* activations = new vector<vector<weight_t>*>();
	this->activate( sequence);	

	for (unsigned int j=0; j < size; j++)
	{
		vector<weight_t>* temp = new vector<weight_t>();
		for (unsigned int i=0; i < sequence->size(); i++)
		{
			//std::cout << j << "/" << size << ":" << nodes[j]->actbuf[0] << endl;
			temp->push_back( nodes[j]->actbuf[i] );

		}
		activations->push_back( temp );
	}
	
	std::cout << activations->size() << endl;
	
	return activations;
}

/**
 activation by forward propagation of an input signal
 from the input nodes to the output nodes.

 requires a topological sorted network (nodes have
 to be sorted according to the Graph that is represented
 by forward connections only).

 procedure:
 - activate all Bias units
 - for all nodes:
	 - fetch all activations from incoming backward connections
	 - activate current node
	 - propagate all activations to outgoing forward connections
**/
void Network::_activate(std::vector<weight_t>* input) {

#ifdef VERBOSE_ACTIVATE	
	std::cout << "========= activation (t=" << timestep << " inbufsize=~" << nodes[0]->inbuf.size() << endl;	
#endif	
	
#ifdef SAFE_MODE	
	//cout << (*input).size() << " " << in_size << endl;
	assert( (*input).size() == in_size);

#endif	
	
	// create buffer in all nodes
	grow_buffer(1);
	
	for (unsigned int i=0; i < bias_size; i++) {
		nodes[this->bias_offset+i]->activate(timestep);
#ifdef VERBOSE_ACTIVATE			
		std::cout << "activated bias node '"<<nodes[this->bias_offset]->name << "' with " << nodes[this->bias_offset]->actbuf[timestep] << "\n";
#endif		


#ifdef SAFE_MODE
	if (isnan(nodes[this->bias_offset+i]->actbuf[timestep])) {
		cout << "ERROR! Activation is NAN! Node: "<< nodes[this->bias_offset+i]->name << " inbuf: "<< nodes[this->bias_offset+i]->inbuf[timestep] << endl;
		exit(0);
	}
	if (isinf(nodes[this->bias_offset+i]->actbuf[timestep])) {
		cout << "ERROR! Activation is INF! Node: "<< nodes[this->bias_offset+i]->name << " inbuf: "<< nodes[this->bias_offset+i]->inbuf[timestep] << endl;
		exit(0);
	}
#endif
	}
	
	// set input to buffers of the input neurons
	//std::cout << "activate network\n";
	for (unsigned int i=0; i < in_size;i++) {
		nodes[this->in_offset+i]->inbuf[timestep] = (*input)[i];
		nodes[this->in_offset+i]->activate(timestep);
#ifdef VERBOSE_ACTIVATE			
		std::cout << "activated input node '"<<nodes[this->in_offset+i]->name << "' with " << nodes[this->in_offset+i]->actbuf[timestep] << "\n";
#endif

#ifdef SAFE_MODE
	if (isnan(nodes[this->in_offset+i]->actbuf[timestep])) {
		cout << "ERROR! Activation is NAN! Node: "<< nodes[this->in_offset+i]->name << " #" <<
				nodes[this->in_offset+i]->id  << " inbuf: "<< nodes[this->in_offset+i]->inbuf[timestep] << endl;
		exit(0);
	}
	if (isinf(nodes[this->in_offset+i]->actbuf[timestep])) {
		cout << "ERROR! Activation is INF! Node: "<< nodes[this->in_offset+i]->name << " #" <<
				nodes[this->in_offset+i]->id  << " inbuf: "<< nodes[this->in_offset+i]->inbuf[timestep] << endl;
		exit(0);
	}
#endif

	}
#ifdef VERBOSE_ACTIVATE			
	cout << "-- collect backward activations" << endl;
#endif	
	// propagate signal
	for (unsigned int i=0; i < size; i++) {
		// fetch all values from incoming backward connections
		for (unsigned int j=0; j < nodes[i]->incoming_connections.size(); j++) {
			Connection conn = *nodes[i]->incoming_connections[j];			
			if (!conn.forward) {
					if (timestep > 0) {
						//nodes[i]->inbuf[timestep] += (*conn.from).actbuf[timestep-1]*conn.weight;
						nodes[i]->add_input( (*conn.from).actbuf[timestep-1], conn.weight, timestep );
					//	cout << "fetching from " << (*conn.from).name << (*conn.from).actbuf[timestep-1] << "," << conn.weight << endl;
					}
#ifdef EXPLICIT_ZEROINPUTS
					else
					{
					//	cout << "fetching from " << (*conn.from).name << "0,0" << endl;
#ifdef WITH_INITIAL_STATE
						nodes[i]->add_input( nodes[i]->initial_state, conn.weight, timestep);
#else
						nodes[i]->add_input( 0.0, conn.weight, timestep );
#endif
					}
#endif


					/*if (isnan(nodes[i]->inbuf[timestep])) {
						cout << "inbuf is nan for " << nodes[i]->name << " with " << conn.weight << endl;
						exit(-1);
					}*/
			}
		}
		
		// activate
		nodes[i]->activate(timestep);

#ifdef VERBOSE_ACTIVATE			
		std::cout << "\033[34mactivate hidden '" << nodes[i]->name<< " #" << nodes[i]->id   << "' with x=" << nodes[i]->inbuf[timestep] << " to y=" <<  nodes[i]->actbuf[timestep] << "\033[0m at time " << timestep << "\n";
#endif

#ifdef SAFE_MODE
	if (isnan(nodes[i]->actbuf[timestep])) {
		cout << "ERROR! Activation is NAN! Node: "<< nodes[i]->name << " inbuf: "<< nodes[i]->inbuf[timestep] << endl;
		exit(0);	
	} 
	if (isinf(nodes[i]->actbuf[timestep])) {
		cout << "ERROR! Activation is INF! Node: "<< nodes[i]->name << " inbuf: "<< nodes[i]->inbuf[timestep] << endl;
		exit(0);
	}
#endif
		
		// propagate
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {
			Connection* conn = nodes[i]->outgoing_connections[j];
			if (conn->forward) {
#ifdef VERBOSE_ACTIVATE					
				std::cout << "send from " << nodes[i]->id << " to " <<  conn->to->id << " value " << (nodes[i]->actbuf[timestep]) << "*"<< (conn->weight) << "=" <<
				(nodes[i]->actbuf[timestep] * conn->weight) << "\n";
#endif
#ifdef SAFE_MODE				
				if (isnan( (nodes[i]->actbuf[timestep]) )) {
					cout << "ERROR HAS BECOME NAN!" << endl;
					exit(0);	
			}
#endif				
				//conn.to->inbuf[timestep] += nodes[i]->actbuf[timestep] * conn.weight;
				conn->to->add_input(nodes[i]->actbuf[timestep], conn->weight, timestep );
			}
		}
	}
	
#ifdef VERBOSE_ACTIVATE
	cout << "finish activation propagation" << endl;
#endif

	// do idiosyncratic ensemble stuff
	for (unsigned int i=0; i < ensembles.size(); i++)
	{
		WinnerTakesAllEnsemble* wta = dynamic_cast<WinnerTakesAllEnsemble*>(ensembles[i]);	
		if (wta != NULL) {
			wta->_after_activation(timestep);	
		}
	}
	

	timestep++;
		
}





int Network::pluck_connections(weight_t chance)
{
	assert((0. < chance) and (chance < 1.));

	unsigned int count = 0;

	for (unsigned int i=0; i < size; i++) 
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {	
			weight_t r = rand()/(RAND_MAX+1.0);
			if (r < chance) {
				nodes[i]->outgoing_connections[j]->remove();
				count++;
			}
		}
		
	}	
	
	return count;
}

/**
 debug output
 **/
void Network::debug() {

	std::cout << "\n==========\nNetwork, current time = " << timestep <<"\nInbuf/Actbuf:\n";
	for (unsigned int i=0; i < size; i++) {
		std::cout << i << "." << nodes[i]->name << ":";
		assert( ( nodes[i]->inbuf.size())==nodes[i]->actbuf.size() );
		for (unsigned int j=0; j < nodes[i]->inbuf.size(); j++) {
			std::cout << " | "<< nodes[i]->inbuf[j] << "/" << nodes[i]->actbuf[j] << "/" << nodes[i]->delta[j] << " ";
		}
		std::cout << "\n";
	}
	
	for (unsigned int i=0; i < size; i++) 
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {
			std::cout << (nodes[i]->outgoing_connections[j]->forward==true?"forward":"backward") << " from " << nodes[i]->id << " to " << nodes[i]->outgoing_connections[j]->to->id << " has weight " << nodes[i]->outgoing_connections[j]->weight  << "\n";
		}
	}
	std::cout << "======\n";
}


void Network::analysis()
{
	int count_nearzero = 0;
	int all = 0;
	
	for (unsigned int i=0;i < size;i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			all++;
			Connection* connection = nodes[i]->outgoing_connections[j];
		//	std::cout << i << "." << j << endl;
			if (fabs(connection->weight) < 10e-3) {
				count_nearzero++;
			}
			
		}	
	}	
	
	std::cout << "weights near zero : " << count_nearzero << " from " << all << endl;
}

void Network::freeze_all_weights(bool freeze)
{
	for (unsigned int i=0;i < size;i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* connection = nodes[i]->outgoing_connections[j];
			connection->freeze_weight = freeze;
		}
	}	
}

/*
weight_t Network::test_sequence_wta(SequenceSet* sequences, bool verbose)
{
	vector<unsigned int> err( sequences->target_size);
	vector<unsigned int> total( sequences->target_size );

	for (unsigned int k=0; k < sequences->size(); k++) {

		//if (verbose)
		//	std::cout << "Sequence # " << k  << "/" << sequences->size() << endl;

		Sequence* sequence = (*sequences)[k];
		reset();
		activate( sequence );

		for (unsigned int j=0; j < sequence->size(); j++)
		{
			unsigned int i = maxindex(this->get_output(j));
			unsigned int i2 = maxindex(sequence->get_target(j));
			if (i != i2) err[i2]++;
			total[i2]++;
		}

	}

	for (unsigned int i=0; i< 5; i++)
	cout << "Correct " << ((total[i]-err[i])/(weight_t)total[i]) << "%" << endl;


}*/

bool Network::has_cycle()
{

    map<Node*,unsigned int>* nodehash = new map<Node*, unsigned int>();

	for (unsigned int i= in_offset; i < in_offset+in_size; i++)
	{
	 for (unsigned int j=0; j < size; j++)
		(*nodehash)[nodes[j]] = 0;

	  if (has_cycle_rec( nodes[i], nodehash )){
	 	 delete nodehash;
	 	 return true;
	  }
	 

	}

	delete nodehash;	
	return false;
}

bool Network::has_cycle_rec(Node* parent, map<Node*,unsigned int>* nodehash)
{
	if ((*nodehash)[parent]==2) return false;	// [BUG? not sure about htis]
//	std::cout << "add" << parent->id << " v=" <<(*nodehash)[parent] << endl;

	(*nodehash)[parent] = (*nodehash)[parent]=1;	
//	std::cout << "add" << parent->id << " v=" <<(*nodehash)[parent] << endl;
	for (unsigned int j=0; j < parent->outgoing_connections.size(); j++)
	{
		Node* to = parent->outgoing_connections[j]->to;

		if (parent->outgoing_connections[j]->forward == true) {
		 //(*nodehash)[to]++;
		 if ((*nodehash)[to] == 1)
		 {
			std::cout << "Found "<< parent->outgoing_connections[j]->forward << "cycle between " << parent->name << "(" << parent->id
			 <<")" << " and " << to->name << " (" << to->id << ")" << endl;
			return true;
		 } else {
			//std::cout << nodehash->find(to) << endl;
			//std::cout << "recurse to " << to->id << endl;
			if (has_cycle_rec(to, nodehash)) return true;	
			
		 }
		} 		
	}	
	(*nodehash)[parent] = (*nodehash)[parent]=2;
	//std::cout << "set" << parent->id << " v=" <<(*nodehash)[parent] << endl;
	
	return false;
		
}

bool Network::has_orphans()
{
	for (unsigned int i=hid_offset; i < nodes.size(); i++)
	{
		if	(nodes[i]->incoming_connections.size() == 0)
			{
			stringstream sstr;
			sstr << nodes[i]->name << " is an orphan (has no incoming connections)!" << endl;
			warning(sstr.str());
			return true;
			}
	}
	
	return false;
}

bool Network::has_deadends()
{
	for (unsigned int i=0; i < hid_size; i++)
	{
		if	(nodes[hid_offset+i]->outgoing_connections.size() == 0) return true;
	}
	
	return false;
}

/**
 * return all nodes that do not have outgoing connections
 * and are not Bias nodes
 */
vector<Node*>* Network::get_potential_output_nodes()
{
	vector<Node*>* temp = new vector<Node*>();

	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if	((nodes[i]->outgoing_connections.size() == 0)
				&& (nodes[i]->get_type() != Node::BIAS_NODE))
		{
			temp->push_back( nodes[i] );
		}
	}

	return temp;
}

/**
 * return all nodes that have no incoming connections
 * and are not Bias Nodes
 */
vector<Node*>* Network::get_potential_input_nodes()
{
	vector<Node*>* temp = new vector<Node*>();

	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if	((nodes[i]->incoming_connections.size() == 0)
			&& (dynamic_cast<BiasNode*>(nodes[i]) == 0))
			temp->push_back( nodes[i] );
	}

	return temp;
}
/*
void Network::set_seed(unsigned int seed)
{
	this->seed = seed;
	srand(seed);
}
*/
bool Network::sanity_check(bool verbose)
{
	//cout << "deadends" << endl;
	bool deadends = !sanity_allows_deadends && has_deadends();
	//cout << "orphans" << endl;
	bool orphans = has_orphans();
	//cout << "cycle" << endl;
	bool cycle = has_cycle();
	//cout << "sorted" << endl;
	bool notsorted = !is_sorted_correctly();
	
	if (verbose)
	{
		cout << "Sanity Check\n=================" << endl
			<< "Deadends   " << deadends << endl
			<< "Orphans    " << orphans << endl
			<< "Cycles     " << cycle << endl
			<< "Not sorted " << notsorted << endl;
	}
	


	return !deadends && !orphans && !cycle && !notsorted;
}



bool Network::is_sorted_correctly()
{
	assert(size == nodes.size());

	//cout << "nodes : " << size << endl;

	for (unsigned int i=0; i < nodes.size(); i++)
	{
		//cout << "Node "<<i << " has outgoings : " << nodes[i]->outgoing_connections.size() << endl;

		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* c = nodes[i]->outgoing_connections[j];


			if ((c->forward) and (c->to->id <= nodes[i]->id))
			{
				cout << "not sorted: " <<nodes[i]->name << " with id=" << nodes[i]->id << "->" << c->to->name << " with id=" << c->to->id << " have a forward connection between them " << c->forward << endl;

				return false;
			}
		}
	}

	return true;
}

/**
 * reserve buffer space (inputs, activations, deltas)
 * in all of the network's nodes.
 *
 *
 *
 */
void Network::reserve( unsigned int buffer_size )
{
#ifndef RECYCLE_MEMORY
	for (unsigned int j=0; j < this->size; j++)
		nodes[j]->reserve( buffer_size );
#else
	for (unsigned int j=0; j < this->size; j++) {
	 int diff = buffer_size-nodes[j]->inbuf.size();
	 if (diff > 0) {
		//cout << "reserve additional " << diff << " cells" << endl;
		nodes[j]->grow_buffer( diff );
	 }
	}

#endif
}

void Network::grow_buffer( unsigned int length )
{
#ifndef RECYCLE_MEMORY
	for (unsigned int j=0; j < this->size; j++)
		nodes[j]->grow_buffer( length );
#else
	for (unsigned int j=0; j < this->size; j++)
		nodes[j]->reset_buffer( timestep );
#endif
}

std::vector<weight_t>* Network::get_all_partial_derivatives()
{
	std::vector<weight_t>* derivs = new std::vector<weight_t>();
	
	for (unsigned int i=0;i < nodes.size();i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			derivs->push_back( nodes[i]->outgoing_connections[j]->derivative );
		}
	}
	
	return derivs;
}

std::vector<weight_t>* Network::get_all_previous_partial_derivatives()
{
	std::vector<weight_t>* derivs = new std::vector<weight_t>();

	for (unsigned int i=0;i < nodes.size();i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			derivs->push_back( nodes[i]->outgoing_connections[j]->previous_derivative );
		}
	}

	return derivs;
}

void Network::set_all_weights(vector<weight_t>* weights)
{
	unsigned int index = 0;
	for (unsigned int i=0;i < nodes.size();i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			assert(index < weights->size());
			nodes[i]->outgoing_connections[j]->weight = (*weights)[index];
			index++; 
		}
	}	
}

void Network::randomize()
{
	randomize(true);
}

void Network::randomize(bool randomize_bias_weights)
{
	randomize(randomize_bias_weights, 0.2);
}

void Network::randomize(bool randomize_bias_weights, double scaling)
{
 	for (unsigned int i=0;i < nodes.size();i++)
 	{
 	 if (!randomize_bias_weights && nodes[i]->get_type() == Node::BIAS_NODE )
 		 {
			 continue;
 		 }
	 for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
	 {
	 	if (!nodes[i]->outgoing_connections[j]->freeze_weight)
			nodes[i]->outgoing_connections[j]->weight =  rand() / (RAND_MAX + 1.) * (2.0*scaling)- scaling;
	 }
	} 	
}

void Network::randomize_initial_states()
{
	weight_t a = 0.2;
 	for (unsigned int i=0;i < nodes.size();i++)
 	{
 		nodes[i]->weight = rand_uniform_01()*a-(a/2);
 	}
}

std::vector<weight_t>* Network::get_all_weights()
{
	std::vector<weight_t>* weights = new std::vector<weight_t>();
	
	for (unsigned int i=0;i < nodes.size();i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{

			weights->push_back( nodes[i]->outgoing_connections[j]->weight );
		}
	}
	
	return weights;	
}

std::vector<weight_t>* Network::get_all_initialstates()
{
	std::vector<weight_t>* states = new std::vector<weight_t>();

	for (unsigned int i=0;i < nodes.size();i++)
	{

		states->push_back( nodes[i]->weight );

	}

	return states;
}

	
int Network::get_num_weights()
{
	int count=0;
	for (unsigned int i=0;i < nodes.size();i++)
	{
		count += nodes[i]->outgoing_connections.size();
	}	
	
	return count;
}

int Network::get_num_trainable_weights()
{
	int count=0;
	for (unsigned int i=0;i < nodes.size();i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
			if (! nodes[i]->outgoing_connections[j]->freeze_weight)
				count ++;
	}

	return count;
}

weight_t Network::test_sequence(Sequence* sequence)
{
	SequenceSet* seq = new SequenceSet();
	
	seq->add_sequence( sequence );
	
	weight_t result = this->test_sequence( seq );
	delete seq;
	
	return result;
}

weight_t Network::test_sequence(SequenceSet* sequences)
{
	return this->test_sequence(sequences, false);	
}

/**
 * unfinished/untested
 */
void Network::reinitialise()
{
	reset();
	randomize();

	for (unsigned int i=0;i < size;i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			nodes[i]->outgoing_connections[j]->gamma = 0.1;
			nodes[i]->outgoing_connections[j]->previous_derivative = 0;
			nodes[i]->outgoing_connections[j]->previous_weight_change = 0;
		}

		if (use_initial_states) {
		 nodes[i]->weight = rand_uniform_01()*0.2-0.1;
		} else {
		 nodes[i]->weight = 0;
		}
	}

	//for (unsigned int i=0; i < size)

}

vector<vector<weight_t>*>* Network::collect_node_outputs(vector<Node*> outputs)
{
	vector<vector<weight_t>*>* out = new vector<vector<weight_t>*>();

	for (unsigned int t=0; t < timestep; t++)
	{
		vector<weight_t>* slice = new vector<weight_t>();
		for (unsigned int i=0; i < outputs.size();i++)
		{
			slice->push_back( outputs[i]->actbuf[t]  );
		}
		out->push_back(slice);
	}

	return out;
}

weight_t Network::get_percent_correct_binary_decision(SequenceSet* sequenceset, int threshold)
{


	unsigned int corr = 0;
	unsigned int tot = 0;

	for (unsigned int k=0; k < sequenceset->size(); k++) {
		Sequence* sequence = sequenceset->get(k);
		reset();
		 unsigned int seq_corr = 0;
		 unsigned int seq_tot = 0;
	 for (unsigned int i=0; i < sequence->size(); i++)
	 {

		this->_activate( sequence->get_input(i) );
		std::vector<weight_t>* output = this->get_output(timestep-1);
		std::vector<weight_t>* target = sequence->get_target(i);
		for (unsigned int j=0; j < output->size(); j++)
		{
		//	bool test = false;
			if (( ((*output)[j] > threshold) and ((*target)[j] > threshold) )
				or ( ((*output)[j] < threshold) and ((*target)[j] < threshold) ) )
			{
				corr++; seq_corr++;
			//	test = true;
			}
			tot++;seq_tot++;
			//cout <<  (*sequence->get_input(i))[0] << " " << (*output)[j] << " vs " << (*target)[j] << " = "  << endl;

		}
		delete output;
	 }

	// cout << "Sequence #" << k << ": " << (seq_corr/(double)seq_tot) << endl;

	}

	return corr/(double)tot;
}

weight_t Network::get_sequence_error(Sequence* sequence, bool verbose)
{
	this->reset();
	return get_sequence_error(sequence, 0, sequence->size(), this->error_function, verbose);
}

weight_t Network::get_sequence_error(Sequence* sequence,
		unsigned int start, unsigned int end, bool verbose)
{
	return get_sequence_error(sequence, start, end, this->error_function, verbose);
}

/**
 * make sure to call the appropriate reset()-method if necessary!
 */
weight_t Network::get_sequence_error(Sequence* sequence,
		unsigned int start, unsigned int end, ErrorFunction* error_function, bool verbose)
{
	weight_t error = 0.0;
	
//	unsigned int tot = 0;
//	unsigned int corr=0;
	//verbose=false;

	//grow_buffer( sequence->size() );	
	reserve( sequence->size() );		//TODO: rather (end-start) than sequence->size() ??

	for (unsigned int i=start; i < end; i++)
	{
	/*	if (verbose)
		 std::cout << "Step # "<< i << " (network t = "<< (timestep) << ")\n";
		*/
		this->_activate( sequence->get_input(i) );
		std::vector<weight_t>* output = this->get_output(timestep-1);
		std::vector<weight_t>* target = sequence->get_target(i);
#ifdef SAFE_MODE		
		assert(output->size() == target->size() );
#endif	

		/*if (verbose) {
			std::cout << "i= ";
			for (unsigned int j=0; j < sequence->get_input(i)->size(); j++)
			{
				std::cout << (*sequence->get_input(i))[j] << "\t";
			}
			std::cout << endl;
		}*/
		
		error += error_function->get_error(target, output);

		/*if (verbose)
		 for (unsigned int j=0; j < output->size(); j++)
		 {

					 std::cout << " : o=" << (*output)[j] << " t="<<(*target)[j]<<"\n";
		 }*/

	/*	for (unsigned int j=0; j < output->size(); j++)
		{
			if (verbose)
			 std::cout << " : o=" << (*output)[j] << " t="<<(*target)[j]<<"\n";
			if (!isnan((*target)[j]))
			//error += ((*output)[j] - (*target)[j])*((*output)[j] - (*target)[j]);
				error += error_function->get_error(
						(*target)[j], (*output)[j]
				);


		}
	*/

		delete output;
	}
	
	//cout << "xxx " << (corr/(double)tot) <<endl;
	
	//error = sqrt(error);

	return error;	
}

double Network::evaluate_validation_error(SequenceSet* validation_set)
{
	return evaluate_validation_error(validation_set, error_function);
}

double Network::evaluate_training_error(SequenceSet* training_set)
{
	return evaluate_training_error(training_set, this->error_function);
}

/**
 * evaluates the error of a network on a given validation set with a given error function. Note
 * that only the subsequences in the validation set are used for this evaluation that are
 * explicitly marked in the sequence by validation_begin and validation_end.
 *
 */
double Network::evaluate_validation_error(SequenceSet* validation_set, ErrorFunction* error_function)
{
	double error = 0.0;

	for (unsigned int i=0; i < validation_set->size(); i++)
	{
		Sequence* sequence = (*validation_set)[i];

		// reset network
		reset();

		// activate training input of sequence
		for (unsigned int t = sequence->training_begin; t < sequence->training_end; t++) {
		 _activate( sequence->input[t] );
		}

		// sanity check
		if (sequence->validation_begin== sequence->validation_end)
		{
			warning("No validation set defined for a sequence in the validation set when trying to evaluate error!");
		}

		// evaluate validation error of sequence
		double sequence_error = get_sequence_error( sequence,
				sequence->validation_begin, sequence->validation_end, error_function, false);

		// update total error
		error += sequence_error;
	}

	return error;
}

double Network::evaluate_test_error(SequenceSet* test_set)
{
	return evaluate_test_error(test_set, this->error_function);
}

double Network::evaluate_test_error(SequenceSet* test_set, ErrorFunction* error_function)
{
	if (test_set->size() == 0) {
		error("Test set may not be empty when evaluating test error!");
	}

	assert(test_set->size() > 0);

	double error = 0.0;

	for (unsigned int i=0; i < test_set->size(); i++)
	{
		Sequence* sequence = (*test_set)[i];

		// reset network
		reset();

		// activate training input of sequence
		for (unsigned int t = sequence->training_begin; t < sequence->training_end; t++) {
		 _activate( sequence->input[t] );
		}

		// activate test input of sequence
		for (unsigned int t = sequence->validation_begin; t < sequence->validation_end; t++) {
		 _activate( sequence->input[t] );
		}

	/*	if (sequence->test_begin== sequence->test_end)
		{
			warning("No test set defined for Sequence when trying to evaluate error!");
		}
*/
		// evaluate validation error of sequence
		double sequence_error = get_sequence_error( sequence,
				sequence->test_begin, sequence->test_end, false);

		// update total error
		error += sequence_error;
	}

	return error;
}

double Network::evaluate_training_error(SequenceSet* set, ErrorFunction* error_function)
{
	double error = 0.0;

	//SquaredErrorFunction error_function;

	for (unsigned int i=0; i < set->size(); i++)
	{
		Sequence* sequence = (*set)[i];

		reset();

		double sequence_error = get_sequence_error( sequence,
				sequence->training_begin, sequence->training_end, error_function, false);

		error+=sequence_error;
	}

	return error;
}

double Network::evaluate_error(SequenceSet* set)
{
	return this->evaluate_error(set, this->error_function);
}

vector<double>* Network::evaluate_error_per_sequence(SequenceSet* set, ErrorFunction* error_function)
{
	vector<double>* error = new vector<double>();



	for (unsigned int i=0; i < set->size(); i++)
	{
		Sequence* sequence = (*set)[i];

		reset(true);		//TODO BUG: this has been False - has this side-effects? has to be true for CGLearner
		this->reserve(sequence->size());

		double serror = 0;
		//cout << "Sequence #" << i << "Size : " << sequence->size() << endl;
		for (unsigned int j=0; j < sequence->size(); j++)
		{

			_activate( sequence->input[j] );
//			if (!isnan(sequence->target[j]))
			vector<weight_t>* output = this->get_output(timestep-1);
			 serror+=error_function->get_error( sequence->target[j],
					output ) ;
			delete output;

		/*	cout << sequence->target[j] << " vs " <<
					this->get_output(timestep-1) << error_function->get_error( sequence->target[j],
							this->get_output(timestep-1) )<<endl;*/
		}
		error->push_back( serror );

	}

	return error;
}

double Network::evaluate_error(SequenceSet* set, ErrorFunction* error_function)
{
	double error = 0.0;



	for (unsigned int i=0; i < set->size(); i++)
	{
		Sequence* sequence = (*set)[i];

		reset(true);		//TODO BUG: this has been False - has this side-effects? has to be true for CGLearner
		this->reserve(sequence->size());

		//cout << "Sequence #" << i << "Size : " << sequence->size() << endl;
		for (unsigned int j=0; j < sequence->size(); j++)
		{

			_activate( sequence->input[j] );
//			if (!isnan(sequence->target[j]))
			vector<weight_t>* output = this->get_output(timestep-1);
			error += error_function->get_error( sequence->target[j],
					output );
			delete output;

		/*	cout << sequence->target[j] << " vs " <<
					this->get_output(timestep-1) << error_function->get_error( sequence->target[j],
							this->get_output(timestep-1) )<<endl;*/
		}

	}

	return error;
}



/**
 * not yet tested
 */
void Network::add_wta_output_ensemble(Ensemble* in_ensemble, Ensemble* output_ensemble)
{
	WinnerTakesAllEnsemble* wta = new WinnerTakesAllEnsemble(Node::LINEAR_NODE, out_size );
	add_ensemble(wta);

	// connect all former output nodes to new ensemble nodes
	connect_ensembles(output_ensemble, wta, true);

	sort_nodes(in_ensemble, wta);

	if (!sanity_check(false))
		error("Network did not pass sanity check after adding WTA layer!");

}

void Network::add_linear_output_ensemble(Ensemble* input_ensemble, Ensemble* output_ensemble)
{
	Ensemble* linear = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	add_ensemble(linear);

	// connect all former output nodes to new ensemble nodes
	connect_ensembles(output_ensemble, linear, true);

	sort_nodes(input_ensemble, linear);

	if (!sanity_check(false))
		error("Network did not pass sanity check after adding linear layer!");
}


weight_t Network::test_sequence(SequenceSet* sequences, bool verbose)
{
	if (verbose)
		std::cout << "Test\n";

	weight_t error = 0.0;

	for (unsigned int k=0; k < sequences->size(); k++) {

		Sequence* sequence = (*sequences)[k];
		
		if (verbose)
			std::cout << "Sequence # " << k  << "/" << sequences->size() << "  (len=" <<sequence->size() <<")"<< sequence->name <<  endl;


		weight_t seqerror = get_sequence_error(sequence, verbose);
		error+=seqerror;
		if (verbose)
			std::cout << "Sequence #"<<k<< " squared-error = " << seqerror << "\n";
	
		if (isnan(error)) {
			std::cout << "WARNING! Error has become NaN! Try to decrease the learning rate to stabilize the Gradient Descent." << endl;
			std::exit(EXIT_FAILURE);	
		}
		
	}
	
	if (verbose)
		std::cout << "TotalSquaredError = " << error << ", MeanSequenceError = " << (error/sequences->size()) <<endl;
	
	return error;
	
	

}

vector<Ensemble*>* Network::get_ensembles_by_type(unsigned int ensemble_type)
{
	vector<Ensemble*>* result = new vector<Ensemble*>();
	
	for (unsigned int i=0; i < ensembles.size();i++)
	{
		if (ensembles[i]->get_type() == ensemble_type)
			result->push_back( ensembles[i] );	
	}
	
	return result;	
}


void Network::add_node(Node* node)
{
	this->nodes.push_back(node);
}

void Network::add_ensemble(Ensemble* ensemble)
{
	this->add_ensemble(ensemble, true);	
}
/**
 * 
 * add all Nodes from Ensemble to the Network
 * The Ensemble will not yet be connected, though!
 * 
 **/
void Network::add_ensemble(Ensemble* ensemble, bool add_nodes_to_network)
{
	ensembles.push_back( ensemble );
	
	if (add_nodes_to_network) {
		this->nodes.reserve( this->nodes.size() + ensemble->nodes.size() );
		for (unsigned int i=0; i < ensemble->nodes.size(); i++)
		{
		 this->nodes.push_back( ensemble->nodes[i] );
		}
	}
	
//	this->reset();
}

void Network::remove_node(Node* node)
{
	for (unsigned int i=0; i < size; i++)
	{
		if (nodes[i]==node) {
			nodes.erase( nodes.begin()+i);
			delete node;

			if (i < bias_offset+bias_size) {
				bias_size--;
				in_offset--;
				hid_offset--;
				out_offset--;
			}
			else if (i < in_offset+in_size) {
				in_size--;
				hid_offset--;
				out_offset--;
			} else if (i < hid_offset+hid_size) {
				hid_size--;
				out_offset--;
			} else {
				out_size--;
			}
			size--;

			return;
		}
	}

	warning("memory leak: could not delete Node: "+ node->name+".  ");
}

void Network::remove_ensemble(Ensemble* ensemble)
{
	// delete all Nodes from the Ensemble
	for (unsigned int j=0; j < ensemble->nodes.size(); j++)
	{
		this->remove_node( ensemble->nodes[j] );
	}

	// delete Ensemble && Ensemble entry in Network
	for (unsigned int i=0; i < ensembles.size(); i++) {
		if (ensembles[i] == ensemble) {
			//cout << "ensemble #" << i << endl;
			ensembles.erase(ensembles.begin()+i);
			delete ensembles[i]; //TODO BUG remove ensemble somewhere?!
			break;
		}
	}
}


/*
void Network::remove_ensemble(Ensemble* ensemble)
{
	// find Ensemble and delete it from Ensemble list
	for (vector<Ensemble*>::iterator i = ensembles.begin(); i < ensembles.end(); i++)
	{
		if (*i == ensemble) {
			
			ensembles.erase( i );
			
			break;
		}
	}
	
	// remove all Connections that lead to Ensemble
	for (unsigned int i=0; i < size; i++)
	{
	 for (vector<Connection*>::iterator j= nodes[i]->outgoing_connections.begin(); j < nodes[i]->outgoing_connections.end(); j++)
	 {
	 	if (ensemble->contains_node( (*j)->to ))
	 	{
	 		nodes[i]->outgoing_connections.erase(j);
	 	}
	 }
	}	
	
	// delete the Ensemble
	delete ensemble;
}	
*/



/**
 * 
 * connect all output Nodes from one Ensemble with all input Nodes
 * from a second Ensemble. All weights between the two Nodes
 * are set to the given weight value.
 * 
 **/
void Network::connect_ensembles(Ensemble* ensemble_from, Ensemble* ensemble_to, bool forward, weight_t value)
{
	// connect the ensembles in the usual way
	Network::connect_ensembles( ensemble_from, ensemble_to, forward);

	vector<Node*>* outputs = ensemble_from->get_outputs();
	vector<Node*>* inputs = ensemble_to->get_inputs();
	// set all connections between the ensembles to the given value manually
	for (unsigned int i=0; i < outputs->size(); i++)
	{
		for (unsigned int j=0; j < (*outputs)[i]->outgoing_connections.size(); j++)
		{
			for (unsigned int k=0; k < inputs->size(); k++)
			{
				if ((*inputs)[k] == (*outputs)[i]->outgoing_connections[j]->to)
				{
					(*outputs)[i]->outgoing_connections[j]->weight = value;
				}	
			}		
			
		}
	}
	
}

void Network::connect_ensembles(Ensemble* ensemble_from, Ensemble* ensemble_to, bool forward)
{
	unsigned int counter = 0;
	
	vector<Node*>* outputs = ensemble_from->get_outputs();
	vector<Node*>* inputs = ensemble_to->get_inputs();
	for (unsigned int i=0; i < outputs->size(); i++)
	{
		for (unsigned int j=0; j < inputs->size(); j++)
		{
			Network::connect( (*outputs)[i],
				(*inputs)[j], forward );
		}		
	}
	
	counter++;
	
	//std::cout << " established " << counter << " connections "<<endl;
}

void Network::print_offsets()
{
	std::cout << "Bias: "<< bias_offset << "/" << bias_size
		<< " In: "<<in_offset << "/" << in_size
		<< " hid:"<< hid_offset << "/"<< hid_size
		<< " out:"<< out_offset << "/" << out_size << endl;
}

// TODO verify, unfinished	add chance level
void Network::perturb_weights(weight_t energy)
{
	for (unsigned int i=0; i < size; i++) {
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {
			nodes[i]->outgoing_connections[j]->weight += ( (rand()/(RAND_MAX + 2.))-1 ) *energy;
		}		
	}
}

weight_t Network::get_mean_weight()
{
	weight_t mean = 0.0;
	unsigned int count = 0;
	for (unsigned int i=0; i < size; i++) {
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {
			mean += nodes[i]->outgoing_connections[j]->weight;
			count++;
		}
	}
	
	return (mean/count);

}

/**
 * TODO: verify
 */
weight_t Network::calculate_SSE(std::vector<std::vector<weight_t>*>* a, std::vector<std::vector<weight_t>*>* b)
{
	//std::cout << (*a).size() << " " << (*b).size()<<endl;
	assert((*a).size() == (*b).size());
	assert((*a).size() > 0);
	assert((*a)[0]->size()==(*b)[0]->size());	
	
	weight_t error = 0.0;
	
	for (unsigned int i=0; i < a->size(); i++)
	{
		for (unsigned int j = 0; j < (*a)[i]->size();j++)
		{
			//std::cout << ( (*(*a)[i])[j])<<" " <<((*(*b)[i])[j] )<<endl;
			weight_t temp = ( (*(*a)[i])[j]-(*(*b)[i])[j] );
			error = error + (temp*temp);
		}
		
	}
	//std::cout << error << endl;
	return 0.5*error;
}

/**
 * 
 * 
 * dot network.dot -Tpng > dot.png
 * */
void Network::export_to_dot_graph(string filename)
{
  string dotfilename = filename;
  dotfilename.append(".dot");
  
  std::ofstream out(dotfilename.c_str());
  
  out << to_dot_graph();
    
  out.close();	
  
  stringstream cmd;
  cmd << "dot "<<filename <<".dot -Tpng > "<<filename<<".png";
  cout << "executing: " << cmd.str() << endl;
  int returnvalue = system(cmd.str().c_str());
  
  if (returnvalue != 0)
    error("Problem in Dot Graph generation");
}


string Network::to_dot_graph()
{
	assert(size > 0);
	
	bool show_weights = true;

   std::basic_stringstream<char> out;
		
	// ratio=auto;rankdir=TB;
	out << "digraph G { mclimit=1000;\n";

	//out << "graph[splines=true overlap=false];\n";

	// output all nodes
/*	for (unsigned int i=0; i < size; i++)
	{
			if (i==in_offset) {
				out << "subgraph cluster_0 {\n";
			}
			if (i==hid_offset) {
				out << "}\n subgraph cluster_1 {\n";	
			}
			if (i==out_offset) {
				out << "}\n subgraph cluster_2 {\n";	
			}
			if (i==size-1) {
				out << "}\n";	
			}
		
		string color = "white";
		if (i>= out_offset) {
			color="blue";	
		} else if (i >= hid_offset) {
			color="white";	
		} else if (i >= in_offset) {
			color="green";	
		}
		out << "\"n" << nodes[i]->id << "\"[style=\"filled\" label = \"" << nodes[i]->name << "\" fillcolor=\""<<color <<"\"];\n";	
	}
*/
	for (unsigned int i=0; i < ensembles.size(); i++)
	{
		out << "subgraph cluster_"<< i <<" {\nlabel=\""<< ensembles[i]->name <<"\"\n";
		for (unsigned int j=0; j < ensembles[i]->nodes.size(); j++)
		{
			string color = "white";
			out << "\"n" << ensembles[i]->nodes[j]->id << "\"[style=\"filled\" label = \"" << ensembles[i]->nodes[j]->name << "[" <<ensembles[i]->nodes[j]->id <<"]"<< "\" fillcolor=\""<<color <<"\"];\n";
		}
		out << "}\n";

	}

	// output all connections		
	for (unsigned int i=0; i < size; i++) {


			
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {
	
			Connection* conn = nodes[i]->outgoing_connections[j];
			
			string color = conn->forward?"black":"grey";
			if (show_weights)
			out << " n" << conn->from->id << " -> n" << conn->to->id << "[color=\""<< color <<"\" label=\""<< conn->weight <<"\"];\n";
			else
			out << " n" << conn->from->id << " -> n" << conn->to->id << "[color=\""<< color <<"\"];\n";

			
		}
		
	}		
		
		out << "}\n";
	
	return(out.str());
		
	
}


weight_t Network::check_gradient(Sequence* sequence)
{
	return this->check_gradient(sequence, 0.00001, false);
}

weight_t Network::check_gradient(Sequence* sequence, weight_t epsilon)
{
	return this->check_gradient(sequence, epsilon, false);
}


void Network::normalize()
{

	// find mean activations
	/*for (unsigned int i=0; i < size; i++) {

	}*/
	cout << "NOT IMPLEMENTED YET" << endl;
}

/**
 * 
 * TODO memory leak
 * 
 **/
weight_t Network::check_gradient(Sequence* sequence, weight_t epsilon, bool verbose)
{
	vector<weight_t> gradient_estimates;	//TODO efficient
	unsigned int count = 0;
	weight_t diff_sum = 0.0;
	weight_t diff_max = 0.0;

	for (unsigned int i=0; i < size; i++) {
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {


			Connection* conn = nodes[i]->outgoing_connections[j];
			// -- check each weight
			weight_t original_weight = conn->weight;
			
			// calculate +epsilon
			nodes[i]->outgoing_connections[j]->weight = original_weight+epsilon;
			this->reset();
			this->activate( sequence );
			//this->debug();
			std::vector<std::vector<weight_t>*>* output = get_outputs_until( sequence->size() );
			
		//	this->export_to_dot_graph();
			
		//	print_vector((*output)[0]);
		//	print_vector(sequence->target[0]);
		//	print_vector( sequence->get_input(0) );
		//	print_vector(output);
			
			weight_t error_plus = calculate_SSE(&sequence->target, output);

/*			for (unsigned int k; k < output->size(); k++)
				delete (*output)[k];
			delete output;
*/			
//			delete output;

			// calculate -epsilon
			nodes[i]->outgoing_connections[j]->weight = original_weight - epsilon;
			this->reset();
			this->activate( sequence );
			output = get_outputs_until( sequence->size() );
			 
			//print_vector(output);
						 
			weight_t error_minus = calculate_SSE(&sequence->target, output);			
			
/*			for (unsigned int k; k < output->size(); k++)
				delete (*output)[k];
			delete output;			

*/			
//			delete output;

			weight_t estimate = - (error_plus-error_minus) / (2.0*epsilon);
			//std::cout << "Error = " << error_plus << " v.s." << error_minus << "Estimate = "<< estimate << endl;
			
			gradient_estimates.push_back( estimate );
			count++;
			
			// restore weight
			nodes[i]->outgoing_connections[j]->weight = original_weight;
			
			//delete conn;
			
		}
		

		
	}
	
//	std::cout << "CHECK BACKPROP GRADIENT " << endl;
	BackpropTrainer trainer(this);
	reset();
	trainer.learning_rate = 1.0;
	trainer.momentum = 0.0;
	trainer.weight_decay = false;
	trainer.batch_learning = false;
	trainer._train_sequence(sequence, false);


	//print_vector( get_all_partial_derivatives() );

	count = 0;
	for (unsigned int i=0; i < size; i++) {
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++) {

		//	std::cout << "prev " << nodes[i]->outgoing_connections[j]->previous_weight_change << endl;

			if (verbose) {
			 std::cout <<nodes[i]->outgoing_connections[j]->from->get_label() << "->" <<nodes[i]->outgoing_connections[j]->to->get_label() << " "
			  << "#" << count << "[" << nodes[i]->outgoing_connections[j]->forward << "]" << ":  estimate=" << gradient_estimates[count] << " v.s. backprop=" << nodes[i]->outgoing_connections[j]->derivative
			  << " diff=" <<(gradient_estimates[count]-nodes[i]->outgoing_connections[j]->derivative)
			  << endl;
			}
			
			diff_sum += fabs(gradient_estimates[count]-nodes[i]->outgoing_connections[j]->derivative);
			diff_max = max(diff_max,  fabs(gradient_estimates[count]-nodes[i]->outgoing_connections[j]->derivative));
			
		count++;
		}
	}	
		
	//assert(count==size);

	diff_sum = diff_sum / ((double)count);

	std::cout << "Precision: " << epsilon << endl;
	std::cout << "Mean difference : " << diff_sum << endl; 
	std::cout << "Max  difference : " << diff_max << endl;

		
	return diff_sum;
	
}

void Network::load(string filename)
{
  init();


  map<int, Node*> node_map;
  //node_map = map<int, Node*>();
  
  unsigned int node_count = 0;
  unsigned int ensemble_count = 0;
  unsigned int connection_count = 0;

  string line;
  ifstream myfile (filename.c_str());

  int current_section = 0;
  if (myfile.is_open())
  {
	  myfile.precision(40);
	while (! myfile.eof() )
    {
      getline (myfile,line);
     


      if (line.length() == 0) continue;
     
      if (line.substr(0,1) == "[") {
      	//std::cout << "section" << endl;
      	if (line == "[nodes]") current_section = 2; // Nodes
      	if (line == "[connections]") current_section = 3; // Connections
      	if (line == "[ensembles]") current_section = 4; // Ensembles
       	if (line == "[parameters]") current_section = 1; // Parameters
       	if (line == "[ensemble-meta]") current_section = 5; // Meta-Parameter
      } else {
          if  (current_section == 0) {
           	 vector<string> tokens;
           	 string sep = std::string(":");
              split(line,sep,tokens);
              if (tokens[0]=="version") {
            	  double version = atof( tokens[1].c_str() );
            	  if (version < __BNNLIB_VERSION)
            		  error("incompatible file version!");
              }
          }
      	if (current_section == 1) {
      	 vector<string> tokens;
      	 string sep = std::string(":");
         split(line,sep,tokens);    
         //std::cout << tokens[0] << ":" <<  atof( tokens[1].c_str() ) <<endl;
         if (tokens[0]=="in_size") {
         	this->in_size = atof( tokens[1].c_str() );
         } 
         if (tokens[0]=="hid_size") {
         	this->hid_size = atof( tokens[1].c_str() );         	
         }         
         if (tokens[0]=="out_size") {
         	this->out_size = atof( tokens[1].c_str() );         	
         }
         if (tokens[0]=="in_offset") {
         	this->in_offset = atof( tokens[1].c_str() );
         } 
         if (tokens[0]=="hid_offset") {
         	this->hid_offset = atof( tokens[1].c_str() );         	
         }         
         if (tokens[0]=="out_offset") {
         	this->out_offset = atof( tokens[1].c_str() );         	
         }         
         if (tokens[0]=="bias_size") {
         	this->bias_size =  atoi( tokens[1].c_str() );
         }    
         if (tokens[0]=="bias_offset") {
         	this->bias_offset = atof( tokens[1].c_str() );         	
         }
         if (tokens[0]=="use_initial_state") {
        	 this->use_initial_states = atoi(tokens[1].c_str())==1;
         }
         if (tokens[0]=="washout-time") {
         	this->washout_time = atof( tokens[1].c_str() );
         }

      	}

      	// Ensemble
      	// row  0 		= type
      	//		1 		= properties
      	//		2..n 	= nodes
      	if (current_section == 4) {
      	 vector<string> tokens;
      	 string sep = std::string("\t");
         split(line,sep,tokens);
         
         int ensemble_type = atoi(tokens[0].c_str());
         //cout << "Ensemble" << ensemble_type << endl;
         vector<Node*> nodes;
         for (unsigned int i=2; i < tokens.size(); i++)
         {
        	// cout << " |- add: "<< tokens[i] << endl;
         	nodes.push_back( node_map[ atoi(tokens[i].c_str()) ] );
         }
         
         Ensemble* ensemble = NULL;
         switch(ensemble_type)
         {
         	//case Ensemble::GENERIC_ENSEMBLE:
         	// ensemble = new Ensemble(&nodes);
         	//break;
         	//case Ensemble::LSTM_ENSEMBLE:
         	// ensemble = new LSTMEnsemble(&nodes);
         	//break;
			case Ensemble::LSTM_COUNTER_ENSEMBLE:
				 ensemble = LSTMCounterEnsemble::from_string(tokens[1],&nodes);
			break;
			case Ensemble::LSTM_COPYRESTORE_ENSEMBLE:
				 ensemble = LSTMCopyRestore::from_string(tokens[1],&nodes);
			break;
         	case Ensemble::LSTM_FORGET_ENSEMBLE:
         	 ensemble = LSTMForgetEnsemble::from_string(tokens[1],&nodes);
         	break;	
         	case Ensemble::LSTM_PLUS_ENSEMBLE:
         	 ensemble = LSTMPlusEnsemble::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::FEEDFORWARD_ENSEMBLE:
         	 ensemble = FeedforwardEnsemble::from_string(tokens[1],&nodes);
         	break;         
         	case Ensemble::RECURRENT_ENSEMBLE:
         	 ensemble = RecurrentEnsemble::from_string(tokens[1], &nodes);
         	break;
         	case Ensemble::CONDITIONAL_BIAS_ENSEMBLE:
         		error("not implemented yet!");
         	break;
         	case Ensemble::GATE_ENSEMBLE:
         	 ensemble = GateEnsemble::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::LINEAR_MEMORY_ENSEMBLE:
         		ensemble = LinearMemoryEnsemble::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::SECONDORDER_ENSEMBLE:
         		ensemble = SecondOrderEnsemble::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::WINNER_TAKES_ALL_ENSEMBLE:
         		ensemble = WinnerTakesAllEnsemble::from_string(tokens[1], &nodes);
         	break;
         	case Ensemble::HARMONIC_GENERATOR_ENSEMBLE:
         		ensemble = HarmonicGenerator::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::SLOW_ENSEMBLE:
         		ensemble = SlowEnsemble::from_string(tokens[1],&nodes);
         	break;
         	case Ensemble::SUBTRACTIVE_ENSEMBLE:
				ensemble = SubtractiveEnsemble::from_string(tokens[1], &nodes);
         	break;
         	default:

         	stringstream errmsg;
         	errmsg << " Unknown Ensemble type encountered while loading Ensembles : " << ensemble_type;
         	error(errmsg.str());

         }

        // set node references
         for (unsigned int i=0; i < nodes.size();i++)
         {
        	 nodes[i]->ensemble = ensemble;
         }

        this->add_ensemble(ensemble, false);
        ensemble_count++;
      	}
      	
      	if (current_section == 2) {
      	 vector<string> tokens;
      	 string sep = std::string("\t");
         split(line,sep,tokens);
      	 int node_type = atoi(tokens[2].c_str());
      	 //std::cout << "create node with type" << node_type << endl;
      	 Node* node = NULL;
      	 node_count++;
      	 switch(node_type)
      	 {
      	 	case Node::BIAS_NODE:
      	 		node = new BiasNode();
      	 	break;	
      	 	case Node::LINEAR_NODE:
      	 		node = new LinearNode();
      	 	break;
      	 	case Node::TANH_NODE:
      	 		node = new TanhNode();
      	 	break;
      	 	case Node::SIGMOID_NODE:
      	 		node = new SigmoidNode();
      	 	break;
      	 	case Node::PI_NODE:
      	 		node = new PiNode();
      	 	break;
      	 	case Node::CONDITIONAL_BIAS_NODE:
      	 		//cout << tokens.size() << tokens[3] << endl;
      	 		node = ConditionalBiasNode::from_string( tokens[5]);
      	 	break;
      	 	case Node::STEEP_SIGMOID_NODE:
      	 		node = SteepSigmoidNode::from_string(tokens[5]);
      	 	break;
      	 	case Node::PRODUCT_NODE:
      	 		node = new ProductNode();
      	 	break;
      	 	case Node::SIN_NODE:
      	 		node = new SinNode();
      	 	break;
      	 	case Node::TSIN_NODE:
				node = new TimedSinNode();
      	 	break;
      	 	case Node::ONE_SHOT_BIAS_NODE:
				node = new OneShotBiasNode();
      	 	break;
      	 	case Node::PSEUDO_OUTPUT_NODE:
      	 		node = new PseudoOutputNode();
      	 	break;
      	 	case Node::MATCHING_OUTPUT_NODE:
      	 		node = MatchingOutputNode::from_string(this, tokens[5]);
      	 	default:
      	 		stringstream s;
      	 		s << "ERROR! Encountered unknown Node type: " << node_type;
      	 		error( s.str() );

      	 }
      	 node->id = atoi(tokens[0].c_str());
      	 node->name= tokens[1];
      	 node->weight = atof(tokens[3].c_str());
      	 node->functional_type = atof(tokens[4].c_str());
      	 add_node(node);
      	 node_map[node->id] = node;
      	}
      	
      	// ensembles meta
      	if (current_section == 5)
      	{
        	 vector<string> tokens;
         	 string sep = std::string("\t");
             split(line,sep,tokens);

             if (tokens[0]=="input_ensemble:")
             {
            	 unsigned int index = atoi(tokens[1].c_str());
            	 this->input_ensemble = ensembles[index];
             }
             if (tokens[0]=="output_ensemble:")
             {
            	 unsigned int index = atoi(tokens[1].c_str());
            	 this->output_ensemble = ensembles[index];
             }

      	}

      	// connections
      	if (current_section == 3)
      	{
     	 vector<string> tokens;
      	 string sep = std::string("\t");
         split(line,sep,tokens);
         Node* from = node_map[atoi(tokens[0].c_str())];
         Node* to = node_map[atoi(tokens[1].c_str())];
         bool forward = (tokens[3]=="1");
         bool frozen = (tokens[4]=="1");
         double weight = atof(tokens[2].c_str());

        // const double* w = reinterpret_cast<const double*>(tokens[6].c_str());
        // double weight = *w;
         //cout << " " << weight<< endl;
         Connection* c = Network::connect(from, to, forward, weight);
         c->freeze_weight = frozen;
         if (tokens.size() > 5) {
        	c->gamma = atof(tokens[5].c_str());
         }
         connection_count++;
        // std::cout << "new connection from " << from->id << " to " << to->id << " with " << weight << " and direction " << forward << endl;  			
      	}
      	
      }
    } 

    myfile.close();
  }

  else {

	  stringstream err;
	  err << "Unable to open network file: " << filename << endl;
	  error(err.str());
  }

  size = node_count;

  if( size != nodes.size() )
  {
  	stringstream stream;
  	stream << "Error in network file! " << nodes.size() << " nodes stored in network, but only " << size << " mentioned in header."<<endl;
  	error(stream.str() );
  }
 
  /*cout << "connections: " << connection_count << endl;
  cout << "nodes: " << node_count << endl;
  cout << "ensembles: " << ensemble_count << endl;
*/
  //filename.append(".bin");
  //load_binary(filename);
	
}


vector<string> Network::get_node_names() 
{
  vector<string> result;
  for (unsigned int i=0; i < this->nodes.size(); i++)
  {
    result.push_back( this->nodes[i]->name);
  }
  return(result);
}

string Network::get_node_name(unsigned int i)
{
    return(this->nodes[i]->name);
}
 

vector<Node*>* Network::get_nodes_with_name(string name)
{
	vector<Node*>* result = new vector<Node*>();
	for (unsigned int i=0; i < this->nodes.size(); i++)
	{
		if (this->nodes[i]->name==name)
			result->push_back(nodes[i]);
	}

	return result;
}


Jacobian* Network::jacobian_backprop(SequenceSet* sequence_set)
{
	unsigned int max_size = 0;
	for (unsigned int i=0; i< sequence_set->size(); i++)
	{
		if (i > 0) {
			assert(max_size ==sequence_set->get(i)->size() );	// nur gleiche Sequences bisher!
		}
		max_size = max(max_size, sequence_set->get(i)->size());
	}
	cout << "Max Size: " << max_size << endl;

	//TODO  ungleiche sequence-sizes handlen

	Jacobian* jacobian = new Jacobian(in_size, out_size, max_size);

	for (unsigned int i=0; i< sequence_set->size(); i++)
	{
		Jacobian* temp = this->jacobian_backprop(sequence_set->get(i));
		//unsigned int rows = out_size*t;
		//unsigned int cols = in_size*t;
		for (unsigned int i=0; i < temp->num_rows;i++)
		{
			for (unsigned int j=0; j < temp->num_cols; j++)
			{
				(*jacobian->data)[i][j]+=(*temp->data)[i][j];
			}
		}
	}

	return jacobian;
}

/**
 * calculation of the Jacobian matrix (derivatives of all outputs to all inputs)
 *
 */
Jacobian* Network::jacobian_backprop(Sequence* seq)
{
	Jacobian* jacobian = new Jacobian(in_size, out_size, seq->size());

	//unsigned int t = seq->size();

	this->activate(seq);

	//unsigned int timestep = t;


	for (unsigned int t1 = 0; t1 < seq->size(); t1++) {

	 for (unsigned int oi=0; oi < out_size; oi++)
	 {
		// cout << "reset deltas" << endl;
		 // reset deltas
		 for (unsigned int i=0; i < this->size; i++) {
				for (unsigned int t = 0; t < seq->size(); t++) {
					nodes[i]->delta[t] = 0.0;
				}
		 }

		//cout << "backprop" << endl;
		for (unsigned int timestep=seq->size(); timestep>0;timestep--)
		{
			// outputs
			for (unsigned int i=out_offset; i < this->size; i++)
			{
				Node* node = nodes[i];
				if ((i==out_offset+oi) and (timestep-1==t1))
			 	 node->delta[timestep-1] = node->get_activation_function_derivative(timestep-1);
				else
				 node->delta[timestep-1] = 0;
			}
			//cout << "hidden" << timestep << endl;
			// for all hidden units
			for (unsigned int i=size-out_size-1; i > 0; i--)
			{
				weight_t delta_sum = 0.0;
				Node* node = nodes[i];
				for (unsigned int j=0; j < node->outgoing_connections.size(); j++)
				{
					Connection* conn = node->outgoing_connections[j];
					if (conn->forward) {
							delta_sum += conn->to->delta[timestep-1] * conn->to->get_input_to_activation_derivative(conn, timestep-1);
					} else {
						if (timestep < conn->to->delta.size()) {
							delta_sum += conn->to->delta[timestep] * conn->to->get_input_to_activation_derivative(conn, timestep);
						}
					}
				}

				//cout << "set delta"<< i << endl;
				node->delta[timestep-1] +=	//+=
						node->get_activation_function_derivative(timestep-1)
						* delta_sum;
				//cout << "Node #" << i << " t="<<(timestep-1)<<":"<< node->delta[timestep-1] << endl;


			} // end hidden units loop
		}	//finished backprop through time

		// fill Jacobian Matrix
		//cout << "fill Jacobian " << in_size << " row:" << (out_size*t1+oi)<< endl;
		for (unsigned int t2=0; t2 < seq->size(); t2++)
		 for (unsigned int oj=0; oj < in_size; oj++)
		 {
			 //cout << nodes[in_offset+oj]->delta[t2] << endl;
			(*jacobian->data)[out_size*t1+oi][in_size*t2+oj] = nodes[in_offset+oj]->delta[t2];
		 }
	}
	}


	//warning("NOT YET IMPLEMENTED");

	return jacobian;
}

void Network::add_output_to_input_feedback(bool trainable)
{

	for (unsigned int i=out_offset; i < out_offset+out_size; i++)
	{
		for (unsigned int j=in_offset; j < in_offset+in_size; j++)
		{
			Connection* c = connect( nodes[i], nodes[j], BACKWARD);
			c->freeze_weight = !trainable;
		}
	}

}


/**
 *
 * search all children of a (tagged) Node
 * if all successors of a children are tagged, tag the child
 * and restart to examine all not-yet-tagged children
 * if children tag-state does not change anymore, recurse to
 * all tagged children
 *
 *
 *
 */
void Network::_sort_nodes_rec(Node* node,set<Node*>* tagged, int* id, vector<Node*>* unfinished)
{
	bool verbose = true;

	if (verbose) { cout << "_sort_nodes_rec()"; }

	bool cont = true;

	while(cont)
	{
		cont = false;
		// for every child
		for (unsigned int i=0; i < node->incoming_connections.size(); i++)
		{
			Node* child = node->incoming_connections[i]->from;

			if (dynamic_cast<BiasNode*>(child) != 0)  continue;

			if (tagged->find(child) != tagged->end())
				continue;

			bool all_successors_tagged = true;
			for (unsigned int j=0; j < child->outgoing_connections.size(); j++ )
			{
				if (child->outgoing_connections[j]->forward)
				 if(tagged->find(child->outgoing_connections[j]->to) == tagged->end())
				  { /*cout << " -- has successor " << child->outgoing_connections[j]->to->name << endl;*/
					 all_successors_tagged=false; break;	}
			}

//			cout << "- testing child " << child->name << " tagged=" << all_successors_tagged << endl;

			if (all_successors_tagged) {
				cont=true;
				tagged->insert(child);
				if (verbose)
					cout << "ID: " << (*id) << " for " << child->name << endl;
				child->id = *id;
				*id = *id - 1;

			}
		}
	}

//	if (verbose) cout << " recurse with all tagged children" << endl;

	// recurse with all tagged children, if their children are not all
	// tagged

	for (unsigned int i=0; i < node->incoming_connections.size(); i++)
	{
		Connection* conn = node->incoming_connections[i];
		Node* child = conn->from;
		if (conn->forward)
		 if (tagged->find(child) != tagged->end()) {
			 bool all_children_tagged = true;
			 for (unsigned int j=0; j < child->incoming_connections.size();j++) {
				 if (tagged->find(child->incoming_connections[j]->from) == tagged->end())
					 all_children_tagged = false;
			 }
			 if (!all_children_tagged)
			 _sort_nodes_rec( child, tagged, id, unfinished );
		 }
	}

	//if (verbos)
	//	unfinished->erase( unfinished->begin() );

}


struct compare_nodes_by_id {
  bool operator ()(Node *lhs, Node *rhs) { return *lhs < *rhs; }
};

void Network::sort_nodes(Ensemble* in_ensemble, Ensemble* out_ensemble)
{
	this->input_ensemble = in_ensemble;
	this->output_ensemble = out_ensemble;

	vector<Node*>* in = &in_ensemble->nodes;
	vector<Node*>* out = &out_ensemble->nodes;
	sort_nodes(in, out);
}

void Network::sort_nodes()
{
	vector<Node*>* out = get_potential_output_nodes();
	vector<Node*>* in = get_potential_input_nodes();
	sort_nodes(in, out);
	delete out;
	delete in;
}

void Network::sort_nodes(vector<Node*>* in, vector<Node*>* out)
{
	bool verbose = false;

	// prepare tagged node set and id counter
	//set<Node*>* tagged = new set<Node*>();
	vector<Node*> candidates;
	int c_down = nodes.size()-1;
	//int* id = &c;
	int c_up = 0;

	// untag all nodes
	if (verbose) cout << "untag all "<< nodes.size() << " nodes" <<endl;
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		nodes[i]->id = -1;
	}

	// tag all bias
	bias_size = 0;
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if (dynamic_cast<BiasNode*>(nodes[i]) != 0) {
			nodes[i]->id = c_up++;
			if (verbose) cout << "tag Bias " << nodes[i]->name << " with " << nodes[i]->id << endl;
			bias_size++;
		}

	}

	// tag all inputs
	for (unsigned int i=0; i < in->size(); i++)
	{
		(*in)[i]->id = c_up++;
		if (verbose) cout << "tag Input " << (*in)[i]->name
				<< " with " << (*in)[i]->id<< endl;

	}

	// tag all outputs
	for (unsigned int i=0; i < out->size(); i++)
	{
		(*out)[i]->id = c_down--;
		if (verbose) cout << "tag Output " << (*out)[i]->name
				<< " with " << (*out)[i]->id<< endl;

	}

	// add all nodes with no outgoing forwards and at least one incoming
		// to candidates
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if (!nodes[i]->has_outgoing_forward_connections()) {
			if (nodes[i]->incoming_connections.size() > 0) {
				if (nodes[i]->id == -1) {
				 nodes[i]->id = c_down--;
				 if (verbose) cout << "tag potential Output " << nodes[i]->name << " with " << nodes[i]->id<< endl;
				}
				 candidates.push_back(nodes[i]);
			}
		}
	}

	unsigned int k=0;

	while (candidates.size() > 0)
	{
		k++;
		k = k % candidates.size();
		Node* node = candidates[k];

		if (verbose) cout << "-- testing candidate" << node->name << endl;

		// for every child
		for (unsigned int i=0; i < node->incoming_connections.size(); i++)
		{
			Node* child = node->incoming_connections[i]->from;

			if (dynamic_cast<BiasNode*>(child) != 0)  continue;

			if (child->id != -1)
				continue;

			bool all_successors_tagged = true;
			for (unsigned int j=0; j < child->outgoing_connections.size(); j++ )
			{
				if (child->outgoing_connections[j]->forward)
				 if(child->outgoing_connections[j]->to->id == -1)
				  { if (verbose) cout << " -- has successor " << child->outgoing_connections[j]->to->name << endl;
					 all_successors_tagged=false; break;	}
			}

//			cout << "- testing child " << child->name << " tagged=" << all_successors_tagged << endl;

			if (all_successors_tagged) {
				//cont=true;
				//tagged->insert(child);
				//if (verbose)
					//cout << "ID: " << (*id) << " for " << child->name << endl;
				child->id = c_down--;
				if (verbose) cout << "tag hidden child " << child->name << " with " << child->id << endl;
				candidates.push_back( child );
			}
		}

		// if all children are tagged
		bool all_tagged = true;
		for (unsigned int i=0; i < node->incoming_connections.size(); i++)
		{
			Node* child = node->incoming_connections[i]->from;
			if (child->id == -1) all_tagged = false;
		}
		if (all_tagged)
		{
			candidates.erase(candidates.begin()+k);
			//node->id = c_down--;
		}

	}

	if (c_up != c_down+1) {
		unsigned int num_not_sorted = c_up-c_down-1;
		stringstream errormsg;
		errormsg << "Not all Nodes could be sorted! There are " << num_not_sorted << " nodes left " << ". c_up at " << c_up << " and c_down at " << c_down;
		error(errormsg.str());
	}

	assert(c_up == c_down+1);

	std::sort(this->nodes.begin(), this->nodes.end(), compare_nodes_by_id() );

	// determine new sizes and offsets
	//unsigned int idx = 0;


	in_size = in->size();
	out_size = out->size();
	hid_size = nodes.size() - bias_size - in_size - out_size;

	bias_offset = 0;
	in_offset = bias_size+bias_offset;
	hid_offset = in_offset+in_size;
	out_offset = hid_offset+hid_size;

	size = bias_size+in_size+hid_size+out_size;

}

/**
 * sort nodes in activation order. This should grant an automatically,
 * failure-safe ordering of the nodes
 *
 * - tag all output nodes
 *
 */
/*void Network::sort_nodes_deprecated(vector<Node*>* in, vector<Node*>* out)
{
	//int removed = remove_secluded_nodes();
	time_t start_sorting = time(NULL);
//	if (removed > 0)
//		warning("There were unconnected nodes that have been removed!");



	//for (//unsi)

	bool verbose = true;

	set<Node*>* tagged = new set<Node*>();
	int c = nodes.size()-1;
	int* id = &c;

	// untag all nodes
	if (verbose) cout << "untag all "<< nodes.size() << " nodes" <<endl;
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		nodes[i]->id = -1;
	}

	// tag all output nodes
	if (verbose) cout << "tag all " <<  out->size() << " output nodes" <<endl;
	for (unsigned int i=0; i < out->size(); i++)
	{
		tagged->insert( (*out)[i]);
		(*out)[i]->id = *id;
		if (verbose) cout << "ID: " << (*out)[i]->id << " for " << (*out)[i]->name << " ( output tag) " << endl;
		*id = *id-1;
	}

	// tag all input nodes
	for (unsigned int i=0; i < in->size(); i++)
	{
		tagged->insert( (*in)[i]);
		//cout << "input: " << (*in)[i]->name << endl;
	}



	// recurse from all nodes that do not have outgoing forward connections
	vector<Node*> start;
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if (dynamic_cast<BiasNode*>(nodes[i]) != 0) continue;
		bool match = true;
		for (unsigned int j = 0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			if (nodes[i]->outgoing_connections[j]->forward) {match=false; break; }
		}

		if (match) {
			start.push_back( nodes[i] );

			// tag them if not yet tagged (but they count as hiddens!)
			if (nodes[i]->id == -1) {
				tagged->insert( nodes[i]);
				nodes[i]->id = *id;
				if (verbose)	cout << "ID: " << (*id) << " for " << nodes[i]->name << " ( output tag, but hidden) " << endl;
				*id = *id-1;
			}

		}
	}

	if (verbose) cout << "start recursion from " << start.size() << " nodes " <<endl;

	for (unsigned int i=0; i < start.size(); i++)
	{
		if (verbose) cout << "recursion " << i << "/" << start.size() << endl;
 		_sort_nodes_rec( start[i], tagged, id, &start );
 		i++;
	}

	delete tagged;

	// give ids to nodes that have only incoming backward connections
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if (nodes[i]->id != -1)
				continue;
		if (dynamic_cast<BiasNode*>(nodes[i]) != 0)
			continue;
		if (nodes[i]->incoming_connections.size() > 0)
		{
			bool ok = true;
			for (unsigned int j=0; j < nodes[i]->incoming_connections.size();j++)
			{
				if (nodes[i]->incoming_connections[j]->forward) { ok=false; break;}
			}
			if (!ok) continue;

			nodes[i]->id = *id;
			*id = *id - 1;
		}
		cout << nodes[i]->name << " gets id " << *id << "( only incoming backward connections ) " << endl;
	}


	// give ids to input
	for (unsigned int i=0; i < in->size(); i++)
	{
		//tagged->insert( (*in)[i]);
		//cout << "input has id " << (*in)[i]->id << endl;
		(*in)[i]->id = *id;
		if (verbose) cout << "ID: " << (*in)[i]->id << " for " << (*in)[i]->name << " ( input tag) " << endl;
		*id = *id-1;
	}

	if (verbose) cout << "remaining IDs to Bias" << endl;

	// give all remaining ids to Bias unit(s)
	bias_size = 0;
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if (dynamic_cast<BiasNode*>(nodes[i]) != 0)  {
			bias_size++;
			(nodes)[i]->id=*id;
			*id = *id-1;
 		if (verbose)	cout << "Bias is : " << nodes[i]->get_label() << endl;
		}
	}

	//cout << "cur-ID: " << *id << " bias size: " << bias_size
	//		 << " Nodes: "<< nodes.size();

	// do the actual resort
	if (verbose) cout << "do resort" << endl;
	std::sort(this->nodes.begin(), this->nodes.end(), compare_nodes_by_id() );

	// determine new sizes and offsets
	//unsigned int idx = 0;


	in_size = in->size();
	out_size = out->size();
	hid_size = nodes.size() - bias_size - in_size - out_size;

	bias_offset = 0;
	in_offset = bias_size+bias_offset;
	hid_offset = in_offset+in_size;
	out_offset = hid_offset+hid_size;

	size = bias_size+in_size+hid_size+out_size;

	if (verbose)
	std::cout << "Bias: "<< bias_offset << "/" << bias_size
		<< " In: "<<in_offset << "/" << in_size
		<< " hid:"<< hid_offset << "/"<< hid_size
		<< " out:"<< out_offset << "/" << out_size << endl;



	//if (*id != -1){
	if (*id != -1) {
	stringstream sstr;
		sstr << "Error when sorting nodes in network! " << *id << "!=-1 could not be sorted! Make sure that you added all Nodes and Ensembles to the Network! Also, there might be a cycle in your forward-connections! ";
		error(sstr.str());
	}


	cout << "Sorting took " << (time(NULL)-start_sorting)/60.0 << "min" << endl;
}
*/

/**
 * removes all nodes that do not have incoming or outgoing connections
 */
int Network::remove_secluded_nodes()
{
	int counter=0;

	for (unsigned int i=0; i < nodes.size(); i++)
	{
		if ( (nodes[i]->incoming_connections.size()==0)
			&& (nodes[i]->outgoing_connections.size()==0) )
		{
			remove_node(nodes[i]);
			counter++;
		}
	}

	return counter;
}

void Network::save_binary(string filename)
{
	cout << "saving binary: " << filename << endl;
	fstream fileStream;
	fileStream.open(filename.c_str(), ios::out | ios::binary);

	 if(!fileStream) {
		 error("Cannot open filestream");
	 }

	 unsigned int size = nodes.size();
	 fileStream.write((char*) &size, sizeof(unsigned int));
 	 for (unsigned int i=0; i < nodes.size(); i++)
 	 {
 		Node* node = nodes[i];
 		double weight = node->weight;
		fileStream.write((char *) &weight, sizeof(double));

		unsigned int connections = nodes[i]->outgoing_connections.size();
		fileStream.write((char *) &connections, sizeof(unsigned int));
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* c = nodes[i]->outgoing_connections[j];
			fileStream.write((char*)&c->weight, sizeof(double));
		}
 	 }

	fileStream.close();
}

void Network::load_binary(string filename)
{
	fstream fileStream;
	fileStream.open(filename.c_str(), ios::in | ios::binary);

	cout.precision(20);

	unsigned int size;
	fileStream.read((char*)&size ,sizeof(unsigned int));
	for (unsigned int i=0; i < size; i++)
	{
		 double weight;
		 fileStream.read((char*)&weight, sizeof(double));
		 cout << "weight # " << i << ": " << nodes[i]->weight << " vs " << weight
				 << " diff=" << fabs(nodes[i]->weight-weight) << endl;
		 unsigned int num_connections;
		 fileStream.read((char*)&num_connections, sizeof(unsigned int));
		 for (unsigned int j=0; j < num_connections; j++)
		 {
			 double weight;
			 fileStream.read((char*)&weight, sizeof(double));
			 cout << "weight # " << i << ": " << nodes[i]->outgoing_connections[j]->weight << " vs " << weight
					 << " diff=" << fabs(nodes[i]->outgoing_connections[j]->weight-weight) << endl;

			 nodes[i]->outgoing_connections[j]->weight = weight;
		 }
	}
	fileStream.close();


}

/**
 * TODO: implement Ensembles, add toString(), fromString() methods to Nodes, Connections
 * 
 * 
 **/
void Network::save(string filename)
{

	

	 ofstream myfile;
 	 myfile.open ( filename.c_str() );

 	 streamsize precision = 15;

 	 myfile.precision(precision);

 	 myfile << "BNNlib\n";
  	 myfile << "neuralnetwork description\n";
  	 myfile << "version: " << __BNNLIB_VERSION << endl;
  	 myfile << "precision: " << precision << endl;
  	 myfile << "description: " << description << endl;
  	 myfile << "[parameters]\n";

  	 myfile << "in_size: " << in_size << endl;
  	 myfile << "hid_size: " << hid_size << endl;
  	 myfile << "out_size: " << out_size << endl;
  	 myfile << "bias_size: " << bias_size << endl;
   	 myfile << "in_offset: " << in_offset << endl;
  	 myfile << "hid_offset: " << hid_offset << endl;
  	 myfile << "out_offset: " << out_offset << endl;
  	 myfile << "bias_offset: " << bias_offset << endl;  
  	 myfile << "use_initial_state: " << use_initial_states << endl;
  	 myfile << "size: " << size << endl;
//  	 myfile << "washout-time: " << washout_time << endl;
 // 	 myfile << "seed: " << seed << endl;
  	 myfile << "[others]" << endl;

  	 time_t rawtime;
   	 time ( &rawtime ); 	 
  	 myfile << "time: " << ctime(&rawtime);
  	 myfile << "[nodes]" << endl;
  	 for (unsigned int i=0; i < nodes.size(); i++)
  	 {
  	 	myfile << nodes[i]->id << "\t" << nodes[i]->name << "\t" << nodes[i]->get_type() << "\t"<< nodes[i]->weight << "\t"<<nodes[i]->functional_type <<"\t"<< nodes[i]->to_string() << "\n";
  	 }
	 myfile << "[connections]" << endl;
  	 for (unsigned int i=0; i < nodes.size(); i++)
  	 {
  	 	for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
  	 	{
  	 		myfile << nodes[i]->id << "\t" << nodes[i]->outgoing_connections[j]->to->id << "\t"
  	 			<< nodes[i]->outgoing_connections[j]->weight;
  	 			//<< "\t";
  	 		//myfile.write(reinterpret_cast<char*>(&nodes[i]->outgoing_connections[j]->weight), sizeof(&nodes[i]->outgoing_connections[j]->weight));
  	 		myfile << "\t" << nodes[i]->outgoing_connections[j]->forward << "\t" << nodes[i]->outgoing_connections[j]->freeze_weight
  	 			<< "\t" << nodes[i]->outgoing_connections[j]->gamma	<< endl;
  	 	}
  	 }  	 
	myfile << "[ensembles]" << endl;
	for (unsigned int i=0; i < ensembles.size(); i++)
	{

		myfile << ensembles[i]->get_type() <<"\t" << ensembles[i]->to_string();
		for (unsigned int j=0; j < ensembles[i]->nodes.size(); j++)
		{
			myfile << "\t" << ensembles[i]->nodes[j]->id;
		}
	
		myfile <<  endl;
	}
	
	myfile << "[ensemble-meta]" << endl;
	//vector<Ensemble*>::iterator p;
	if (input_ensemble != NULL) {
		unsigned int distance = find( ensembles.begin(), ensembles.end(), input_ensemble ) - ensembles.begin();
		myfile << "input_ensemble: "<< distance << endl;
	}
	if (output_ensemble != NULL) {
		unsigned int distance = find( ensembles.begin(), ensembles.end(), output_ensemble ) - ensembles.begin();
		myfile << "output_ensemble: "<< distance << endl;
	}
	//	myfile << "output_ensemble: " << ensembles.at( output_ensemble ) << endl;
	
  	 myfile.close();	
  	 
  	 
//	filename.append(".bin");
//	save_binary( filename );

}


// sensitiv
/**
 * be aware that this algorithm is extremly slow! Complexity is O(N*N*T*T) for N = number of nodes, T = time
 *
 */
Jacobian* Network::jacobian_numerical(Sequence* seq, double epsilon)
{
	Jacobian* jacobian = new Jacobian(in_size, out_size, seq->size());

	//epsilon = 1;

	unsigned int t = seq->size();
//	double J[out_size*t][in_size*t];

	for (unsigned int t1 = 0; t1 < t; t1++) {
	for (unsigned int i=0; i < out_size; i++)
		for (unsigned int t2=0; t2 < t; t2++)
		for (unsigned int j=0; j < in_size; j++)
		{
			weight_t old_value = (*seq->input[t2])[j];
			(*seq->input[t2])[j] = old_value+epsilon;
			activate(seq);
			double eps_plus = (*get_output(t1))[i];
			(*seq->input[t2])[j] = old_value-epsilon;
			activate(seq);
			double eps_minus = (*get_output(t1))[i];
			(*seq->input[t2])[j] = old_value;
			//cout << eps_plus << " " << eps_minus << endl;
			(*jacobian->data)[out_size*t1+i][in_size*t2+j] = (eps_plus-eps_minus)/(2*epsilon);
			//cout << ".";
			//cout.flush();
		}
		cout << "." << endl;
	}



	return jacobian;

}

void Network::hessian()
{

	// find h_kj by forward propagation
	double h[size][size];
	for (unsigned int i=0; i < size; i++)
		for (unsigned int j=0; j < size; j++)
			h[i][j] = 0;
	for (unsigned int i=0; i < size; i++)
			h[i][i] = 1;



	//TODO ....


}

#endif
