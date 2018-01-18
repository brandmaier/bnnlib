#include "RecurrentEnsemble.h"
#include <iostream>
#include "../nodes/Node.h"
#include "../nodes/TanhNode.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/LinearNode.h"
#include "../nodes/BiasNode.h"
#include <stdlib.h>
#include <string>
#include <algorithm>
#include "../Network.h"

RecurrentEnsemble::RecurrentEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
{
	this->node_type = ((*nodes)[0])->get_type();
	this->name = "Recurrent Ensemble";
}

RecurrentEnsemble::RecurrentEnsemble(int node_type, unsigned int size)
{
		this->node_type = node_type;
		this->name = "Recurrent Ensemble";
	
			for (unsigned int i=0; i < size; i++)
			{
				Node* node;
				switch(node_type) {
				case Node::TANH_NODE:
				 node = new TanhNode();
				break;
				case Node::SIGMOID_NODE:
				 node = new SigmoidNode();
				break;
				case Node::LINEAR_NODE:
				 node = new LinearNode();
				break;
				case Node::BIAS_NODE:
				 node = new BiasNode();
				break;
				error( "No matching Node type when instantiating Feedforward Ensemble!");;
				
				}
				this->add_node( node );
			}
			
		for (unsigned int i=0; i < nodes.size(); i++)
			for (unsigned int j=0; j < nodes.size(); j++)
			{
				Network::connect(nodes[i], nodes[j], false);	
				
			}			
				
}

RecurrentEnsemble* RecurrentEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

	int type = atoi(tokens[0].c_str());
	unsigned int size = atoi(tokens[1].c_str());

	RecurrentEnsemble* ensemble = new RecurrentEnsemble(type, size);
	return ensemble;
}

RecurrentEnsemble* RecurrentEnsemble::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

	 int type = atoi(tokens[0].c_str());
	unsigned int size = atoi(tokens[1].c_str());

	for (unsigned int i=0; i < nodes->size(); i++)
	{
		if ((*nodes)[i]->get_type() != type) {
		 cout << type << "/" << (*nodes)[i]->get_type() << endl;
		 error("Cannot instantiate RecurrentEnsemble from string! Node type mismatch! ");
		 exit(0);
		}
	}

	if (size != nodes->size())
	{
		 error("Cannot instantiate RecurrentEnsemble from string! Wrong node size! ");
		exit(0);
	}

	RecurrentEnsemble* ensemble = new RecurrentEnsemble(nodes);
	return ensemble;
}


string RecurrentEnsemble::to_string()
{
	stringstream stream;
	stream << node_type << ";" << nodes.size();
	return stream.str();
}
