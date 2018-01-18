#ifndef LSTMCOUNTERENSEMBLE_
#define LSTMCOUNTERENSEMBLE_

#include "Ensemble.h"
#include <vector>
#include "../Connection.h"
#include "../nodes/Node.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/LinearNode.h"
#include "../nodes/BiasNode.h"
#include "../nodes/PiNode.h"
#include "../Network.h"

struct LSTMCounterEnsemble : Ensemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;
	
	Connection *b1, *b2;

	bool peepholes;

	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();
	
	LSTMCounterEnsemble();
	LSTMCounterEnsemble(bool peepholes);
	LSTMCounterEnsemble(std::vector<Node*>* nodes);
	~LSTMCounterEnsemble() {};
	
	void init(bool peepholes);

	virtual const unsigned int get_type() { return LSTM_COUNTER_ENSEMBLE; }
	
	static LSTMCounterEnsemble* from_string(string parameters);
	static LSTMCounterEnsemble* from_string(string parameters, vector<Node*>* nodes);
	string to_string();

};

#endif /*LSTMCOUNTERENSEMBLE_*/
