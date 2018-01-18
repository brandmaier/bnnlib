#include "SigmoidNode.h"
#include <iostream>

	
	SigmoidNode::SigmoidNode()
	{
		std::stringstream namestream;
		namestream << "Sig" << id;
		name = namestream.str();
	
		activation_function = &sigmoid;
		derivative_activation_function = &dsigmoid;	
		
		//std::cout << "created sigmoid " << endl;
	}