#include "Ensemble.h"
#include "../nodes/LinearNode.h"
#include "../nodes/BiasNode.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/Node.h"


	Ensemble::Ensemble() {
		name = "Generic Ensemble";
	}
	
	Ensemble::Ensemble(std::vector<Node*>* nodes)
	{
		name = "Generic Ensemble";
		for (unsigned int i=0; i < nodes->size(); i++)
		{
			this->nodes.push_back( (*nodes)[i] );	
		}	
	}	
	
	Ensemble::Ensemble(Ensemble &ensemble) {
	  name = ensemble.name;
	  
	  nodes = ensemble.nodes;
	}
	
	Ensemble::~Ensemble() {
		/*for (unsigned int i = 0; i < nodes.size();i++)
		{
			delete nodes[i];	
		}*/
	}
	
	void Ensemble::add_node(Node* node)
	{
		nodes.push_back(node);	
		//node->enseble = this;
	}	
	
	bool Ensemble::contains_node(Node* node)
	{
		vector<Node*>::iterator it;
		for (it = nodes.begin(); it < nodes.end(); it++)
		{
			if ((*it) == node) return true;
		}
		
		return false;
	}
	

	
	/*Ensemble* Ensemble::create_feedforward_sigmoid_ensemble(unsigned int size)
	{
			Ensemble* ensemble = new Ensemble();
			
			for (unsigned int i=0; i < size; i++)
			{
				Node* node = new SigmoidNode();
				ensemble->add_node( node );
			}
			
			return ensemble;
	}	*/

	
	Ensemble* Ensemble::create_fullrecurrent_ensemble(unsigned int size)
	{
		Ensemble* ensemble = new Ensemble();
			
		for (unsigned int i=0; i < size; i++)
		{
			Node* node = new TanhNode();
			ensemble->add_node( node );
		}
			
		for (unsigned int i=0; i < size; i++) {
			for (unsigned int j=0; j < size; j++) {
				 Network::connect(ensemble->nodes[i],ensemble->nodes[j], false);
			}
		}			
			
		return ensemble;
	}
	
	Ensemble* Ensemble::create_selfrecurrent_ensemble(unsigned int size)
	{
		Ensemble* ensemble = new Ensemble();
			
		for (unsigned int i=0; i < size; i++)
		{
			Node* node = new TanhNode();
			ensemble->add_node( node );
		}
			
		for (unsigned int i=0; i < size; i++) {

			Network::connect(ensemble->nodes[i],ensemble->nodes[i], false);
			
		}			
			
		return ensemble;
	}	
	
	Ensemble* Ensemble::create_stacked_ensemble(unsigned int size)
	{
		Ensemble* ensemble = new Ensemble();
			
		for (unsigned int i=0; i < size; i++)
		{
			Node* node = new TanhNode();
			ensemble->add_node( node );
		}
			
		for (unsigned int i=0; i < size; i++) {
			for (unsigned int j=0; j < size; j++) {
				if (i < j)
				 Network::connect(ensemble->nodes[i],ensemble->nodes[j], true);
			}
		}			
			
		return ensemble;
	}	
	

	/*
std::vector<Node*>* Ensemble::get_inputs() {
		return &this->nodes;
}*/
	/*
std::vector<Node*>* Ensemble::get_outputs() {
		return &this->nodes;
}
*/
unsigned int Ensemble::get_num_nodes()
{
	return this->nodes.size();	
}

string Ensemble::to_string()
{
	return "<NONE>";
}

/*
void Ensemble::set_frozen(bool state)
{
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			nodes[i]->outgoing_connections[j]->freeze_weight = state;	
		}
	}	
}
	
*/
