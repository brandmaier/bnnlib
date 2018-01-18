/*
 * SineEmbeddedTask.h
 *
 *  Created on: Jan 18, 2010
 *      Author: brandmaier
 */

#ifndef SINEEMBEDDEDTASK_H_
#define SINEEMBEDDEDTASK_H_

struct SineEmbeddedTask : Task
{
	unsigned int out_size;
	unsigned int dimension;
	SequenceSet* embedded_set;


	SineEmbeddedTask() : Task("datasets/pfg-cos.dat","sineembedded.bnn")
	{
		out_size = 1;
		dimension = 5;
		embedded_set = sequence_set->embed(dimension);
		embedded_set->save_to_file("datasets/pfg-cos-embedded.dat");
	}

	void init()
	{
		unsigned in_size = 1;
		in_size *= dimension;

		unsigned int num_layers = 2;
		unsigned int layer_size[] = {20,10};

		network = NetworkFactory::createFeedForwardNetwork(in_size, Node::TANH_NODE, num_layers, layer_size, Node::TANH_NODE, out_size);

		//Ensemble* noise = new FeedforwardEnsemble(Node::UNIFORM_NOISE_NODE, 1);
		//network->

		network->sort_nodes();

		trainer = new ImprovedRPropTrainer(network);
		trainer->add_callback( new ReportCallback(), 100);
		trainer->add_callback( new SaveNetworkCallback("sineembedded"), 200);



	}

	void train()
	{
		cout << "Start training" << endl;
		trainer->train( embedded_set, 10000);
		network->save(network_name);

	}

	void test()
	{
		vector<int> mapping;
		for (unsigned int i=0; i < out_size; i++)
			mapping.push_back(-1);

		//mapping[0] = 0;

		TransferFunction* transfer_function =
				new MappedEmbeddedTransferFunction(dimension, &mapping);

		AutoPredictor predictor(network,transfer_function);
		//predictor.transfer_function = transfer_function;

		predictor.predict_and_plot(embedded_set->get(0),1000,100);

	}

};

#endif /* SINEEMBEDDEDTASK_H_ */
