/*
 * TimeseriesClassification.h
 *
 *  Created on: Dec 18, 2009
 *      Author: brandmaier
 */

#ifndef TIMESERIESCLASSIFICATION_H_
#define TIMESERIESCLASSIFICATION_H_

#include "../GnuplotGenerator.h"
#include "../Network.h"
#include "../Sequence.h"

struct TimeseriesClassification
{
	vector<unsigned int> classification;
	Network* network;
	Sequence* sequence;

	static const string colors[10];

	TimeseriesClassification(Network* network) {
		this->network = network;


	}

	void classify(Sequence* sequence)
	{
		classification.clear();

		this->sequence = sequence;
		network->activate(sequence);
		for (unsigned int i=0; i < sequence->size(); i++)
		{
			vector<weight_t>* output = network->get_output( network->timestep-sequence->size()+i);
			unsigned int max_index=0;
			for (unsigned int j=1; j < output->size(); j++)
			{
				if ((*output)[max_index] < (*output)[j])
				{
					max_index = j;
				}
			}
			classification.push_back(max_index);
			delete output;
		}
	}

	void plot()
	{

//		print_vector( sequence->target[0] );

		unsigned int total = classification.size();
		unsigned int correct = 0;

		// compact representation
		vector<unsigned int> positions;
		vector<string> shades;

		unsigned int lastclass = classification[0];
		positions.push_back( 0 );
		shades.push_back( colors[classification[0]] );
		cout << classification[0] << endl;
		for (unsigned int i=0; i < classification.size(); i++)
		{
			if (lastclass != classification[i]) {
				positions.push_back( i );
				shades.push_back( colors[classification[i]]);
				lastclass = classification[i];
			}

			if (classification[i] == sequence->get_maximum_index(i))
				correct++;
		}

		//print_vector( &classification );

		stringstream title;

		double acc = correct*100.0/total;

		title << "Classification: " << acc << "%";

		GnuplotGenerator::plot_shaded_curve(title.str(), &sequence->input,
				&positions, &shades);
	}

};



#endif /* TIMESERIESCLASSIFICATION_H_ */
