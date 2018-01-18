/*
 * CascadeCorrelationTrainer.h
 *
 *  Created on: Dec 17, 2009
 *      Author: brandmaier
 */

#ifndef CASCADECORRELATIONTRAINER_H_
#define CASCADECORRELATIONTRAINER_H_

#include "Trainer.h"
#include "../Network.h"
#include "../nodes/Node.h"
#include "../nodes/TanhNode.h"
#include "ImprovedRPropTrainer.h"

class CascadeCorrelationTrainer : Trainer {
public:
	CascadeCorrelationTrainer(Network* network);
	virtual ~CascadeCorrelationTrainer();

	void step(SequenceSet* sequences);
};

#endif /* CASCADECORRELATIONTRAINER_H_ */
