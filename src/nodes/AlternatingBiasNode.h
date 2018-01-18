/*
 * AlternatingBiasNode.h
 *
 *  Created on: Nov 5, 2009
 *      Author: brandmaier
 */

#ifndef ALTERNATINGBIASNODE_H_
#define ALTERNATINGBIASNODE_H_

#include "BiasNode.h"

struct AlternatingBiasNode : BiasNode
{
	unsigned int t;
	weight_t v1;
	weight_t v2;

	AlternatingBiasNode(unsigned int t, weight_t v1, weight_t v2) {
		this->t = t;
	}

	void activate(unsigned int timestep) {
		this->actbuf[timestep] = (timestep%t==0)?v1:v2;
	}

	virtual string to_string()
	{
		stringstream ostr;
		ostr << t << ";" << v1 << ";" << v2;
		return ostr.str();
	}

	static AlternatingBiasNode* from_string(string str)
	{
     	 vector<string> tokens;
      	 string sep = std::string(":");
         split(str,sep,tokens);

		return new AlternatingBiasNode( atoi(tokens[0].c_str()),
				atof(tokens[1].c_str()), atof(tokens[2].c_str()) );
	}

	const nodetype get_type() { return Node::ALTERNATING_BIAS_NODE; }
};

#endif /* ALTERNATINGBIASNODE_H_ */
