
#ifndef PROGRESSDOT_H_
#define PROGRESSDOT_H_

#include "CallbackHandler.h"

struct ProgressDot : CallbackHandler
{

	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		std::cout << ".";
		std::cout.flush();
	}

};

#endif /* DUMMYCALLBACK_H_ */
