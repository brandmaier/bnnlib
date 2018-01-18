#include "LSTMPlusEnsemble.h"
#include "../nodes/Node.h"
#include "../nodes/ProductNode.h"
#include "../nodes/LinearNode.h"
#include "../Network.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/BiasNode.h"
#include "../nodes/LnNode.h"


int LSTMPlusEnsemble::LSTM_COUNTER = 0;

/*	Node* LSTMPlusEnsemble::get_inputgate() {return nodes[1];}
	Node* LSTMPlusEnsemble::get_outputgate(){return nodes[2];}
	Node* LSTMPlusEnsemble::get_forgetgate() { return nodes[3];}
	Node* LSTMPlusEnsemble::get_cec(){return nodes[6];}
	Node* LSTMPlusEnsemble::get_output(){return nodes[8];}
*/

LSTMPlusEnsemble::LSTMPlusEnsemble(std::vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "LSTMPlusEnsemble";
	// TODO set references to weights! BUG
}

LSTMPlusEnsemble::LSTMPlusEnsemble(bool peepholes, int cec_activation)
{
	init(peepholes, cec_activation,-1,+1,-1,0,0,0);
}

LSTMPlusEnsemble::LSTMPlusEnsemble(bool peepholes, int cec_activation,
		weight_t bias_discount_input, weight_t  bias_discount_forget, weight_t  bias_discount_output)
{
	init(peepholes, cec_activation,
			bias_discount_input,bias_discount_forget,bias_discount_output,
			0,0,0);
}

void LSTMPlusEnsemble::init(bool peepholes,  nodetype cec_activation,
		weight_t bias_discount_input, weight_t bias_discount_forget, weight_t bias_discount_output,
		weight_t bias_offset_input, weight_t bias_offset_output, weight_t bias_offset_forget)
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
	Node* forgetgate = new SigmoidNode();
	Node* input = new SigmoidNode();
	Node* input_mult = new ProductNode();
	Node* cell = new LinearNode();
	Node* forget_mult = new ProductNode();
	Node* cell_act;

	if (cec_activation == Node::SIGMOID_NODE) {
		cell_act = new SigmoidNode();
	} else if (cec_activation == Node::LINEAR_NODE) {
		cell_act = new LinearNode();
	} else {
		error("Invalid Activation function given for LSTM Forget Ensemble");
	}
	Node* output_mult = new ProductNode();

	bias->name = "lstmbias";
	inputgate->name = "ingate";
	outputgate->name ="outgate";
	forgetgate->name = "forgetgate";
	cell->name = "CEC";
	output_mult->name = "lstmoutput";
	
	nodes.push_back(bias);
	nodes.push_back(inputgate);
	nodes.push_back(outputgate);
	nodes.push_back(forgetgate);
	nodes.push_back(input);
	nodes.push_back(input_mult);
	nodes.push_back(cell);
	nodes.push_back(forget_mult);
	nodes.push_back(cell_act);
	nodes.push_back(output_mult);

	bool dir = true;

	c1 = Network::connect(inputgate, input_mult, dir);
	c2 = Network::connect(input, input_mult, dir);
	c3 = Network::connect(input_mult, cell, dir);
	f1 = Network::connect(cell, forget_mult,false);
	f2 = Network::connect(forgetgate, forget_mult, dir);
	f3 = Network::connect(forget_mult, cell, dir);
	c4 = Network::connect(cell, cell_act, dir);
	c5 = Network::connect(cell_act, output_mult, dir);
	c6 = Network::connect(outputgate, output_mult, dir);

	cout << "BIAS LSTM: " << (bias_offset_input + bias_discount_input*LSTM_COUNTER)<<" "
			<< (bias_offset_forget + bias_discount_forget*LSTM_COUNTER)
	<< +(bias_offset_output + bias_discount_output*LSTM_COUNTER) << " " << endl;

	b1 = Network::connect(bias, inputgate, true, +(bias_offset_input + bias_discount_input*LSTM_COUNTER) );
	b3 = Network::connect(bias, outputgate, true, +(bias_offset_output + bias_discount_output*LSTM_COUNTER) );
	b2 = Network::connect(bias, forgetgate, true, +(bias_offset_forget + bias_discount_forget*LSTM_COUNTER) );

	
				
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
		/*Connection* cp1 =*/ Network::connect(cell, inputgate, false);		
		/*Connection* cp2 = */ Network::connect(cell, outputgate, true);
		/*Connection* cp3 =*/ Network::connect(cell, forgetgate, false);
		
		//cp1->set_identity_and_freeze();
		//cp2->set_identity_and_freeze();
	}	
	
	LSTM_COUNTER++;

}

LSTMPlusEnsemble::~LSTMPlusEnsemble()
{
	
}

	Node* LSTMPlusEnsemble::get_inputgate() {return nodes[1];}
	Node* LSTMPlusEnsemble::get_outputgate(){return nodes[2];}
	Node* LSTMPlusEnsemble::get_forgetgate() { return nodes[3];}
	Node* LSTMPlusEnsemble::get_cec(){return nodes[6];}
	Node* LSTMPlusEnsemble::get_output(){return nodes[8];}

std::vector<Node*>* LSTMPlusEnsemble::get_inputs() {
		return &this->inputs;
}
	
std::vector<Node*>* LSTMPlusEnsemble::get_outputs() {
		return &this->outputs;
}

string LSTMPlusEnsemble::to_string()
{
	stringstream out;
	out << (peepholes==true?"1":"0");
	out << ";" << get_cec()->get_type();
	return out.str();
}

LSTMPlusEnsemble* LSTMPlusEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

    bool peepholes = (atoi(tokens[0].c_str())==1);
     int cec_type = atoi(tokens[1].c_str());
    LSTMPlusEnsemble* ensemble = new LSTMPlusEnsemble(peepholes, cec_type);

    return ensemble;
}

LSTMPlusEnsemble* LSTMPlusEnsemble::from_string(string parameters, vector<Node*>* nodes)
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

    LSTMPlusEnsemble* ensemble = new LSTMPlusEnsemble(nodes);
    ensemble->peepholes = peepholes;

    return ensemble;
}
