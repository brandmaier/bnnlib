#ifndef NOISENODE_H_
#define NOISENODE_H_

#include <iostream>
#include <stdlib.h>

struct GaussianNoiseNode : BiasNode
{

	double mu, sigma;

	GaussianNoiseNode()
	{
		this->name = "GaussianNoise";
		this->mu = 0;
		this->sigma = 1;
	}

	GaussianNoiseNode(double mu, double sigma) {
		this->name = "GaussianNoise";
		this->mu = mu;
		this->sigma = sigma;
	};

	void activate(unsigned int timestep)
	{
		this->actbuf[timestep] = rand_gauss(mu, sigma);
	}
	
	const int get_type() { return Node::GAUSSIAN_NOISE_NODE; }

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return 1.0;
	}

	virtual string to_string()
	{
		stringstream ostr;
		ostr << mu << ";" << sigma;
		return ostr.str();
	}

	static GaussianNoiseNode* from_string(string parameters)
	{
		vector<string> tokens;
	    string sep = std::string(";");
	    split(parameters,sep,tokens);

		return new GaussianNoiseNode( atof(tokens[0].c_str()),atof(tokens[1].c_str())  );
	}


};

#endif /*NOISENODE_H_*/
