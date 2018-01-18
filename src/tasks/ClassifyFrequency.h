/*
 * ClassifyFrequency.h
 *
 * The network has to learn to discern between sine waves
 * at different frequencies
 *
 *  Created on: Jan 8, 2010
 *      Author: brandmaier
 */

#ifndef CLASSIFYFREQUENCY_H_
#define CLASSIFYFREQUENCY_H_

#include "../classification/TimeseriesClassification.h"
#include "../classification/ConfusionMatrix.h"
#include "../callbacks/ErrorEvaluationCallback.h"
#include "../networks/SlowNetwork.h"
#include "../misc/ParameterSet.h"
#include "../errorfunctions/WinnerTakesAllErrorFunction.h"
#include "../callbacks/ClassificationCallback.h"

struct ClassifyFrequencyTask : Task
{
	unsigned num_freqs;

	DataSet* dataset;

	ParameterSet parameter_set;

	ClassifyFrequencyTask() : Task("datasets/classify_frequency2.dat","cf.bnn")
	{
		num_freqs = 3;
		parameter_set.set("slowness", 0.00);
		dataset = sequence_set->toDataSet(0.5,0.25);

		//srand(1264971150);
	}

		void init()
		{
			unsigned int in_size = 1;
			unsigned int hid_size = 10;
			unsigned int out_size = num_freqs;

			stringstream msg;
			msg << "Slowness at " << parameter_set.get_double("slowness");
			message(msg.str());

			network = new LSTMNetwork(in_size,hid_size,out_size
					);

			//srand(2358954);
			// 0.001 vs 0.000 so cool! srand=2358954
			//network = new SlowNetwork(in_size, 10, out_size, parameter_set.get_double("slowness"));
			//unsigned int hid_size[] = {10,10};
			//unsigned int hid_slow[] = {0.1,1};
			//network = NetworkFactory::create_stacked_slow(in_size, 2, hid_size, hid_slow, out_size);

			//network = new LSTMNetwork(in_size, 10, out_size);

//			network->add_wta_output_ensemble( network->ensembles[1], network->ensembles[network->ensembles.size()-1] );


			trainer = //new BackpropTrainer(network);
					new ImprovedRPropTrainer(network);
			trainer->learning_rate = 0.0001;
			trainer->add_callback( new ReportCallback(), 100);
			trainer->add_callback( new SaveNetworkCallback("cf"), 50);

//		ErrorFunction* err_func = new WinnerTakesAllErrorFunction();
//			network->error_function = err_func;
//			trainer->add_callback( new ErrorEvaluationCallback(err_func, dataset->validation_set), 10);
			trainer->add_callback( new ClassificationCallback(dataset->validation_set), 10);

			trainer->add_abort_criterion(
					new GeneralizationLoss(dataset->validation_set,5),10
			);
			trainer->add_abort_criterion(
					new ConvergenceCriterion(),1
			);

		}

		void train()
		{
			trainer->train( dataset->training_set, dataset->validation_set, 500);
			network->save(network_name);

			stringstream title;
			title << "Error during training" << parameter_set.get_double("slowness");

			GnuplotGenerator::plot_vectors(title.str(), &trainer->error_train, "training set",
					&trainer->error_validation, "validation set");

			ConfusionMatrix cm(network, dataset->test_set);
			cm.create();
		}

		void test()
		{
			ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.5);
			network->error_function = err_func;
			//network->test_sequence( sequence_set, true );

			// choose Sequence
			Sequence* sequence = dataset->test_set->get(2);

			// plot activations over time
			network->activate( sequence );

			// generate animation
				vector<Node*> outputs;
				for (unsigned int i=0; i < network->out_size; i++) {
					outputs.push_back( network->nodes[network->out_offset+i] );
				}
				vector<vector<weight_t>*>* data = network->collect_node_outputs(outputs);
				GnuplotGenerator::animate(data, &sequence->input, 1);

			// plot shaded classification visualization
				//unsigned int time = sequence->size();

				vector<weight_t>* entropy = new vector<weight_t>();
				for (unsigned int i=0; i < data->size(); i++)
				{
					double p = 0.0;

					vector<weight_t>* temp = (*data)[i];
					for (unsigned int j=0; j < temp->size(); j++)
					{
						double value = (*temp)[j];
						p-= value*log(value);
					}

					entropy->push_back(p);
				}

				GnuplotGenerator::plot_vector("Entropy", entropy);

				//GnuplotGenerator::plot_shaded_curve(sequence->get_name(), network->get_outputs_until(time),pos, shades);
				TimeseriesClassification tc(network);

				tc.classify(sequence);
				tc.plot();

				ConfusionMatrix cm(network, dataset->test_set);
				cm.create();
				cm.create_with_entropythreshold(0.3);
		}
};

#endif /* CLASSIFYFREQUENCY_H_ */
