/*
 * LSTMNetwork.h
 *
 *  Created on: Nov 23, 2009
 *      Author: brandmaier
 */

#ifndef LSTMNETWORK_H_
#define LSTMNETWORK_H_

#include "../Network.h"
#include "../nodes/Node.h"
#include "../ensembles/LSTMForgetEnsemble.h"

struct LSTMNetwork : Network
{
	vector<LSTMForgetEnsemble*> lstms;

/*	Ensemble* out;
	Ensemble* in;
*/
	LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size);

	LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
			int lstm_act_type, int out_type,
			weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount
);

	LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
			int lstm_act_type, int out_type);

	~LSTMNetwork();

	void init(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
			int lstm_act_type, int out_type,
			weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount);

	void add_lstm_cell();
	void add_lstm_cell(float in_bias, float forget_bias, float out_bias);


};

#endif /* LSTMNETWORK_H_ */
