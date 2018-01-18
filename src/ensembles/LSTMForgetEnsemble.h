#ifndef LSTMFORGETENSEMBLE_H_
#define LSTMFORGETENSEMBLE_H_

#include "Ensemble.h"
#include "../Connection.h"
#include "../nodes/Node.h"
#include "../nodes/PiNode.h"
#include "../Network.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/BiasNode.h"
#include "../nodes/LinearNode.h"
#include "../nodes/ScaledSigmoidNode.h"
#include "../nodes/SteepSigmoidNode.h"
#include "../nodes/ScaledTanhNode.h"

struct LSTMForgetEnsemble : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;

	unsigned int cec_activation;

	LSTMForgetEnsemble(std::vector<Node*>* nodes);	
	//LSTMForgetEnsemble(bool peepholes);
	LSTMForgetEnsemble(bool peepholes,  int cec_activation);	
	LSTMForgetEnsemble(bool peepholes,  nodetype cec_activation,
			weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight);
	LSTMForgetEnsemble(bool peepholes, nodetype input_activation, nodetype cec_activation,
			weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight);


	~LSTMForgetEnsemble();

	void init(bool peepholes, nodetype input_activation, nodetype cec_activation,
			weight_t bias_input_weight, weight_t bias_forget_weight, weight_t bias_output_weight);

	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();

	Node* get_inputgate();
	Node* get_outputgate();
	Node* get_forgetgate();
	Node* get_cec();
	Node* get_output();
	Node* get_input();
	
	Connection* self;
	Connection* c1;
	Connection* c2;
	Connection* c3;
	Connection *c4;
	Connection *c5;
	Connection *c6;
	Connection *f1, *f2, *f3, *b1, *b2, *b3;
	Connection *cp1, *cp2, *cp3;

	static int LSTM_COUNTER;
	
	bool peepholes;

	void set_peepholes(weight_t in, weight_t hid, weight_t out)
	{
		if ((cp1 == NULL) || (cp2 == NULL) or (cp3 == NULL))
		{
			error("LSTM cell has no peepholes!");
		}
		cp1->weight = in;
		cp2->weight = hid;
		cp3->weight = out;
	}
	
	virtual const unsigned int get_type() { return LSTM_FORGET_ENSEMBLE; }	
	
	void set_frozen(bool state);
	
	static LSTMForgetEnsemble* from_string(string parameters);
	static LSTMForgetEnsemble* from_string(string parameters, vector<Node*>* nodes);
	string to_string();

	void open_mults_to_public();
};

#endif /*LSTMFORGETENSEMBLE_H_*/
