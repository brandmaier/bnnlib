/*
 *  Connection.cpp
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 19.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Connection.h"
#include "nodes/Node.h"
#include <cmath>
#include "stdlib.h"
#include <vector>

int STATIC_CONNECTION_ID_COUNTER = 1000000;


Connection::Connection(Node* node1, Node* node2) : Trainable() {
	this->init(node1, node2, true);
}

Connection::Connection(Node* node1, Node* node2, bool forward) : Trainable() {
	this->init(node1, node2, forward);
}

void Connection::init(Node* node1, Node* node2, bool forward)
{
  //this->init();
	this->from = node1;
	this->to = node2;
	//this->weight =  rand() / (RAND_MAX + 1.) * 0.4 - 0.2;
	this->id = STATIC_CONNECTION_ID_COUNTER++;
	this->forward = forward;

}




// TODO unfinished
void Connection::remove()
{
	std::vector<Connection*>::iterator out = this->from->outgoing_connections.begin();

	for (unsigned int i=0; i < this->from->outgoing_connections.size(); i++)
	{
		if (this->from->outgoing_connections[i] == this) {
			this->from->outgoing_connections.erase(out);
			break;	
		}	
	}
	
	exit(-1);
}

void Connection::set_identity_and_freeze()
{
	freeze_weight = true;
	weight = 1.0;	
}
