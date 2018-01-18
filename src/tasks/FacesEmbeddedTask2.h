/*
 * FacesEmbededTask.h
 *
 *	Create an embedding of the original time series and a matching
 *	feedforward network with Rprop+.
 *
 *
 *  Created on: Jan 18, 2012
 *      Author: brandmaier
 */

#ifndef FACESEMBEDEDTASK2_H_
#define FACESEMBEDEDTASK2_H_

#include "../classification/AutoPredictor.h"

struct FacesEmbeddedTask2 : Task
{
	unsigned int dimension; // embedding dimension
	bool shape_only;
	SequenceSet* embedded_set;
	SequenceSet* embedded_validation_set;

	unsigned out_size;

	FacesEmbeddedTask2() : Task("ExperimentFaces/faces1-4.dat","ExperimentFaces/faces5.dat", "facesembed.bnn")
	{
		cout << "Train Faces" << endl;
		cout << sequence_set->input_size << "->" << sequence_set->target_size << endl;

		sequence_set->normalize();
		validation_set->normalize();

//shape_only = true;
		dimension = 10;
		embedded_set = sequence_set->embed(dimension);
		embedded_validation_set = validation_set->embed(dimension);
		embedded_validation_set->set_all_as_validation();

		//cout << embedded_validation_set->size() << " und " << embedded_set->size() << endl;
		//cout << embedded_validation_set->get(0)->size() << "by"<<embedded_set->get(0)->size()  << embedded_set->get(1)->size()  << embedded_set->get(2)->size() << embedded_set->get(3)->size() << endl;
		//return;
	//	if (shape_only) {
		//	out_size = 12;
		//} else {
			out_size = 31;
		//}
	}

	void init()
	{

		unsigned in_size = 71;
		in_size *= dimension;

		unsigned int num_layers = 1;
		unsigned int layer_size[] = {200};

		network = NetworkFactory::createFeedForwardNetwork(in_size, Node::TANH_NODE, num_layers, layer_size, Node::LINEAR_NODE, out_size);

		trainer = new ImprovedRPropTrainer(network);
		trainer->add_callback( new ReportCallback(), 1);
		trainer->add_callback( new SaveNetworkCallback("facesembed"), 50);

		trainer->add_abort_criterion(new GeneralizationLoss(embedded_validation_set), 10);
	}

	void train()
	{
		trainer->train( embedded_set, embedded_validation_set, 10000);
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

		predictor.predict_and_plot(embedded_set->get(0),1700,1);

		//Jacobian* jacobian = network->jacobian_numerical(embedded_set->get(0), 10e-6);
	}
};

#endif /* FACESEMBEDEDTASK2_H_ */
