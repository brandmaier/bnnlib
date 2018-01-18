#ifndef __LSTMPLUSENSEMBLE_H_
#define __LSTMPLUSENSEMBLE_H_

#include <vector>
#include "../nodes/Node.h"
#include "Ensemble.h"

struct LSTMPlusEnsemble : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;

	unsigned int cec_activation;
	
	Connection* self;
	Connection* c1;
	Connection* c2;
	Connection* c3;
	Connection *c4;
	Connection *c5;
	Connection *c6;
	Connection *f1, *f2, *f3, *b1, *b2, *b3;

	LSTMPlusEnsemble(bool peepholes,  int cec_activation,weight_t bias_discount_input, weight_t  bias_discount_forget, weight_t  bias_discount_output);
	LSTMPlusEnsemble(bool peepholes,  int cec_activation);

	LSTMPlusEnsemble(std::vector<Node*>* nodes);
	~LSTMPlusEnsemble();

	void init(bool peepholes,  nodetype cec_activation,
			weight_t bias_discount_input, weight_t bias_discount_forget, weight_t bias_discount_output,
			weight_t bias_offset_input, weight_t bias_offset_output, weight_t bias_offset_forget);

	
	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();
	
	static int LSTM_COUNTER;
	
	bool peepholes;
	
	Node* get_inputgate();
	Node* get_outputgate();
	Node* get_forgetgate();
	Node* get_cec();
	Node* get_output();

	static LSTMPlusEnsemble* from_string(string parameters);
	static LSTMPlusEnsemble* from_string(string parameters, vector<Node*>* nodes);
	string to_string();
};

#endif /*LSTMPLUSENSEMBLE_H_*/
