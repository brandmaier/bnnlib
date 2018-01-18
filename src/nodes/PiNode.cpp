
#include "PiNode.h"
#include <iostream>
#include <cassert>
#include "../functions.h"
#include <sstream>
#include <stdlib.h>



PiNode::PiNode()
{
	activation_function = &linear;
	derivative_activation_function = &dlinear;	
	std::stringstream namestream;
	namestream << "Pi-" << id;
	name = namestream.str();
}

void PiNode::add_input(weight_t input, weight_t weight, unsigned int time)
{
	this->inbuf[time] *= weight*input;
}

//TODO unterscheide zwischen backward und forward connection !! BUG BUG BUG
weight_t PiNode::get_input_to_weight_derivative(Connection* connection, unsigned int time)
{
	//if (connection->weight==0) { return 0; }

	assert( abs(connection->weight) > 10e-9 );

	if (connection->forward)
	 return connection->to->inbuf[time]/(connection->weight/**connection->from->actbuf[time]*/);
	else
	 return connection->to->inbuf[time+1]/(connection->weight/**connection->from->actbuf[time]*/);

}

weight_t PiNode::get_input_to_activation_derivative(Connection* connection, unsigned int time)
{


	//cout << abs(connection->from->actbuf[time]) <<endl;
	//assert( abs(connection->from->actbuf[time]) > 10e-9 );



	
	if (connection->forward) {
		if (    abs(connection->from->actbuf[time]) < 10e-9 )
		{
			stringstream cstr;
			cstr << "Connection from " << connection->from->name << " to " << connection->to->name << " has d_inp/d_act = 0 when calculating deltas!" << endl;
			//warning(cstr.str());
			return 0.0; //return 0.0; //TODO BUG UNSECURE
		}
		return inbuf[time]/connection->from->actbuf[time];
	}
	else {

	 if (time > 0) {
		//cout << "\033[35;1mxxx\033[0m" <<endl;
			if (    abs(connection->from->actbuf[time-1]) < 10e-9 )
			{
				stringstream cstr;
				cstr << "Connection from " << connection->from->name << " to " << connection->to->name << " has d_inp/d_act = 0 when calculating deltas!" << endl;
				//warning(cstr.str());
				return 0.0; //return 0.0; //TODO BUG UNSECURE
			}

		 return inbuf[time]/connection->from->actbuf[time-1];
	 }
	 else
	 {
		error("not implemented code in PiNode.cpp");
		return 0.0;	//TODO
	 }
	}
}

void PiNode::grow_buffer(unsigned int size)
{
	for (unsigned int i=0; i < size; i++){
	 this->inbuf.push_back(1.);
	 this->actbuf.push_back(0.);
	 this->delta.push_back(0.);
	}
}

void PiNode::reset_buffer(unsigned int t)
{
		this->inbuf[t] = 1.;
		this->actbuf[t] = 0.;
		this->delta[t] = 0.;
}

void PiNode::activate(unsigned int timestep) {
	this->actbuf[timestep] = this->inbuf[timestep] ;
}
