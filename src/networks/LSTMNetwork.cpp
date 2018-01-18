/*
 * LSTMNetwork.cpp
 *
 *  Created on: Nov 23, 2009
 *      Author: brandmaier
 */
#include "LSTMNetwork.h"
#include "../ensembles/Ensemble.h"
#include <vector>
#include "../ensembles/FeedforwardEnsemble.h"
#include "../ensembles/LSTMForgetEnsemble.h"
#include "../ensembles/LSTMPlusEnsemble.h"
#include "../ensembles/LSTMCounterEnsemble.h"
#include "../ensembles/LSTMEnsemble.h"
#include "../nodes/Node.h"

LSTMNetwork::LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size)
{
	init(in_size, lstm_size, out_size, Node::TANH_NODE, Node::TANH_NODE, 0,0,0);
}

LSTMNetwork::LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
		int lstm_act_type, int out_type,
		weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount)
{
init(in_size, lstm_size, out_size, lstm_act_type, out_type,
		input_bias_discount, forget_bias_discount, output_bias_discount);
}

LSTMNetwork::LSTMNetwork(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
		int lstm_act_type, int out_type)
{
init(in_size, lstm_size, out_size, lstm_act_type, out_type,
		0,0,0);
}

void LSTMNetwork::init(unsigned int in_size, unsigned int lstm_size, unsigned int out_size,
		int lstm_act_type, int out_type,
		weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount)
{

	bias_size = 1;

	bool bias_to_lstm_input = false;
	bool peepholes = true;
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE,in_size);
	 lstms = vector<LSTMForgetEnsemble*>(lstm_size);
	for (unsigned int i=0; i < lstm_size; i++) {

			 lstms[i] = new LSTMForgetEnsemble( peepholes, lstm_act_type,
					 (i+1)*input_bias_discount, (i+1)*forget_bias_discount,
					 (i+1)*output_bias_discount );


		hid_size += lstms[i]->nodes.size();
	}
	this->in_size = in_size;
	this->out_size = out_size;
	this->size = this->in_size+this->hid_size+this->out_size+1;


	in_offset = 1;
	hid_offset = in_offset+in_size;
	out_offset = hid_offset+hid_size;
	bias_offset = 0;

	output_ensemble = new FeedforwardEnsemble(out_type, out_size);

	add_ensemble( bias );
	add_ensemble( input_ensemble );
	for (unsigned int i=0; i < lstm_size; i++)
		add_ensemble( lstms[i] );

	add_ensemble( output_ensemble );



	connect_ensembles(bias, output_ensemble, true);
	//network->connect_ensembles(in, out, true);


	for (unsigned int i=0; i < lstm_size; i++) {

		if (bias_to_lstm_input)
		 connect( (bias->nodes[0]), (lstms[i])->get_input() , true);

		connect_ensembles(input_ensemble, lstms[i], true);
		connect_ensembles(lstms[i], output_ensemble, true);
			for (unsigned int j=0; j < lstm_size; j++) {
				//if (i != j)
				 connect_ensembles(lstms[i], lstms[j], false );
			}
	}


	sort_nodes();
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;

}

LSTMNetwork::~LSTMNetwork()
{

}

void LSTMNetwork::add_lstm_cell()
{
	add_lstm_cell(0.0,0.0,0.0);
}

void LSTMNetwork::add_lstm_cell(float in_bias, float forget_bias, float out_bias)
{
	LSTMForgetEnsemble* lstm = new LSTMForgetEnsemble(true, Node::TANH_NODE, in_bias, forget_bias, out_bias);
	add_ensemble(lstm);
	connect_ensembles(input_ensemble, lstm, true);
	for (unsigned int i=0;i < lstms.size();i++)
	{
		connect_ensembles(lstms[i], lstm,true);
	}
	connect_ensembles(lstm, output_ensemble, true);
	sort_nodes();
}

