#ifndef TIMEDSINUNIT_H_
#define TIMEDSINUNIT_H_

#include "Node.h"
#include <cmath>

//TODO efficient
/*float my_sin(float x) { return sin( 3.14159265f*x);}
float my_cos(float x) { return cos( 3.14159265f*x);}
double my_sin(double x) { return sin(M_PI*x);}
double my_cos(double x) { return cos(M_PI*x);}
*/

struct TimedSinNode : Node
{

	//Trainable freq;

	TimedSinNode()
	{
		name = "TSin";
	}
	
	void activate(unsigned int time) {
		this->actbuf[time] = sin(time*this->inbuf[time]) ;
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return cos(time*this->inbuf[time] ) * time;
	}

	const nodetype get_type() { return Node::TSIN_NODE; }
};

#endif /*SINUNIT_H_*/
