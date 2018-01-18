#ifndef NOISENODE_H_
#define NOISENODE_H_

#include <iostream>
#include <stdlib>

struct UniformNoiseNode : BiasNode
{

	double scaling_factor;

	UniformNoiseNode(double scaling_factor) {
		this->scaling_factor = scaling_factor;
	};

	//TODO unfinished
	void activate(unsigned int timestep)
	{
		this->actbuf[timestep] = rand()/((double)RAND_MAX+(double)1.0) * scaling_factor;
	}
	
	const int get_type() { return Node::UNIFORM_NOISE_NODE; }

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return 1.0;
	}

};

#endif /*NOISENODE_H_*/
