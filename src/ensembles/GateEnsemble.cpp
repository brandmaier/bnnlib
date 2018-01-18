/*
 * GateEnsemble.cpp
 *
 *  Created on: Nov 12, 2009
 *      Author: brandmaier
 */

#include "GateEnsemble.h"
#include "../nodes/BiasNode.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/LinearNode.h"
#include "../nodes/PiNode.h"
#include "../Connection.h"
#include "../Network.h"

GateEnsemble::GateEnsemble()
{
	init(Node::TANH_NODE);
	name = "GateEnsemble";
}

GateEnsemble::GateEnsemble(int node_type)
{
	init(node_type);
	name = "GateEnsemble";
}

GateEnsemble::GateEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "GateEnsemble";
}

GateEnsemble::~GateEnsemble()
{

}

Node* GateEnsemble::get_gate() {
	return nodes[2];
}
Node* GateEnsemble::get_signal()
{
	return nodes[0];
}

void GateEnsemble::init(int node_type)
{


	Node* input;
	if (node_type == Node::TANH_NODE) {
		input = new TanhNode();
	} else if (node_type == Node::LINEAR_NODE) {
		input = new LinearNode();
	} else {
		error("Nodetype not supported in GateEnsemble");
	}
	Node* mult = new PiNode();
	Node* gate = new SigmoidNode();
	nodes.push_back(input);
	nodes.push_back(mult);
	nodes.push_back(gate);

	input->name = "Signal";
	gate->name = "Gate";

	Connection* gb1 = Network::connect(input, mult, true);
	Connection* gb2 = Network::connect(gate, mult, true);

	gb1->set_identity_and_freeze();
	gb2->set_identity_and_freeze();


	inputs.push_back(gate);
	inputs.push_back(input);
	outputs.push_back(mult);

}

std::vector<Node*>* GateEnsemble::get_inputs() {
		return &this->inputs;
}

std::vector<Node*>* GateEnsemble::get_outputs() {
		return &this->outputs;
}


GateEnsemble* GateEnsemble::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

   // assert( (*nodes)[0]->get_type() == Node::TANH_NODE );
    assert( (*nodes)[1]->get_type() == Node::PI_NODE );
    assert( (*nodes)[2]->get_type() == Node::SIGMOID_NODE );

	/*for (unsigned int i=0; i < nodes->size(); i++)
	{
		if ((*nodes)[i]->get_type() != type) {
		 cout << type << "/" << (*nodes)[i]->get_type() << endl;
		 error("Cannot instantiate FeedforwardEnsemble from string! Node type mismatch! ");
		 exit(0);
		}
	}
	*/
	/*if (size != nodes->size())
	{
		 error("Cannot instantiate FeedforwardEnsemble from string! Wrong node size! ");
		exit(0);
	}*/

	GateEnsemble* ensemble = new GateEnsemble(nodes);
	return ensemble;
}


string GateEnsemble::to_string()
{
	return "<NONE>";
}
