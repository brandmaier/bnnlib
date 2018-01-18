/*
 * CorrelationDerivatives.h
 *
 *  Created on: Dec 23, 2009
 *      Author: brandmaier
 */

#ifndef CORRELATIONDERIVATIVES_H_
#define CORRELATIONDERIVATIVES_H_

#include "../Network.h"
#include <vector>
#include "../nodes/Node.h"

/*
struct NodePair
{
	NodePair(Node* node1, Node* node2)
	{
		this->node1 = node1;
		this->node2 = node2;
	}

	Node* node1;
	Node* node2;
};
*/
class CorrelationDerivatives {
public:
	Network* network;
	vector<Node*>* nodes;

	CorrelationDerivatives(Network* network, vector<Node*>* nodes);
	virtual ~CorrelationDerivatives();

	void calculate_derivatives(double alpha);

	void correlation_error();
};

#endif /* CORRELATIONDERIVATIVES_H_ */
