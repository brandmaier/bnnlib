/*
 * CorrelationDerivatives.cpp
 *
 *  Created on: Dec 23, 2009
 *      Author: brandmaier
 */

#include "CorrelationDerivatives.h"
#include <cmath>

CorrelationDerivatives::CorrelationDerivatives(Network* network, vector<Node*>* nodes) {
	this->network = network;
	this->nodes = nodes;
}

CorrelationDerivatives::~CorrelationDerivatives() {
	// TODO Auto-generated destructor stub
}


void CorrelationDerivatives::calculate_derivatives(double alpha)
{
	//assert(network->timestep > 0);
	double E = 0.0;

	// find mean activations
	vector<double> mean(nodes->size());
//	vector<double> d(nodes->size());

	unsigned int T = network->timestep;

	//cout << "T=" << T << endl;

	for (unsigned int i=0; i < nodes->size(); i++)
	{
		mean[i] = 0.0;
	//	d[i] = 0.0;
	}

	for (unsigned int t=0; t < T; t++)
	{
		for (unsigned int i=0; i < nodes->size(); i++)
		{
			mean[i] += (*nodes)[i]->actbuf[t];
		}
	}

	for (unsigned int i=0; i < nodes->size(); i++) {
		mean[i] /= T;
		//if (isnan(mean[i])) error("MEAN NAN!");
	}



	for (unsigned int i=0; i < nodes->size(); i++)
		for (unsigned int j=0; j < nodes->size(); j++)
		{
	 	 for (unsigned int t=0; t < T; t++)

		 {

			if (i==j)
				{

				double c = 0.0;
				double correlation = 0.0;
				for (unsigned int u=0; u < T; u++)
				{
					if (u != t) {
						c+= (-1.0/T)*( (*nodes)[i]->actbuf[u]-mean[i] );
					} else {
						c+= (1-(1.0/T)) * ( (*nodes)[i]->actbuf[u]-mean[i] );
					}

					correlation+=( (*nodes)[i]->actbuf[u]-mean[i] ) * ( (*nodes)[j]->actbuf[u]-mean[j] );

				}

				double signum = correlation>0?+1:-1;

				// hier umgekehrtes Vrz!
				(*nodes)[i]->delta[t] += -(0.5/(T-1))*signum*alpha*(*nodes)[i]->get_activation_function_derivative(t) * c;


				continue;
				}

			double corr=( (*nodes)[i]->actbuf[t]-mean[i] ) * ( (*nodes)[j]->actbuf[t]-mean[j] );
			E+=corr;

			double c = 0.0;
			double correlation = 0.0;
			for (unsigned int u=0; u < T; u++)
			{
				if (u != t) {
					c+= (-1.0/T)*( (*nodes)[j]->actbuf[u]-mean[j] );
				} else {
					c+= (1-(1.0/T)) * ( (*nodes)[j]->actbuf[u]-mean[j] );
				}

				correlation+=( (*nodes)[i]->actbuf[u]-mean[i] ) * ( (*nodes)[j]->actbuf[u]-mean[j] );

			}



			double signum = correlation>0?+1:-1;

			(*nodes)[i]->delta[t] += (0.5/(T-1))*signum*alpha*(*nodes)[i]->get_activation_function_derivative(t) * c;
			//cout << (*nodes)[i]->delta[t] << endl;
			if (isnan((*nodes)[i]->delta[t] )) {
				error("NAN in CD");
			}
			//cout << "sum:" << sum << ", sum2=" << sum2 << " " << mean[i] << " und " << mean[j] << " d[i]="<< d[i] << " T=" << T << endl;
		}
	}
	/*for (unsigned int i=0; i < nodes->size(); i++)
	{
		//cout << d[i] << "\t";
		(*nodes)[i]->delta += d[i];
	}*/
	//cout << "\n";
	//cout << "T="<<T<<endl;
	//cout << "Error: " << (0.5/(T-1))*E << ", "<<E<< endl;
	//cout << "mean " << mean[0] << ", " << mean[1] << endl;
}
