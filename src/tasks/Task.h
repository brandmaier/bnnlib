/*
 * Task.h
 *
 *  Created on: Jan 7, 2010
 *      Author: brandmaier
 */
#ifndef TASK_H_
#define TASK_H_

struct Task
{

	string sequenceset_name;
	string validationset_name;
	string network_name;

	Network* network;
	SequenceSet* sequence_set;
	SequenceSet* validation_set;
	Trainer* trainer;

	Task() {}
	virtual ~Task() {};

	Task(string sequenceset_name, string network_name)
	{
		this->sequenceset_name = sequenceset_name;
		this->network_name = network_name;

		sequence_set = new SequenceSet(sequenceset_name);
	}

	Task(string sequenceset_name, string validationset_name, string network_name)
	{
		this->sequenceset_name = sequenceset_name;
		this->validationset_name = validationset_name;
		this->network_name = network_name;

		sequence_set = new SequenceSet(sequenceset_name);
		validation_set = new SequenceSet(validationset_name);

		validation_set->set_all_as_validation();
	}

	virtual void init() = 0;
	virtual void restore() {
		//sequence_set = new SequenceSet(sequenceset_name);
		network = new Network(network_name);
	}
	virtual void train() = 0;
	virtual void test() = 0;

};

#endif
