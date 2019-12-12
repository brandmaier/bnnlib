/*
 * HarmonicGeneratorEnsemble.cpp
 *
 *  Created on: Jan 13, 2010
 *      Author: brandmaier
 */
#include "HarmonicGeneratorEnsemble.h"
#include <cmath>

	void HarmonicGenerator::init()
	{
		outputs.push_back( nodes[nodes.size()-1] );
		//inputs.push_back( nodes[0] );

		nodes[nodes.size()-1]->name = "hgout";

		for (unsigned int i=0; i < nodes.size();i++)
			inputs.push_back( nodes[i] );


		// self connections
		for (unsigned int i=0; i < nodes.size(); i++)
		{
			Connection* c3 = Network::connect(nodes[i], nodes[i], false);
			//c3->weight = 0.01;
		}

    // ring connection
		for (unsigned int i=0; i < nodes.size()-1; i++)
		{
			Connection* c1 = Network::connect(nodes[i], nodes[i+1], false);
			//c1->set_identity_and_freeze();
		}
		Connection* c2 = Network::connect(nodes[nodes.size()-1], nodes[0], false);
		//c2->set_identity_and_freeze();
	}
