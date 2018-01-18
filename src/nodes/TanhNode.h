#ifndef TANHNODE_H_
#define TANHNODE_H_

#include <cmath>
#include "../functions.h"
#include "Node.h"
#include <iostream>

#undef APPROXIMATE_TANH

struct TanhNode : Node
{
	
	TanhNode();
	
	const nodetype get_type() { return Node::TANH_NODE; }
	
	void activate(unsigned int timestep) {
		//cout << "ACTIVATE!" << endl;
		// actbuf[timestep] = activation_function(inbuf[timestep]);
//#ifndef APPROXIMATE_TANH
		actbuf[timestep] =  tanh(inbuf[timestep]);
//#else
/*		if (inbuf[timestep] == 0) actbuf[timestep]=0;
		else
		actbuf[timestep] =  2.0 / (1.0 + exp(-2.0 *inbuf[timestep] )) - 1.0;
		*/
/*		double x = inbuf[timestep];
	      if (x > 1.92033) actbuf[timestep] =  0.96016;
	      else if (x < -1.92033) actbuf[timestep] =  -0.96016;
	      else if (0 < x) actbuf[timestep] = 0.96016 - 0.26037 * (x - 1.92033)*(x - 1.92033);
	      else actbuf[timestep] = 0.26037 * (x + 1.92033)*(x - 1.92033) - 0.96016;

#endif*/
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return 1.0 - this->actbuf[time]*this->actbuf[time] ;
	}
};


#endif /*TANHNODE_H_*/
