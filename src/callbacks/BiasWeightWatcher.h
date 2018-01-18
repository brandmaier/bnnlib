/*
 * BiasWeightWatcher.h
 *
 *  Created on: Oct 7, 2009
 *      Author: brandmaier
 */

#ifndef BIASWEIGHTWATCHER_H_
#define BIASWEIGHTWATCHER_H_

#include "CallbackHandler.h"
#include "../Network.h"
#include "../SequenceSet.h"
#include "../ensembles/LSTMCounterEnsemble.h"

struct BiasWeightWatcher : CallbackHandler
{

	void callback(Network* net, SequenceSet* set)
	{
		cout << "BiasWeightWatcher" << endl;
		vector<Ensemble*>* ensembles = net->get_ensembles_by_type(Ensemble::LSTM_FORGET_ENSEMBLE);
		for (unsigned int i=0; i < ensembles->size(); i++)
		{
			LSTMForgetEnsemble* lstm = (LSTMForgetEnsemble*)(*ensembles)[i];

			cout << "LSTMForget " << i << ": "
					<< (lstm->b1==NULL?0:lstm->b1->weight) << " "
					<< (lstm->b2==NULL?0:lstm->b2->weight)
				    << " " << (lstm->b3==NULL?0:lstm->b3->weight) << endl;
		}

		ensembles = net->get_ensembles_by_type(Ensemble::LSTM_COUNTER_ENSEMBLE);
		for (unsigned int i=0; i < ensembles->size(); i++)
		{
			LSTMCounterEnsemble* lstm = (LSTMCounterEnsemble*)(*ensembles)[i];
			cout << "LSTMCounter " << i << ": " << lstm->b1->weight << " " << lstm->b2->weight
					<< endl;
		}

		//net->export_to_dot_graph();
	}

};

#endif /* BIASWEIGHTWATCHER_H_ */
