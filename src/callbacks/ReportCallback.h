/*
 * ReportCallback.h
 *
 *  Created on: Nov 24, 2009
 *      Author: brandmaier
 */

#ifndef REPORTCALLBACK_H_
#define REPORTCALLBACK_H_

struct ReportCallback : CallbackHandler
{

	void callback(Network* network,Trainer* trainer, SequenceSet* set)
	{

		 double elapsed_time = (time(NULL) - trainer->training_start_time) / 60.0;

		 //weight_t error = this->test_sequence(sequenceset, false) / sequenceset->size();
		 std::cout << "Iteration " << trainer->total_iterations << " (t=" << round(elapsed_time) << "min)";

		 if (trainer->sequenceset == NULL) {
			 cout << " WARNING: No SequenceSet specified in Trainer class" << endl;
			 return;
		 }

		 if (trainer->error_train.size() > 0) {
			std::cout << "MSE: "<< (trainer->get_mean_squared_error() );
			cout << " nrms/sequence: "<< trainer->get_normalized_root_mean_sequence_error();
			cout << " nrms/element: "<< trainer->get_normalized_root_mean_element_error();
			cout << " error%: "<< trainer->get_squared_error_percentage();
			cout << " best mse: " << trainer->lowest_error;
	  	 std::cout << ", total set error :" << network->evaluate_error(set,network->error_function);
		 }


		 std::cout << endl;
		 if (trainer->error_validation.size() > 0) {
			 std::cout << "validation set: " << trainer->error_validation[trainer->error_validation.size()-1] << endl;
		 }
	}


};

#endif /* REPORTCALLBACK_H_ */
