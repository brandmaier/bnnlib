#include "LinearNode.h"
#include <sstream>
#include <string>
#include "../functions.h"



LinearNode::LinearNode() {
	
		std::stringstream namestream;
		namestream << "Lin" << id;
		name = namestream.str();

		activation_function = &linear;
		derivative_activation_function = &dlinear;
		

}

void LinearNode::activate(unsigned int time) {
	this->actbuf[time] = this->inbuf[time] ;
}

weight_t  LinearNode::get_activation_function_derivative(unsigned int time)
{
	return 1.0;
}
