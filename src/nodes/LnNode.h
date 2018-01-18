#ifndef LNNODE_H_
#define LNNODE_H_

#include "Node.h"

struct LnNode : Node
{
		
	LnNode();
	~LnNode() {};
	
	const int get_type() { return Node::LN_NODE; }
	
};

#endif /*LNNODE_H_*/
