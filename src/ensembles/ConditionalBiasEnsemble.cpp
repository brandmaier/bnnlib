/*
 * ConditionalBiasEnsemble.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: brandmaier
 */

#include "ConditionalBiasEnsemble.h"
#include "../nodes/ConditionalBiasNode.h"

ConditionalBiasEnsemble::ConditionalBiasEnsemble()
{
	bias0 = new ConditionalBiasNode(ConditionalBiasNode::ZERO);
	biasx = new ConditionalBiasNode(ConditionalBiasNode::X);

	this->nodes.push_back(bias0);
	this->nodes.push_back(biasx);
}

ConditionalBiasEnsemble::ConditionalBiasEnsemble(vector<Node*>* nodes)
{
	//assert(len(nodes) == 2);
	this->nodes.push_back((*nodes)[0]);
	this->nodes.push_back((*nodes)[1]);
}

ConditionalBiasEnsemble::~ConditionalBiasEnsemble()
{
	delete bias0;
	delete biasx;
}

string ConditionalBiasEnsemble::to_string()
{
	return "";
}
ConditionalBiasEnsemble* ConditionalBiasEnsemble::from_string(string from_string)
{
	ConditionalBiasEnsemble* temp = new ConditionalBiasEnsemble();

	return temp;
}

ConditionalBiasEnsemble* ConditionalBiasEnsemble::from_string(string from_string, vector<Node*>* nodes)
{
	ConditionalBiasEnsemble* temp = new ConditionalBiasEnsemble(nodes);

	return temp;
}

