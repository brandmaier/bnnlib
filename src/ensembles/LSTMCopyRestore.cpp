/*
 * LSTMCopyRestore.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "LSTMCopyRestore.h"

int LSTMCopyRestore::LSTM_COUNTER = 0;

	Node* LSTMCopyRestore::get_inputgate() {return nodes[1];}
	Node* LSTMCopyRestore::get_outputgate(){return nodes[2];}
	Node* LSTMCopyRestore::get_forgetgate() { return nodes[3];}
	Node* LSTMCopyRestore::get_cec(){return nodes[6];}
	Node* LSTMCopyRestore::get_output(){return nodes[8];}

LSTMCopyRestore::LSTMCopyRestore(std::vector<Node*>* nodes) : Ensemble(nodes)
{
	name = "LSTMCopyRestore";
	// TODO set references to weights! BUG
}

LSTMCopyRestore::LSTMCopyRestore(bool peepholes,  nodetype cec_activation)
{

	init(peepholes, cec_activation,-1,+3,-2,0,0,0);

}


LSTMCopyRestore::LSTMCopyRestore(bool peepholes,  nodetype cec_activation,
		weight_t bias_discount_input, weight_t bias_discount_forget, weight_t bias_discount_output)
{
	init(peepholes, cec_activation,
			bias_discount_input,bias_discount_forget,bias_discount_output,
			0,0,0);
}

void LSTMCopyRestore::init(bool peepholes,  nodetype cec_activation,
		weight_t bias_discount_input, weight_t bias_discount_forget, weight_t bias_discount_output,
		weight_t bias_offset_input, weight_t bias_offset_forget, weight_t bias_offset_output)
{
	name = "LSTMCopyRestore";
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
	Node* input = new TanhNode();
	Node* input_mult = new PiNode();
	Node* cell = new LinearNode();
	Node* forget_mult = new PiNode();
	Node* cell_act;

	Node* copygate = new SigmoidNode();
	Node* copy = new LinearNode();
	Node* copymult = new PiNode();
	Node* restoregate = new SigmoidNode();
	Node* restoremult = new PiNode();


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
	cell->name = "cell";
	output_mult->name = "lstmoutput";

	nodes.push_back(bias);
	nodes.push_back(inputgate);
	nodes.push_back(outputgate);
	nodes.push_back(forgetgate);
	nodes.push_back(copygate);
	nodes.push_back(restoregate);
	nodes.push_back(input);
	nodes.push_back(input_mult);
	nodes.push_back(cell);
	nodes.push_back(copymult);
	nodes.push_back(restoremult);
	nodes.push_back(copy);
	nodes.push_back(forget_mult);
	nodes.push_back(cell_act);
	nodes.push_back(output_mult);

	bool dir = true;

	c1 = Network::connect(inputgate, input_mult, dir);
	c2 = Network::connect(input, input_mult, dir);
	c3 = Network::connect(input_mult, cell, dir);
	f1 = Network::connect(cell, forget_mult,dir);
	f2 = Network::connect(forgetgate, forget_mult, dir);
	f3 = Network::connect(forget_mult, cell, false);
	c4 = Network::connect(cell, cell_act, dir);
	c5 = Network::connect(cell_act, output_mult, dir);
	c6 = Network::connect(outputgate, output_mult, dir);

	cout << "BIAS LSTM: " << (bias_offset_input + bias_discount_input*LSTM_COUNTER)<<" "
			<< (bias_offset_forget + bias_discount_forget*LSTM_COUNTER)
	<< +(bias_offset_output + bias_discount_output*LSTM_COUNTER) << " " << endl;

	b1 = Network::connect(bias, inputgate, true, +(bias_offset_input + bias_discount_input*LSTM_COUNTER) );
	b3 = Network::connect(bias, outputgate, true, +(bias_offset_output + bias_discount_output*LSTM_COUNTER) );
	b2 = Network::connect(bias, forgetgate, true, +(bias_offset_forget + bias_discount_forget*LSTM_COUNTER) );


	Network::connect(copygate, copymult, true);
	Network::connect(cell, copymult, true);
	Network::connect(copymult, copy, true);
	Network::connect(copy, restoremult, true);
	Network::connect(restoregate, restoremult, true);
	Network::connect(restoremult, cell, false);

	inputs.push_back( input );
	inputs.push_back( inputgate );
	inputs.push_back( forgetgate );
	inputs.push_back( outputgate );
	inputs.push_back( copygate );
	inputs.push_back( restoregate );

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
		/*Connection* cp2 = */ Network::connect(cell, outputgate, false);
		/*Connection* cp3 =*/ Network::connect(cell, forgetgate, false);

		//cp1->set_identity_and_freeze();
		//cp2->set_identity_and_freeze();
	}

	LSTM_COUNTER++;

}

LSTMCopyRestore::~LSTMCopyRestore()
{

}

std::vector<Node*>* LSTMCopyRestore::get_inputs() {
		return &this->inputs;
}

std::vector<Node*>* LSTMCopyRestore::get_outputs() {
		return &this->outputs;
}

void LSTMCopyRestore::set_frozen(bool state)
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

string LSTMCopyRestore::to_string()
{
	stringstream out;
	out << (peepholes==true?"1":"0");
	out << ";" << get_cec()->get_type();
	return out.str();
}

LSTMCopyRestore* LSTMCopyRestore::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

    bool peepholes = (atoi(tokens[0].c_str())==1);
     int cec_type = atoi(tokens[1].c_str());
    LSTMCopyRestore* ensemble = new LSTMCopyRestore(peepholes, cec_type);

    return ensemble;
}

LSTMCopyRestore* LSTMCopyRestore::from_string(string parameters, vector<Node*>* nodes)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

    bool peepholes = (atoi(tokens[0].c_str())==1);
    int cec_type = atoi(tokens[1].c_str());

    if (cec_type != (*nodes)[6]->get_type() )
    {
    	cout << "CEC: " << cec_type <<endl;
    	cout << "is : " << (*nodes)[6]->get_type() << endl;
    	error("CEC type mismatch when creating LSTM from string!");
    }

    LSTMCopyRestore* ensemble = new LSTMCopyRestore(nodes);
    ensemble->peepholes = peepholes;

    return ensemble;
}
