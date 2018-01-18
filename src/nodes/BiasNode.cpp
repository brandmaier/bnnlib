#include "BiasNode.h"
#include "../functions.h"

BiasNode::BiasNode() {
		name = "Bias  ";
		activation_function = &linear;
		derivative_activation_function = &dlinear;
};

void BiasNode::activate(unsigned int timestep)
{
		//std::cout << "actbuf bias : size=" << this->actbuf.size() << "\n";
		this->actbuf[timestep] = 1.0;	
}

void  BiasNode::add_input(weight_t input, weight_t weight, unsigned int time)
{
	error("BiasNode cannot receive inputs from other Nodes!");
}
