/*
 * SaveNetworkCallback.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */

#ifndef SAVENETWORKCALLBACK_H_
#define SAVENETWORKCALLBACK_H_

#include "CallbackHandler.h"

struct SaveNetworkCallback : CallbackHandler
{
	string basefilename;
	bool incremental;

	SaveNetworkCallback(string name)
	{
		basefilename = name;
		incremental = false;
	}

	SaveNetworkCallback(string name, bool incremental)
	{
		basefilename = name;
		this->incremental = incremental;
	}


	void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		message("saving network");
		stringstream filename;
		if (incremental)
			filename << basefilename << "-epochs" << trainer->total_iterations << ".bnn";
		else
			filename << basefilename << ".bnn";
		net->save( filename.str() );

	}

};

#endif /* SAVENETWORKCALLBACK_H_ */
