/*
 * WeightWatcher.h
 *
 *  Created on: Oct 15, 2009
 *      Author: brandmaier
 */

#ifndef WEIGHTWATCHER_H_
#define WEIGHTWATCHER_H_

struct WeightWatcher : CallbackHandler
{
	bool watch_all_weights;
	bool watch_derivatives;

	WeightWatcher()
	{
		watch_all_weights = true;
		watch_derivatives = true;
	}

	void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		if (watch_all_weights)
		{
			cout << "Weight Watcher" << endl;
			print_vector(net->get_all_weights());

			print_vector(net->get_all_previous_partial_derivatives());
		}
	};

};

#endif /* WEIGHTWATCHER_H_ */
