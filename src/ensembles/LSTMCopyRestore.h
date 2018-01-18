/*
 * LSTMCopyRestore.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */
#ifndef LSTMCOPYRESTOREENSEMBLE_H_
#define LSTMCOPYRESTOREENSEMBLE_H_

#include "Ensemble.h"
#include "../Connection.h"
#include "../nodes/Node.h"
#include "../nodes/PiNode.h"
#include "../nodes/LinearNode.h"
#include "../Network.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/BiasNode.h"


struct LSTMCopyRestore : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;

	unsigned int cec_activation;

	LSTMCopyRestore(std::vector<Node*>* nodes);
	//LSTMCopyRestoreEnsemble(bool peepholes);
	LSTMCopyRestore(bool peepholes,  int cec_activation);
	LSTMCopyRestore(bool peepholes,  nodetype cec_activation,
			weight_t bias_discount_input, weight_t  bias_discount_forget, weight_t  bias_discount_output);

	~LSTMCopyRestore();

	void init(bool peepholes,  nodetype cec_activation,
			weight_t bias_discount_input, weight_t bias_discount_forget, weight_t bias_discount_output,
			weight_t bias_offset_input, weight_t bias_offset_output, weight_t bias_offset_forget);

	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();

	Node* get_inputgate();
	Node* get_outputgate();
	Node* get_forgetgate();
	Node* get_cec();
	Node* get_output();

	Connection* self;
	Connection* c1;
	Connection* c2;
	Connection* c3;
	Connection *c4;
	Connection *c5;
	Connection *c6;
	Connection *f1, *f2, *f3, *b1, *b2, *b3;

	static int LSTM_COUNTER;

	bool peepholes;

	virtual const unsigned int get_type() { return LSTM_COPYRESTORE_ENSEMBLE; }

	void set_frozen(bool state);

	static LSTMCopyRestore* from_string(string parameters);
	static LSTMCopyRestore* from_string(string parameters, vector<Node*>* nodes);
	string to_string();
};

#endif /*LSTMCopyRestoreEnSEMBLE_H_*/

