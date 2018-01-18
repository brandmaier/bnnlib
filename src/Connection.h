/*
 *  Connection.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 19.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __CONNECTION_H_
#define __CONNECTION_H_

//#include "nodes/Node.h"
#include "functions.h"
#include "Trainable.h"

struct Node;

struct Connection : Trainable {

	Node* from;
	Node* to;

	int id;

	bool forward;

	
	Connection(Node* node1, Node* node2);
	Connection(Node* node1, Node* node2, bool forward);
	
	void init(Node* node1, Node* node2, bool forward);
	
	void remove();
	
	virtual ~Connection() {};
	
	void set_identity_and_freeze();
	
	/*weight_t weight;
	weight_t derivative;
	weight_t previous_weight_change;
	weight_t previous_derivative;
	
	weight_t gamma; //rprop
	*/
};

#endif
