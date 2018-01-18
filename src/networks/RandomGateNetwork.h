/*
 * RandomGateNetwork.h
 *
 *  Created on: 07.04.2010
 *      Author: andreas
 */

#ifndef RANDOMGATENETWORK_H_
#define RANDOMGATENETWORK_H_

#include "../Network.h"
#include "../ensembles/LinearMemoryEnsemble.h"

struct RandomGateNetwork : Network
{
	RandomGateNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size)
	{
		init(in_size, hid_size, out_size, 1.0);
	}

	void init(unsigned int in_size, unsigned int hid_size, unsigned int out_size, double connectivity)
	{

		input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
		output_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
		FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

		add_ensemble(input_ensemble);
		add_ensemble(bias);


		for (unsigned int k=0; k < hid_size; k++)
		{

			Ensemble* ge;

			if (k%2==0)
				ge = new GateEnsemble();
			else
				ge = new LinearMemoryEnsemble(1);


			connect_ensembles(input_ensemble, ge, true);
			connect_ensembles(bias, ge, true);

			// connect other Ensembles to new Ensemble with connectivity factor
			unsigned int c = 0;
			for (unsigned int i=2; i < ensembles.size(); i++)
			{
				if (rand_uniform_01() <= connectivity) {
					connect_ensembles( ensembles[i], ge, true);
					c++;
	//				cout << "connect " << i << " to " << "current hidden " << k << endl;

				}
			}
	//		cout << "connected " << c << endl;

			add_ensemble(ge);
			connect_ensembles(ge, output_ensemble, true);

		}
		add_ensemble(output_ensemble);

		size = nodes.size();

	//	sanity_check(true);
		//cout << "nodes: " << nodes.size() << endl;
		//if (has_cycle())
		//	error("Network has cycle!");


		for (unsigned int i=2; i < ensembles.size()-1; i++)
		{
			for (unsigned int j=2; j < ensembles.size()-1; j++)
			{
				if (i > j)	// IMPORTANT XXX BUG hier war >= und alles war gut!
				connect_ensembles( ensembles[i],
						ensembles[j],false);
			}
		}


		sort_nodes(input_ensemble, output_ensemble);
	}

};

#endif /* RANDOMGATENETWORK_H_ */
