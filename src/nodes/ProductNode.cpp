
#include "ProductNode.h"
#include <iostream>
#include <cassert>
#include "../functions.h"
#include <cmath>

ProductNode::ProductNode()
{
		activation_function = &linear;
		derivative_activation_function = &dlinear;	
		name = "ProductNode";
}

void ProductNode::add_input(weight_t input, weight_t weight, unsigned int time)
{
//	std::cout << "before: " << this->inbuf[time]<< " pow="<< pow(input,weight) <<endl;
	assert(input >= 0);	//TODO remove
	this->inbuf[time] *= pow(input,weight);
//	std::cout << "add to input w=" << weight << ",inp=" << input << " buf="<< this->inbuf[time] << endl;

}

//TODO unterscheide zwischen backward und forward connection !! BUG BUG BUG
weight_t ProductNode::get_input_to_weight_derivative(Connection* connection, unsigned int time)
{
	if (connection->forward)
	 return  inbuf[time]*log( connection->from->actbuf[time] );
	else
	{
	if (time > 0)
	 return inbuf[time]*log( connection->from->actbuf[time-1] );
	else
	 return 0;
	//TODO BUG unsure about this
	 error("not yet implemented fully! ProductNode::get_input_to_weight_derivative()");
	}
}

weight_t ProductNode::get_input_to_activation_derivative(Connection* connection, unsigned int time)
{
	assert( connection->from->actbuf[time] > 0);
	if (connection->forward)
	 return inbuf[time]*(connection->weight / connection->from->actbuf[time]);
	else
	 return inbuf[time+1]*(connection->weight / connection->from->actbuf[time]);
	
}

void ProductNode::grow_buffer(unsigned int size)
{
	this->inbuf.push_back(1.);
	this->actbuf.push_back(0.);
	this->delta.push_back(0.);
}

void ProductNode::reset_buffer(unsigned int t)
{
	this->inbuf[t] = 1.;
	this->actbuf[t] = 0.;
	this->delta[t] = 0.;
}

void ProductNode::activate(unsigned int timestep) {

	this->actbuf[timestep] = this->inbuf[timestep] ;
}
