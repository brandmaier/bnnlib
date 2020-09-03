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

struct CascadeCorrelationTrainer : public Trainer {

	CascadeCorrelationTrainer(Network* network);
	virtual ~CascadeCorrelationTrainer();
	
	void change_weight(Trainable* c) {
	  
	};

	void step(SequenceSet* sequences, unsigned int steps);
	
	virtual string get_name()
	{
	  return "CascadeCorrelation";
	}
};

#endif /* CASCADECORRELATIONTRAINER_H_ */
