#ifndef LSTMENSEMBLE_
#define LSTMENSEMBLE_

#include "Ensemble.h"
#include "../nodes/Node.h"
#include "../nodes/PiNode.h"
#include "../nodes/LinearNode.h"
#include "../Network.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"

struct LSTMEnsemble : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;
	
	LSTMEnsemble();
	LSTMEnsemble(std::vector<Node*>* nodes);
	LSTMEnsemble(bool peepholes);
	~LSTMEnsemble();
	
	void init(bool peepholes);
	
	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();
	
	Node* get_inputgate();
	Node* get_outputgate();
	Node* get_cec();
	Node* get_output();
	
	bool peepholes;
	
	virtual const unsigned int get_type() { return LSTM_ENSEMBLE; }	
};

#endif /*LSTMENSEMBLE_*/
