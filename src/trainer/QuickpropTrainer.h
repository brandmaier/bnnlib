/*
 * QuickpropTrainer.h
 *
 *  Created on: Dec 17, 2009
 *      Author: brandmaier
 */

#ifndef QUICKPROPTRAINER_H_
#define QUICKPROPTRAINER_H_

#include "Trainer.h"
#include "../Trainable.h"
#include "../Network.h"

class QuickpropTrainer: public Trainer {
public:
	QuickpropTrainer(Network* network);
	virtual ~QuickpropTrainer();

	void change_weight(Trainable* conn);

	virtual string get_name()
	{
		return "QuickProp";
	}
};

#endif /* QUICKPROPTRAINER_H_ */
