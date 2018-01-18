#ifndef KEEPENSEMBLE_H_
#define KEEPENSEMBLE_H_

#include "../nodes/Node.h"
#include <vector>

#include "../nodes/BiasNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/PiNode.h"
#include "../Network.h"


struct KeepEnsemble : Ensemble
{
	
	KeepEnsemble(unsigned int size);
	~KeepEnsemble() {};
	
	virtual std::vector<Node*>* get_inputs();
	virtual std::vector<Node*>* get_outputs();
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;
	
	static unsigned int KEEP_COUNTER;
	
};

#endif /*KEEPENSEMBLE_H_*/
