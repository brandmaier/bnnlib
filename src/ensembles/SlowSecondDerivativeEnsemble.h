/*
 * SlowSecondDerivativeEnsemble.h
 *
 *  Created on: Jan 31, 2010
 *      Author: brandmaier
 */

#ifndef SLOWSECONDDERIVATIVEENSEMBLE_H_
#define SLOWSECONDDERIVATIVEENSEMBLE_H_

#include "RecurrentEnsemble.h"
#include "../nodes/PseudoOutputNode.h"
#include "../nodes/LinearNode.h"

/**
 *
 * dysfunctional sketch
 */
struct SlowSecondDerivativeNetwork : RecurrentEnsemble
{

	SlowSecondDerivativeNetwork(int node_type, unsigned int size) : RecurrentEnsemble(node_type, size)
	{
		init();
	}

	SlowSecondDerivativeNetwork(vector<Node*>* nodes) : RecurrentEnsemble(nodes)
	{

	}

	void init()
	{
		PseudoOutputNode* pseudo = new PseudoOutputNode();


		for (unsigned int i=0; i < nodes.size(); i++)
		{
			Node* back1 = new LinearNode();
			Node* back2 = new LinearNode();

			Connection* cback1 = Network::connect( nodes[i], back1, false);
			cback1->set_identity_and_freeze();
			Connection* cback2 = Network::connect( back1, back2, false);
			cback2->set_identity_and_freeze();

		}

		nodes.push_back( pseudo );
	}

};

#endif /* SLOWSECONDDERIVATIVEENSEMBLE_H_ */
