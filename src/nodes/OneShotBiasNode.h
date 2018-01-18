/*
 * OneShotBiasNode.h
 *
 *  Created on: Jan 15, 2010
 *      Author: brandmaier
 */

#ifndef ONESHOTBIASNODE_H_
#define ONESHOTBIASNODE_H_

struct OneShotBiasNode : BiasNode
{
	OneShotBiasNode() : BiasNode()
		{
			name = "OneshotBias";
		}

	void activate(unsigned int timestep) {
		this->actbuf[timestep] = (timestep==0)?1:0;
	}

	virtual string to_string()
	{
		return "NONE";
	}

	static OneShotBiasNode* from_string(string str)
	{
		return new OneShotBiasNode();
	}

	const nodetype get_type() { return Node::ONE_SHOT_BIAS_NODE; }

};

#endif /* ONESHOTBIASNODE_H_ */
