#include "ReLUNode.h"
#include <sstream>
#include <string>
#include "../functions.h"



ReLUNode::ReLUNode() {
	
		std::stringstream namestream;
		namestream << "ReLU" << id;
		name = namestream.str();

		activation_function = &linear;
		derivative_activation_function = &dlinear;
		

}

void ReLUNode::activate(unsigned int time) {
	if (this->inbuf[time]>0)
		this->actbuf[time] = this->inbuf[time] ;
	else
		this->actbuf[time] = 0 ;
}

weight_t  ReLUNode::get_activation_function_derivative(unsigned int time)
{
	if (this->inbuf[time]>0)
		return 1.0;
	else
		return 0.0;
}
