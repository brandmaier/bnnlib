/*
 * ScaledScaledSigmoidNode.h
 *
 *  Created on: Nov 4, 2009
 *      Author: brandmaier
 */

#ifndef ScaledSigmoidNode_H_
#define ScaledSigmoidNode_H_


#include "Node.h"
#include <sstream>

struct ScaledSigmoidNode : Node
{
	double f;

	ScaledSigmoidNode(double f)
	{
		std::stringstream namestream;
		namestream << "ScaledSig" << id;
		name = namestream.str();

		this->f = f;
	}

	const nodetype get_type() { return Node::SCALED_SIGMOID_NODE; }

	void activate(unsigned int timestep) {

		actbuf[timestep] = f/(1.0+safeexp(-inbuf[timestep]));
	//	cout << actbuf[timestep] << endl;
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return (f*this->actbuf[time] * (1.0-this->actbuf[time]));
	}

	virtual string to_string()
	{
		stringstream ostr;
		ostr << f;
		return ostr.str();
	}

	static ScaledSigmoidNode* from_string(string str)
	{
		return new ScaledSigmoidNode( atof(str.c_str()) );
	}

};



#endif /* SCALEDSigmoidNode_H_ */
