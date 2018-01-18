#include "LSTMForgetEnsemble.h"


int LSTMForgetEnsemble::LSTM_COUNTER = 0;

	Node* LSTMForgetEnsemble::get_inputgate() {return nodes[1];}
	Node* LSTMForgetEnsemble::get_outputgate(){return nodes[3];}
	Node* LSTMForgetEnsemble::get_forgetgate() { return nodes[2];}
	Node* LSTMForgetEnsemble::get_cec(){return nodes[7];}
	Node* LSTMForgetEnsemble::get_output(){return nodes[9];}
	Node* LSTMForgetEnsemble::get_input(){return nodes[4];}

LSTMForgetEnsemble::LSTMForgetEnsemble(std::vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "LSTMForgetEnsemble";
	// TODO set references to weights! BUG
}

LSTMForgetEnsemble::LSTMForgetEnsemble(bool peepholes,  nodetype cec_activation)
{

	init(peepholes, Node::TANH_NODE, cec_activation,0,0,0);
	//init(peepholes, cec_activation,+1,-3,+2,0,0,0);
	//init(peepholes, cec_activation,-0.2,+.5,-0.2,0,0,0);

}


LSTMForgetEnsemble::LSTMForgetEnsemble(bool peepholes,nodetype input_activation,  nodetype cec_activation,
		weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight)
{
	init(peepholes, input_activation,cec_activation,bias_input_weight, bias_forget_weight, bias_output_weight);
}

LSTMForgetEnsemble::LSTMForgetEnsemble(bool peepholes,  nodetype cec_activation,
		weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight)
{
	init(peepholes, Node::TANH_NODE,cec_activation,bias_input_weight, bias_forget_weight, bias_output_weight);
}

void LSTMForgetEnsemble::init(bool peepholes, nodetype input_activation, nodetype cec_activation,
		weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight
)
{
	name = "LSTMForgetEnsemble";
	this->cec_activation = cec_activation;
	// Gers - Grammar Learning
	// all nodes: input -1.0, forget +2.0, output -2.0
	
	/* bias_discount_input = -1;  andys guess
	 bias_discount_forget = +3;
	 bias_discount_output = -2;
	*/
	

	
	this->peepholes = peepholes;
	
	Node* bias = new BiasNode();
	Node* inputgate = new SigmoidNode();
	Node* outputgate = new SigmoidNode();
	Node* forgetgate = new SigmoidNode(); //new ScaledSigmoidNode(2.0); //SteepSigmoidNode(1.0);
	Node* input;
	if (input_activation == Node::TANH_NODE) {
		input 	= new TanhNode();
	} else if (input_activation == Node::LINEAR_NODE) {
		input = new LinearNode();
	} else {
		error("LSTMForgetEnsemble: not all input activations implemented!");
	}

	Node* input_mult = new PiNode();
	Node* forget_mult = new PiNode();
	Node* cell = new LinearNode();
	Node* cell_act;
	if (cec_activation == Node::TANH_NODE) {
		cell_act = new TanhNode();
	} else if (cec_activation == Node::LINEAR_NODE) {
		cell_act = new LinearNode();
	} else {
		error("Invalid Activation function given for LSTM Forget Ensemble");
	}
	Node* output_mult = new PiNode();

	bias->name = "lstmbias";
	inputgate->name = "ingate";
	outputgate->name ="outgate";
	forgetgate->name = "forgetgate";
	cell->name = "CEC";
	output_mult->name = "out";
	input->name = "input";
	input_mult->name = "in_mult";
	forget_mult->name = "f_mult";
	output_mult->name = "o_mult";
	
	inputgate->functional_type = Node::LSTM_INPUT_GATE;
	forgetgate->functional_type = Node::LSTM_FORGET_GATE;
	outputgate->functional_type = Node::LSTM_OUTPUT_GATE;
	cell->functional_type = Node::LSTM_CEC;

	nodes.push_back(bias);	//#0
	nodes.push_back(inputgate);	//#1
	nodes.push_back(forgetgate);	//#2
	nodes.push_back(outputgate);	//#3
	nodes.push_back(input);	//#4
	nodes.push_back(input_mult);	//#5
	nodes.push_back(forget_mult);	//#6
	nodes.push_back(cell);	//#7
	nodes.push_back(cell_act);	//#8
	nodes.push_back(output_mult);	//#9

	bool dir = true;

	bool graves_style = true;

	c1 = Network::connect(inputgate, input_mult, dir);
	c2 = Network::connect(input, input_mult, dir);
	c3 = Network::connect(input_mult, cell, dir);
	f1 = Network::connect(cell, forget_mult, !graves_style);
	f2 = Network::connect(forgetgate, forget_mult, dir);
	f3 = Network::connect(forget_mult, cell, graves_style);
	c4 = Network::connect(cell, cell_act, dir);
	c5 = Network::connect(cell_act, output_mult, dir);
	c6 = Network::connect(outputgate, output_mult, dir);

/*	cout << "BIAS LSTM: " << (bias_offset_input + bias_discount_input*LSTM_COUNTER)<<" "
			<< (bias_offset_forget + bias_discount_forget*LSTM_COUNTER)
	<< +(bias_offset_output + bias_discount_output*LSTM_COUNTER) << " " << endl;
*/
	if (abs(bias_input_weight) > 10e-9)
	 b1 = Network::connect(bias, inputgate, true, bias_input_weight );
	
	if (abs(bias_forget_weight) > 10e-9)
	 b2 = Network::connect(bias, forgetgate, true, bias_forget_weight );

	if (abs(bias_output_weight) > 10e-9)
		 b3 = Network::connect(bias, outputgate, true, bias_output_weight );

	
				
	inputs.push_back( input );
	inputs.push_back( inputgate );
	inputs.push_back( forgetgate );
	inputs.push_back( outputgate );
	
	outputs.push_back( output_mult );
	
	// self-recurrent connection is constant

	//self->set_identity_and_freeze();
	c1->set_identity_and_freeze();
	c2->set_identity_and_freeze();
	c3->set_identity_and_freeze();
	c4->set_identity_and_freeze();
	c5->set_identity_and_freeze();	
	c6->set_identity_and_freeze();
	f1->set_identity_and_freeze();
	f2->set_identity_and_freeze();
	f3->set_identity_and_freeze();
	
	if (peepholes) {
		cp1 = Network::connect(cell, inputgate, false);
		cp2 =  Network::connect(cell, outputgate, true);
		cp3 =Network::connect(cell, forgetgate, false);
		
		//cp1->set_identity_and_freeze();
		//cp2->set_identity_and_freeze();
	}	
	

	bool gated_bias = false;
	if (gated_bias) {

		Node* bias = new BiasNode();
		Node* bias_mult = new PiNode();
		Node* bias_gate = new SigmoidNode();
		nodes.push_back(bias);
		nodes.push_back(bias_mult);
		nodes.push_back(bias_gate);

		Connection* gb1 = Network::connect(bias, bias_mult, true);
		Connection* gb2 = Network::connect(bias_gate, bias_mult, true);
		Connection* gb3 = Network::connect(bias_mult, cell, true);

		gb1->set_identity_and_freeze();
		gb2->set_identity_and_freeze();
		gb3->set_identity_and_freeze();

		inputs.push_back(bias_gate);

	}

	//open_mults_to_public();

	LSTM_COUNTER++;

}

LSTMForgetEnsemble::~LSTMForgetEnsemble()
{

}

std::vector<Node*>* LSTMForgetEnsemble::get_inputs() {
		return &this->inputs;
}
	
std::vector<Node*>* LSTMForgetEnsemble::get_outputs() {
		return &this->outputs;
}

void LSTMForgetEnsemble::set_frozen(bool state)
{
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			nodes[i]->outgoing_connections[j]->freeze_weight = state;	
		}
	}	
	
	//self->set_identity_and_freeze();
	c1->set_identity_and_freeze();
	c2->set_identity_and_freeze();
	c3->set_identity_and_freeze();
	c4->set_identity_and_freeze();
	c5->set_identity_and_freeze();	
	c6->set_identity_and_freeze();	
	f1->set_identity_and_freeze();
	f2->set_identity_and_freeze();
	f3->set_identity_and_freeze();
}

string LSTMForgetEnsemble::to_string()
{
	stringstream out;
	out << (peepholes==true?"1":"0");
	out << ";" << get_cec()->get_type();
	return out.str();
}

LSTMForgetEnsemble* LSTMForgetEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);  
    
    bool peepholes = (atoi(tokens[0].c_str())==1);
     int cec_type = atoi(tokens[1].c_str());
    LSTMForgetEnsemble* ensemble = new LSTMForgetEnsemble(peepholes, cec_type);
    
    return ensemble;
}

LSTMForgetEnsemble* LSTMForgetEnsemble::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);  
    
    bool peepholes = (atoi(tokens[0].c_str())==1);
    int cec_type = atoi(tokens[1].c_str());

    if (cec_type != (*nodes)[7]->get_type() )
    {
    	cout << "CEC: " << cec_type <<endl;
    	cout << "is : " << (*nodes)[7]->get_type() << endl;
    	error("CEC type mismatch when creating LSTM from string!");
    }
    
    LSTMForgetEnsemble* ensemble = new LSTMForgetEnsemble(nodes);
    ensemble->peepholes = peepholes;
    
    return ensemble;
}

void LSTMForgetEnsemble::open_mults_to_public()
{
	// add the multiplicative units to the public
	outputs.push_back( nodes[5] );
	outputs.push_back( nodes[6] );
	outputs.push_back( nodes[9] );
}
