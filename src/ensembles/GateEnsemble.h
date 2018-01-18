/*
 * GateEnsemble.h
 *
 *  Created on: Nov 12, 2009
 *      Author: brandmaier
 */

#ifndef GATEENSEMBLE_H_
#define GATEENSEMBLE_H_

#include "Ensemble.h"

struct GateEnsemble : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;

	GateEnsemble();
	GateEnsemble(int node_type);
	GateEnsemble(vector<Node*>* nodes);
	~GateEnsemble();

	void init(int node_type);
	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();

	Node* get_gate();
	Node* get_signal();

	void connect_incoming_ensembles(vector<Node*>* signals, vector<Node*>* controllers);

	virtual const unsigned int get_type() { return GATE_ENSEMBLE; }
	string to_string();
	static GateEnsemble* from_string(string from_string);
	static GateEnsemble* from_string(string from_string, vector<Node*>* nodes);

};

#endif /* GATEENSEMBLE_H_ */
