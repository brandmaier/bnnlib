/*
 *  Connection.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 19.08.09.
 *  Copyright 2009 Andreas Brandmaier. All rights reserved.
 *
 */

#ifndef __CONNECTION_H_
#define __CONNECTION_H_

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
	
//	void init() {};
	
	void init(Node* node1, Node* node2, bool forward);
	
	void remove();
	
	virtual ~Connection() {};
	
	void set_identity_and_freeze();
	
};

#endif
