/*
 * DelayEnsemble.cpp
 *
 *  Created on: Jan 18, 2010
 *      Author: brandmaier
 */

#include "DelayEnsemble.h"
#include "../nodes/LinearNode.h"
#include "../Network.h"


DelayEnsemble::DelayEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "DelayEnsemble";
}

DelayEnsemble::DelayEnsemble( Ensemble* from_ensemble, unsigned int delay )
{
	this->delay = delay;
	name = "DelayEnsemble";

	for (unsigned int i=0; i < from_ensemble->nodes.size();i++)
	{
		Node* prev = from_ensemble->nodes[i];
		for (unsigned int j=0; j < delay; j++)
		{
			LinearNode* node = new LinearNode();
			this->nodes.push_back(node);
			Connection* connection = Network::connect(prev, node, false, 1.0);
			connection->set_identity_and_freeze();
			prev = node;
		}
	}

}

DelayEnsemble* DelayEnsemble::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

	DelayEnsemble* ensemble = new DelayEnsemble(nodes);
	return ensemble;
}

string DelayEnsemble::to_string()
{
	stringstream stream;
	stream << delay;
	return stream.str();
}
