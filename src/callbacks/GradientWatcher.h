/*
 * GradientWatcher.h
 *
 *  Created on: Nov 4, 2009
 *      Author: brandmaier
 */

#ifndef GRADIENTWATCHER_H_
#define GRADIENTWATCHER_H_

#include "CallbackHandler.h"
#include "../Network.h"
#include "../SequenceSet.h"

struct GradientWatcher : CallbackHandler
{

	GradientWatcher();

	void callback(Network* net,Trainer* trainer, SequenceSet* set);

};



#endif /* GRADIENTWATCHER_H_ */
