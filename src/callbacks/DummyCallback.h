/*
 * DummyCallback.h
 *
 *  Created on: Nov 4, 2009
 *      Author: brandmaier
 */

#ifndef DUMMYCALLBACK_H_
#define DUMMYCALLBACK_H_

#include "CallbackHandler.h"

struct DummyCallback : CallbackHandler
{

	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		// noop
	}

};

#endif /* DUMMYCALLBACK_H_ */
