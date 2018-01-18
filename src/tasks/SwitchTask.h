/*
 * SwitchTask.h
 *
 *  Created on: Jan 8, 2010
 *      Author: brandmaier
 */

#ifndef SWITCHTASK_H_
#define SWITCHTASK_H_

struct SwitchTask : Task
{

	SwitchTask() : Task("datasets/switch.dat","switch.bnn");

};

#endif /* SWITCHTASK_H_ */
