#include "LSTMCounterEnsemble.h"


LSTMCounterEnsemble::LSTMCounterEnsemble(std::vector<Node*>* nodes) : Ensemble(nodes)
{
	
}

LSTMCounterEnsemble::LSTMCounterEnsemble()
{
	init(true);
}

LSTMCounterEnsemble::LSTMCounterEnsemble(bool peepholes)

{
	init(peepholes);
}

void LSTMCounterEnsemble::init(bool peepholes)
{
	
	Node* bias = new BiasNode();
	Node* outputgate = new SigmoidNode();
	Node* forgetgate = new SigmoidNode();	
	Node* cell = new LinearNode();
	Node* cell_act = new LinearNode(); //TanhNode();
	Node* forgetmult = new PiNode();
	Node* outputmult = new PiNode();
	
	nodes.push_back(bias);
	nodes.push_back(forgetgate);
	nodes.push_back(outputgate);
	nodes.push_back(forgetmult);
	nodes.push_back(cell);
	nodes.push_back(cell_act);
	nodes.push_back(outputmult);
	
	cell->name = "CEC";
	outputgate->name = "Outputgate";
	outputmult->name = "outputmult";
	forgetgate->name = "Forgetgate";
	forgetmult->name = "Forgetmult";
	
	Connection* c1 = Network::connect(bias, cell, true);
	Connection* c2 = Network::connect(cell, forgetmult, true);
	//Connection* c3 = Network::connect(cell, outputmult, true);
	Connection* c4 = Network::connect(forgetmult, cell, false);
	Connection* c5 = Network::connect(forgetgate, forgetmult, true);
	Connection* c6 = Network::connect(cell, cell_act, true);
	Connection* c7 = Network::connect(cell_act, outputmult, true);
	Connection* c8 = Network::connect(outputgate, outputmult, true);

	b1 = Network::connect(bias, forgetgate, true);
	b2 = Network::connect(bias, outputgate, true);

	b1->weight = +3;
	b2->weight = -2;
	
	c1->set_identity_and_freeze();
	c2->set_identity_and_freeze();
	c4->set_identity_and_freeze();
	c5->set_identity_and_freeze();	
	c6->set_identity_and_freeze();
	c7->set_identity_and_freeze();
	c8->set_identity_and_freeze();
	
	if (peepholes)
	{
		Network::connect(cell, outputgate, false);
		Network::connect(cell, forgetgate, false);
	}

//	std::cout << this->inputs.size() << " " << this->inputs.capacity()<< endl;
//	this->inputs = vector<Node*>();
	
	this->inputs.push_back( outputgate );
	this->inputs.push_back( forgetgate);
	this->outputs.push_back(outputmult);
}

std::vector<Node*>* LSTMCounterEnsemble::get_inputs() {
	return &this->inputs;
}

std::vector<Node*>* LSTMCounterEnsemble::get_outputs() {
	return &this->outputs;
}	

string LSTMCounterEnsemble::to_string()
{
	stringstream out;
	out << (peepholes==true?"1":"0");
	//out << ";" << get_cec()->get_type();
	return out.str();
}

LSTMCounterEnsemble* LSTMCounterEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

    bool peepholes = (atoi(tokens[0].c_str())==1);
     //int cec_type = atoi(tokens[1].c_str());
    LSTMCounterEnsemble* ensemble = new LSTMCounterEnsemble(peepholes);

    return ensemble;
}

LSTMCounterEnsemble* LSTMCounterEnsemble::from_string(string parameters, vector<Node*>* nodes)
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

    LSTMCounterEnsemble* ensemble = new LSTMCounterEnsemble(nodes);
    ensemble->peepholes = peepholes;

    return ensemble;
}
