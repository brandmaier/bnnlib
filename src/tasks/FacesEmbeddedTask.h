/*
 * FacesEmbededTask.h
 *
 *  Created on: Jan 13, 2010
 *      Author: brandmaier
 */

#ifndef FACESEMBEDEDTASK_H_
#define FACESEMBEDEDTASK_H_

#include "../classification/AutoPredictor.h"

struct FacesEmbeddedTask : Task
{
	unsigned int dimension; // embedding dimension
	bool shape_only;
	SequenceSet* embedded_set;

	unsigned out_size;

	FacesEmbeddedTask() : Task("ExperimentFaces/faces-nca.dat", "facesembed.bnn")
	{
		cout << sequence_set->input_size << "->" << sequence_set->target_size << endl;

		sequence_set->normalize();

		shape_only = true;
		dimension = 10;
		embedded_set = sequence_set->embed(dimension);
		cout << "start training" << endl;

		if (shape_only) {
			out_size = 12;
		} else {
			out_size = 38;
		}
	}

	void init()
	{

		unsigned in_size = 71;
		in_size *= dimension;

		unsigned int num_layers = 1;
		unsigned int layer_size[] = {300};

		network = NetworkFactory::createFeedForwardNetwork(in_size, Node::TANH_NODE, num_layers, layer_size, Node::LINEAR_NODE, out_size);

		trainer = new ImprovedRPropTrainer(network);
		trainer->add_callback( new ReportCallback(), 10);
		trainer->add_callback( new SaveNetworkCallback("facesembed"), 50);
	}

	void train()
	{
		trainer->train( embedded_set, 10000);
		network->save(network_name);

		GnuplotGenerator::plot_vectors("Error during training", &trainer->error_train, "training set",
				&trainer->error_validation, "validation set");
	}

	void test()
	{
		/*Sequence* seq = embedded_set->get(0);
		//cout << "out "
		network->activate(seq);
		vector<vector<weight_t>*>* outputs = network->get_outputs_until(seq->size());
		GnuplotGenerator::plot_vector("Original", outputs);
		delete outputs;
*/

		// mapping function
		vector<int> mapping;
		for (unsigned int i=0; i < out_size; i++)
			mapping.push_back(-1);


		for (unsigned int i=0; i < 12; i++)
				mapping[i] = 10+i;

		if (!shape_only) {
			for (unsigned int i=0; i < 26; i++)
				mapping[12+i] = 43+i;
		}

		TransferFunction* transfer_function =
				new MappedEmbeddedTransferFunction(dimension, &mapping);

		AutoPredictor predictor(network,transfer_function);
		//predictor.transfer_function = transfer_function;

		//predictor.predict_and_plot(embedded_set->get(0),1800,1);

		//Jacobian* jacobian = network->jacobian_numerical(embedded_set->get(0), 10e-6);
	}
};

#endif /* FACESEMBEDEDTASK_H_ */
