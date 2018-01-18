/*
 * DebugCallback.h
 *
 *  Created on: Oct 22, 2009
 *      Author: brandmaier
 */

#ifndef DEBUGCALLBACK_H_
#define DEBUGCALLBACK_H_

#include "CallbackHandler.h"
#include "../functions.h"

struct DebugCallback : CallbackHandler
{

	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		/*cout << "Debug Callback" << endl;
		print_vector( net->get_all_weights() );
		cout << "*"<<endl;
		print_vector( net->get_all_previous_partial_derivatives() );
		cout << endl;
		 */
		/*for (unsigned int i=0; i < 100; i++) {
			cout << "output #" << i << endl;
			print_vector(net->get_output(i));
		}*/
		print_vector( net->get_all_initialstates() );
	}

};

#endif /* DEBUGCALLBACK_H_ */
