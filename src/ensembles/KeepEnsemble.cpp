#include "KeepEnsemble.h"
#include <vector>

using namespace std;

unsigned int KeepEnsemble::KEEP_COUNTER = 0;

KeepEnsemble::KeepEnsemble(unsigned int size)
{
		KEEP_COUNTER++;
	
		Node* bias = new BiasNode();
		Node* input = new TanhNode();
		Node* output_gate = new SigmoidNode();
		Node* input_gate = new SigmoidNode();
		Node* input_mult = new PiNode();
		Node* output_mult = new PiNode();
		Node* keep_sum = new TanhNode();
		
		input_gate->name = "keep_input_gate";
		output_gate->name = "keep_output_gate";
		input->name = "keep_input";
		output_mult->name = "keep_output";
		
		vector<Node*> inner_nodes;
		inner_nodes.reserve(size);
		for (unsigned int i=0; i < size; i++)
		{
			inner_nodes.push_back( new TanhNode() );
		}		
		nodes.push_back(bias);
		nodes.push_back(input);
		nodes.push_back(input_gate);
		nodes.push_back(output_gate);
		nodes.push_back(input_mult);

		
		for (unsigned int i=0; i < size; i++)
		{
			nodes.push_back( inner_nodes[i] );
		} 
		nodes.push_back(keep_sum);
		nodes.push_back(output_mult);
		
		Network::connect(bias, input_gate, false, (-0.5*KEEP_COUNTER) );
		Network::connect(bias, output_gate, false, (-0.5*KEEP_COUNTER) );
		
		for (unsigned int i=0; i < size; i++)
		{
			Network::connect(input_mult, inner_nodes[i], false);
			Network::connect(inner_nodes[i], keep_sum, false);
			for (unsigned int j=0; j < size; j++)
			{		
				Network::connect(inner_nodes[i], inner_nodes[j], false);
			}
		} 	
		
		Connection* c4 = Network::connect(keep_sum, output_mult, false);
		
		Connection* c1 = Network::connect(input, input_mult, false);
		Connection* c2 = Network::connect(input_gate, input_mult, false);	
		Connection* c3 = Network::connect(output_gate, output_mult, false);
		c1->set_identity_and_freeze();
		c2->set_identity_and_freeze();
		c3->set_identity_and_freeze();
		c4->set_identity_and_freeze();
		
		outputs.push_back(output_mult);
		inputs.push_back(input);
		inputs.push_back(input_gate);
		inputs.push_back(output_gate);
}

std::vector<Node*>* KeepEnsemble::get_inputs() {
		return &this->inputs;
}
	
std::vector<Node*>* KeepEnsemble::get_outputs() {
		return &this->outputs;
}
