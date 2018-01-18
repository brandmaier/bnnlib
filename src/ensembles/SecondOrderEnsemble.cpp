/*
 * SecondOrderEnsemble.cpp
 *
 *  Created on: Dec 7, 2009
 *      Author: brandmaier
 */

#include "SecondOrderEnsemble.h"

SecondOrderEnsemble::SecondOrderEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "SecondOrderEnsemble";
}

SecondOrderEnsemble::SecondOrderEnsemble(vector<Node*>* inputs, vector<Node*>* outputs)
{
	name = "SecondOrderEnsemble";
	init(inputs, outputs, false);
}

SecondOrderEnsemble::SecondOrderEnsemble(vector<Node*>* inputs, vector<Node*>* outputs,	bool include_additional_quadratic_terms)
{
	name = "SecondOrderEnsemble";
/*	for (unsigned int i=0; i < inputs->size(); i++)
	{
		this->inputs.push_back( (*inputs)[i] );
	}
*/
	init(inputs, outputs,include_additional_quadratic_terms);
}

void SecondOrderEnsemble::init(vector<Node*>* inputs, vector<Node*>* outputs,	bool include_additional_quadratic_terms)
{
	unsigned int num_outputs = outputs->size();
	unsigned int num_inputs = inputs->size();


	// create first-order connections
	for (unsigned int i=0; i < num_inputs;i++)
	{
		for (unsigned int j=0; j < num_outputs; j++) {
			Network::connect( (*inputs)[i], (*outputs)[j], true);
		}
	}


	// create intermediate layer
	//std::vector<Node*> intermediate;
	for (unsigned int i=0; i < num_inputs; i++)
	{
		for (unsigned int j=i+(include_additional_quadratic_terms?0:1); j < num_inputs; j++)
		{
			Node* node = new PiNode();
			nodes.push_back(node);
			//intermediate.push_back( node );

			Connection* c1 = Network::connect( (*inputs)[i], node, true);
			Connection* c2 = Network::connect( (*inputs)[j], node, true);

			c1->set_identity_and_freeze();
			c2->set_identity_and_freeze();

			for (unsigned int k=0; k < num_outputs; k++)
			{
				Network::connect(node, (*outputs)[k], true);
			}
		}
	}

}

/*
std::vector<Node*>* SecondOrderEnsemble::get_inputs() {
	return &this->inputs;
}

std::vector<Node*>* SecondOrderEnsemble::get_outputs() {
	return &this->outputs;
}
*/

string SecondOrderEnsemble::to_string()
{
	//stringstream stream;
	//stream << node_type << ";" << nodes.size();
	return "<NONE>";//stream.str();
}


SecondOrderEnsemble* SecondOrderEnsemble::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

	SecondOrderEnsemble* ensemble = new SecondOrderEnsemble(nodes);
	return ensemble;
}

