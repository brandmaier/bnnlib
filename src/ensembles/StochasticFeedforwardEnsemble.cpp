#include "StochasticFeedforwardEnsemble.h"
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
#include "../nodes/ReLUNode.h"
#include "../nodes/StochasticSigmoidNode.h"
#include "../NodeFactory.h"
#include <stdlib.h>
#include <string>
#include <algorithm>

using namespace std;

	/**
	 * create an ensemble of Nodes that represent the notion of
	 * a stochastic feedforward layer. There are no connections between
	 * the nodes.
	 * 
	 **/
	
StochasticFeedforwardEnsemble::StochasticFeedforwardEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
	{
    //TODO add type check
	  name = "Feedforward Ensemble";
	  this->node_type = ((*nodes)[0])->get_type();
	}


StochasticFeedforwardEnsemble::StochasticFeedforwardEnsemble(unsigned int size, weight_t gain, weight_t bias, weight_t stochastic_range)
{
	name = "Stochastic Feedforward Ensemble";
	
			for (unsigned int i=0; i < size; i++)
			{
				Node* node = new StochasticSigmoidNode(gain, bias, stochastic_range);
				this->add_node( node );
			}
				
}

StochasticFeedforwardEnsemble::StochasticFeedforwardEnsemble(unsigned int size)
{
  name = "Stochastic Feedforward Ensemble";
  
  for (unsigned int i=0; i < size; i++)
  {
    Node* node = new StochasticSigmoidNode(1, 0, 0);
    this->add_node( node );
  }
  
}

StochasticFeedforwardEnsemble* StochasticFeedforwardEnsemble::from_string(string parameters)
{
	
	unsigned int size = stoi(parameters);
	
	StochasticFeedforwardEnsemble* ensemble = new StochasticFeedforwardEnsemble(size);	
	return ensemble;	
}

StochasticFeedforwardEnsemble* StochasticFeedforwardEnsemble::from_string(string parameters, vector<Node*>* nodes)
{

	unsigned int size = stoi(parameters);
	
	for (unsigned int i=0; i < nodes->size(); i++)
	{
		if ((*nodes)[i]->get_type() != Node::STOCHASTIC_SIGMOID_NODE) {
		
		 error("Cannot instantiate StochasticFeedforwardEnsemble from string! Node type mismatch! ");
		 exit(0);				
		}	
	}
	
	if (size != nodes->size())
	{
		 error("Cannot instantiate StochasticFeedforwardEnsemble from string! Wrong node size! ");
		exit(0);	
	}
	
	StochasticFeedforwardEnsemble* ensemble = new StochasticFeedforwardEnsemble(nodes);	
	return ensemble;	
}	

void StochasticFeedforwardEnsemble::set_stochastic_range(weight_t x) {
  
  for (unsigned int i=0; i < nodes.size(); i++)
  {
    //TODO: unsafe type-cast
    ((StochasticSigmoidNode*)nodes[i])->stochastic_range = x;
  }
}

string StochasticFeedforwardEnsemble::to_string()
{
	stringstream stream;
	stream << nodes.size();
	return stream.str();	
}
