/*
 * ZeroErrorFailReport.h
 *
 *  Created on: Nov 27, 2009
 *      Author: brandmaier
 */

#ifndef ZEROERRORFAILREPORT_H_
#define ZEROERRORFAILREPORT_H_

#include "CallbackHandler.h"

struct ZeroErrorFailCallback : CallbackHandler
{
	ErrorFunction* error_function;

	ZeroErrorFailCallback(ErrorFunction* error_function) {
		this->error_function = error_function;
	}

	void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		unsigned int num_failed_sequences = 0;
		stringstream report;
		double error = 0.0;

		for (unsigned int k=0; k < set->size(); k++)
		{
			Sequence* sequence = (*set)[k];

			double sequence_error = 0.0;

			net->reset();

			unsigned int start = sequence->training_begin;
			unsigned int end = sequence->training_end;

			net->reserve( sequence->size() );

			for (unsigned int i=start; i < end; i++)
			{
				//if (verbose)
				// std::cout << "Step # "<< i << " (network t = "<< (timestep) << ")\n";
				net->_activate( sequence->get_input(i) );
				std::vector<weight_t>* output = net->get_output(net->timestep-1);
				std::vector<weight_t>* target = sequence->get_target(i);

				/*for (unsigned int j=0; j < output->size(); j++)
				{
					if (!isnan((*target)[j])) {
						double temperror = this->error_function->get_error(
								(*target)[j], (*output)[j]
						);
						sequence_error+=temperror;

					}

				}*/
				sequence_error+= this->error_function->get_error(target, output);

				delete output;
			}


			if (sequence_error > 0) {
				num_failed_sequences++;
				report << " " << k;
				//report << " of sequence #"<<k<<" " << endl;
			}

			error+=sequence_error;
		}

		cout << "report: #failed seqs" << (num_failed_sequences)
				<< "/" << set->size() << ":" << report.str() << endl;
	}

};

#endif /* ZEROERRORFAILREPORT_H_ */
