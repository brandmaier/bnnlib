#include "FeedforwardEnsemble.h"
#include <iostream>
#include "../nodes/Node.h"
#include "../nodes/TanhNode.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/LinearNode.h"
#include "../nodes/BiasNode.h"
#include "../nodes/TimedSinNode.h"
#include "../nodes/SinNode.h"
#include "../nodes/OneShotBiasNode.h"
#include "../nodes/GaussianNoiseNode.h"
#include <stdlib.h>
#include <string>
#include <algorithm>

using namespace std;

	/**
	 * create an ensemble of Nodes that represent the notion of
	 * a feedforward layer. There are no connections between
	 * the Nodes present.
	 * 
	 **/

FeedforwardEnsemble::FeedforwardEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "Feedforward Ensemble";
	this->node_type = ((*nodes)[0])->get_type();
}

FeedforwardEnsemble::FeedforwardEnsemble(int node_type, unsigned int size)
{
	name = "Feedforward Ensemble";
		this->node_type = node_type;
	
			for (unsigned int i=0; i < size; i++)
			{
				Node* node = NULL;
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
				case Node::TSIN_NODE:
					node = new TimedSinNode();
				break;
				case Node::SIN_NODE:
					node = new SinNode();
				break;
				case Node::ONE_SHOT_BIAS_NODE:
					node = new OneShotBiasNode();
				break;
				case Node::GAUSSIAN_NOISE_NODE:
					node = new GaussianNoiseNode();
				break;
				default:
				error( "No matching Node type when instantiating Feedforward Ensemble!");;
				break;
				}
				this->add_node( node );
			}
				
}

FeedforwardEnsemble* FeedforwardEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);  
         
	int type = atoi(tokens[0].c_str());
	unsigned int size = atoi(tokens[1].c_str());
	
	FeedforwardEnsemble* ensemble = new FeedforwardEnsemble(type, size);	
	return ensemble;	
}

FeedforwardEnsemble* FeedforwardEnsemble::from_string(string parameters, vector<Node*>* nodes)
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
		 error("Cannot instantiate FeedforwardEnsemble from string! Node type mismatch! ");
		 exit(0);				
		}	
	}
	
	if (size != nodes->size())
	{
		 error("Cannot instantiate FeedforwardEnsemble from string! Wrong node size! ");
		exit(0);	
	}
	
	FeedforwardEnsemble* ensemble = new FeedforwardEnsemble(nodes);	
	return ensemble;	
}	


string FeedforwardEnsemble::to_string()
{
	stringstream stream;
	stream << node_type << ";" << nodes.size();
	return stream.str();	
}
