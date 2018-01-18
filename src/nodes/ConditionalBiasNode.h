/*
 * ConditionalBias.h
 *
 *  Created on: Oct 26, 2009
 *      Author: brandmaier
 */

#ifndef CONDITIONALBIASNODE_H_
#define CONDITIONALBIASNODE_H_

#include "BiasNode.h"

struct ConditionalBiasNode : BiasNode
{
	static const bool ZERO = true;
	static const bool X = false;

	ConditionalBiasNode(bool type);

	bool type;

	void activate(unsigned int timestep);

	const int get_type() { return Node::CONDITIONAL_BIAS_NODE; }

	static ConditionalBiasNode* from_string(string str);

	string to_string();
};

#endif /* CONDITIONALBIAS_H_ */
