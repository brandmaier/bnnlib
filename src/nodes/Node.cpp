/*
 *  Node.cpp
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 19.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Node.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <sstream>
#include "../functions.h"


int STATIC_ID_COUNTER = 0;


Node::Node() : Trainable() {
	//activation_function = (&tanh_efficient);
	//derivative_activation_function = (&dtanh);
	id = STATIC_ID_COUNTER++;
	//std::cout << "id-counter:" << STATIC_ID_COUNTER << "\n";
//	std::stringstream namestream;
//	namestream << "Tan" << id;
	name = "unnamed";//namestream.str();
	
	sparsity_prior = 0.0;

	//initial_state = 0; //rand_uniform_01()*2-1;
	//last_initial_state_change = 0;
	//previous_state_derivative = 0;
}

void Node::activate(unsigned int time) {
	//assert(this->actbuf.size() == this->inbuf.size());
	this->actbuf[time] = activation_function(this->inbuf[time]) ;
}

void Node::reserve(unsigned int timesteps)
{
	this->inbuf.reserve( timesteps );
	this->actbuf.reserve( timesteps );
	this->delta.reserve( timesteps );
}

void Node::grow_buffer(unsigned int size)
{
	for (unsigned int i=0; i < size; i++) {
		this->inbuf.push_back(0.);
		this->actbuf.push_back(0.);
		this->delta.push_back(0.);
	}
}

/***
 * resets the input buffer, the activation buffer and the delta buffer (for backprop)
 * of a node at time point t to zero.
 */
void Node::reset_buffer(unsigned int t)
{
	bool warn = false;

	if (this->inbuf.size() <= t) {
		warn = true;
		//cout << this->inbuf.size() << " vs. " << t << endl;
	//	warning("When resetting node, input buffer size was reset with a larger value than its buffer length!");
	} else {
		this->inbuf[t] = 0.;
	}

	if (this->actbuf.size() <= t) {
		warn=true;
		//cout << this->inbuf.size() << this->name << endl;
	//	warning("When resetting node, activation buffer size was reset with a larger value than its buffer length!");

	} else {
		this->actbuf[t] = 0.;
	}

	if (this->delta.size() <= t) {
		warn=true;
	//	warning("When resetting node, delta buffer size was reset with a larger value than its buffer length!");
		//cout << this->inbuf.size() << this->name << endl;
	} else {
		this->delta[t] = 0.;
	}


	if (warn) warning("When resetting buffer of a node, there was a problem with the buffer size (smaller than expected!)");


}

weight_t  Node::get_activation_function_derivative(unsigned int time)
{
	return derivative_activation_function( this->actbuf[time] );
}

weight_t  Node::get_input_to_activation_derivative(Connection* connection, unsigned int time) {
	return connection->weight;
}

void  Node::add_input(weight_t input, weight_t weight, unsigned int time)
{
	this->inbuf[time] += weight*input;
}

weight_t Node::get_input_to_weight_derivative(Connection* connection, unsigned int time)
{
	return connection->from->actbuf[time];
}

Node::~Node()
{
	cut_all_links();
}

string Node::to_string()
{
	return "";
}



void Node::cut_all_links()
{
	for (unsigned int i=0; i < incoming_connections.size(); i++)
	{
		for (unsigned int j=0; j < incoming_connections[i]->from->outgoing_connections.size(); j++)
		{
			if (incoming_connections[i]->from->outgoing_connections[j] == incoming_connections[i]) {
				incoming_connections[i]->from->outgoing_connections.erase(
						incoming_connections[i]->from->outgoing_connections.begin()+j
				);

			}
		}
	}

	
	for (unsigned int i=0; i < outgoing_connections.size(); i++)
	{
		for (unsigned int j=0; j < outgoing_connections[i]->to->incoming_connections.size(); j++)
		{
			if (outgoing_connections[i]->to->incoming_connections[j] == outgoing_connections[i]) {
				//cout << i << "->" << j << endl;
				outgoing_connections[i]->to->incoming_connections.erase(
						outgoing_connections[i]->to->incoming_connections.begin()+j
				);

			}
		}
	}

	for (unsigned int i=0; i <incoming_connections.size(); i++)
	{
		delete incoming_connections[i];
	}
	for (unsigned int i=0; i <outgoing_connections.size(); i++)
	{
		delete outgoing_connections[i];
	}
	outgoing_connections.clear();
	incoming_connections.clear();
	// todo: missing is the deletion of the actual Connections

}
