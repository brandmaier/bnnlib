#ifndef NETWORKFACTORY_H_
#define NETWORKFACTORY_H_

#include "Network.h"
#include "ensembles/SlowSecondDerivativeEnsemble.h"
#include "nodes/MatchingOutputNode.h"

struct NetworkFactory
{

	virtual void i_am_abstract() = 0;
	
	static Network* createTestNetwork1(unsigned int in_size, unsigned int hid_size,unsigned int out_size,bool self_recurrence);
	static Network* createTestNetwork2(unsigned int in_size, unsigned int hid_size,unsigned int out_size);
	static Network* createTestNetwork3(unsigned int in_size, unsigned int hid_size,unsigned int out_size);
	static Network* createTestNetwork4(unsigned int in_size, unsigned int hid_size,unsigned int out_size, bool bias);
	static Network* createTestNetwork5(unsigned int in_size, unsigned int hid_size,unsigned int out_size);
//	static Network* createTestNetwork6(unsigned int in_size, unsigned int hid_size,unsigned int out_size);
	static Network* createTestNetwork7(unsigned int in_size, unsigned int hid_size1, unsigned int hid_size2,unsigned int out_size);
	static Network* createTestNetwork8(unsigned int in_size, unsigned int hid_size, unsigned int out_size);
	static Network* createTestNetwork9(unsigned int in_size, unsigned int lstm_cells, unsigned int hid_size, unsigned int out_size, bool peepholes);
	static Network* createTestNetwork10(unsigned int in_size,	unsigned int keep_ensemble, unsigned int keep_ensemble_size , unsigned int hid_size, unsigned int out_size);
	static Network* createTestNetwork11(unsigned int in_size,	unsigned int lstm_cells , unsigned int hid_size, unsigned int out_size);
	static Network* createTestNetwork13(unsigned int in_size, unsigned int lstm_cells, unsigned int hid_size, unsigned int out_size);

	static Network* createLinLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size);
	static Network* createTanhLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size);
	static Network* createLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int lstm_act_type, unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size,
				weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount,
				bool peepholes,bool bias_to_lstm_input);

	static Network* createExtendedLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int lst_act_type,
	unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int hidden_size, unsigned int out_size);

	static Network* createFeedForwardNetwork(unsigned int in_size,int hid_type,unsigned int num_layers, unsigned int layer_sizes[], int out_type, unsigned int out_size);
	static Network* createFeedForwardNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size);
	static Network* createFeedForwardNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size, int out_type);
	static Network* createFeedForwardWTANetwork(unsigned int in_size, unsigned int layers, unsigned int layer_sizes[], unsigned int out_size);

	static Network* createLSTMWTANetwork(unsigned int lstm_type, unsigned int lst_act_type,
	 unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size
	 ,weight_t in_disc, weight_t forg_disc, weight_t out_disc, bool out_bias);
	
	static Network* createRecurrentNetwork(unsigned int in_size, int hid_type, unsigned int num_layers,unsigned int layer_sizes[], int out_type, unsigned int out_size);
	static Network* createRecurrentWTANetwork(unsigned int in_size, int hid_type, unsigned int num_layers,unsigned int layer_sizes[], unsigned int out_size);

	static Network* createRecurrentNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size);


	static Network* createCLSTM(unsigned int in, unsigned int hid, unsigned int out);

	static Network* create_random_gate_network(unsigned int in_size,
			unsigned int gate_size, unsigned int out_size, double connectivity,
			unsigned int num_backward_connections);

	static Network* create_random_gate_memory_network(unsigned int in_size,
			unsigned int gate_size, unsigned int out_size, double connectivity,
			unsigned int num_backward_connections);


	static Network* create_gers02a();

	static Network* create_2ndorder(unsigned int in_size, unsigned int out_size);
	static Network* create_2ndorder_lstm(unsigned int in_size, unsigned int lstm_size, unsigned int out_size);

	static Network* create_tsin(unsigned int in_size, unsigned int hid_size, unsigned int out_size);

	static Network* create_harmonic_generator(unsigned int generators, unsigned int hid_size, unsigned int out_size);

	static Network* create_stacked_slow(unsigned int in_size, unsigned int num_layers, 
		 unsigned int hid_size[], double hid_slow[], unsigned int out_size);

	static Network* create_slow_feature_network(unsigned int in_size,unsigned int hid_size, unsigned int out_size);
	static Network* create_slow_feature_network_v2(unsigned int in_size,unsigned int hid_size, unsigned int out_size);

	static Network* createCopyRestoreLSTM(unsigned int in_size, unsigned int cells, unsigned int out_size);

	
	static Network* createSparseAutoencoder(unsigned int in_size, unsigned int hidden_size, double sparsity);
	static Network* createSparseAutoencoder(unsigned int in_size, unsigned int hidden_size, double sparsity, int hid_type, int out_type);
	
};

#endif /*NETWORKFACTORY_H_*/
