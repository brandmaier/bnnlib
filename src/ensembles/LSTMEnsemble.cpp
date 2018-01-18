#include "LSTMEnsemble.h"


LSTMEnsemble::LSTMEnsemble(std::vector<Node*>* nodes) : Ensemble(nodes)
{
		inputs.push_back( (*nodes)[0] );
		inputs.push_back( (*nodes)[1] );
		inputs.push_back( (*nodes)[2] );		
		outputs.push_back( (*nodes)[6] );		
}

LSTMEnsemble::LSTMEnsemble()
{
	init(false);
}

LSTMEnsemble::LSTMEnsemble(bool peepholes)
{
	init(peepholes);
}

	Node* LSTMEnsemble::get_inputgate() {return nodes[0];}
	Node* LSTMEnsemble::get_outputgate(){return nodes[1];}

	Node* LSTMEnsemble::get_cec(){return nodes[4];}
	Node* LSTMEnsemble::get_output(){return nodes[6];}

void LSTMEnsemble::init(bool peepholes)
{
	this->peepholes = peepholes;
	
	Node* inputgate = new SigmoidNode();
	Node* outputgate = new SigmoidNode();
	Node* input = new TanhNode();
	Node* input_mult = new PiNode();
	Node* cell = new LinearNode();
	Node* cell_act = new TanhNode();		
	Node* output_mult = new PiNode();
	
	inputgate->name = "ingate";
	outputgate->name ="outgate";
	cell->name = "cell";
	output_mult->name = "output";
	
	inputgate->functional_type = Node::LSTM_INPUT_GATE;
	//forgetgate->functional_type = Node::LSTM_FORGET_GATE;
	outputgate->functional_type = Node::LSTM_OUTPUT_GATE;
	cell->functional_type = Node::LSTM_CEC;


	nodes.push_back(inputgate);
	nodes.push_back(outputgate);
	nodes.push_back(input);
	nodes.push_back(input_mult);
	nodes.push_back(cell);
	nodes.push_back(cell_act);
	nodes.push_back(output_mult);

	bool direction = true;

	Connection* c1 = Network::connect(inputgate, input_mult, direction);
	Connection* c2 = Network::connect(input, input_mult,direction);
	Connection* c3 = Network::connect(input_mult, cell,direction);
	Connection* self = Network::connect(cell, cell,direction);
	Connection* c4 = Network::connect(cell, cell_act,direction);
	Connection* c5 = Network::connect(cell_act, output_mult,direction);
	Connection* c6 = Network::connect(outputgate, output_mult,direction);
	
	
	inputs.push_back( input );
	inputs.push_back( inputgate );
	inputs.push_back( outputgate );	
	outputs.push_back( output_mult );
	
	self->set_identity_and_freeze();
	c1->set_identity_and_freeze();
	c2->set_identity_and_freeze();
	c3->set_identity_and_freeze();
	c4->set_identity_and_freeze();
	c5->set_identity_and_freeze();	
	c6->set_identity_and_freeze();
	
	if (peepholes) {
		Connection* cp1 = Network::connect(cell_act, inputgate, false);		
		Connection* cp2 = Network::connect(cell_act, outputgate, false);
		//cp1->set_identity_and_freeze();
		//cp2->set_identity_and_freeze();
	}
	
	// set Ensemble references:
	for (unsigned int i=0; i < nodes.size();i++)
	{
		nodes[i]->ensemble = this;
	}
}

LSTMEnsemble::~LSTMEnsemble()
{
	
}

std::vector<Node*>* LSTMEnsemble::get_inputs() {
		return &this->inputs;
}
	
std::vector<Node*>* LSTMEnsemble::get_outputs() {
		return &this->outputs;
}
