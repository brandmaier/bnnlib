#ifndef CALLBACKHANDLER_H_
#define CALLBACKHANDLER_H_

#include "../Network.h"
#include "../SequenceSet.h"
#include "../trainer/Trainer.h"

struct Network;
struct Trainer;

struct CallbackHandler
{
	CallbackHandler() {};
	virtual ~CallbackHandler() {};
	
	virtual void callback(Network* net, Trainer* trainer, SequenceSet* set) = 0;
	
};

#endif /*CALLBACKHANDLER_H_*/
