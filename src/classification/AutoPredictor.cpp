/*
 * Autopredict.cpp
 *
 *  Created on: Dec 8, 2009
 *      Author: brandmaier
 */

#include "AutoPredictor.h"
#include "../GnuplotGenerator.h"

AutoPredictor::~AutoPredictor()
	{
	/*	if (input != NULL)
		{
			for (unsigned int i=0; i < input->size(); i++)
				delete (*input)[i];

			delete input;
		}*/
	}

AutoPredictor::AutoPredictor(Network* network)
{
	this->network = network;

	int* mapping = NULL;

	// if no mapping
	//if (!has_mapping) {
	 if ((network->in_size > 0) && (network->in_size != network->out_size))
	 {
		 error("AutoPredictor needs explicit mapping if network input size is not equal to network output size!");
	 } else {
		mapping = new int[network->out_size];
		for (unsigned int i=0; i < network->out_size; i++)
		{
			mapping[i] = i;
		}
		//has_mapping = true;
	 }
	//}

	if (network->in_size == 0)
	{
		for (unsigned int i=0; i < network->out_size; i++)
			mapping[i] = -1;
	}

	TransferFunction* transfer_function = new MappedTransferFunction(mapping);
	this->transfer_function = transfer_function;
}

AutoPredictor::AutoPredictor(Network* network, TransferFunction* transfer_function)
{

	this->network = network;
	this->transfer_function = transfer_function;
}

AutoPredictor::AutoPredictor(Network* network, int* mapping)
{

	this->network = network;
	this->transfer_function = new MappedTransferFunction(mapping);
}

double AutoPredictor::predict_and_plot(Sequence* sequence, unsigned int timesteps, 	unsigned int context_length)
{

	bool with_target = true;

	vector<vector<weight_t>*>* result = predict(sequence, timesteps, context_length);
//	GnuplotGenerator::plot_vector("Generalization", result,"prediction", sequence->target,"target");

	stringstream title;
	title << "Generalization on " << sequence->name;

	if (!with_target) {
		GnuplotGenerator::plot_vector(title.str(), result);
	} else {
		vector<vector<weight_t>*>* target = sequence->get_targets_between(0, result->size());
		GnuplotGenerator::plot_vector(title.str(), result, "prediction", target, "target");
	}

	for (unsigned int i=0; i < result->size(); i++)
		delete (*result)[i];
	delete result;

	return 0.0;
}

vector<vector<weight_t>*>*  AutoPredictor::predict(Sequence* sequence, unsigned int timesteps, 	unsigned int context_length)
{
	assert(context_length <= sequence->size());
	assert(context_length >= 1);

	//unsigned int timesteps = sequence->size();

	vector<vector<weight_t>*>* result = new vector<vector<weight_t>*>();
	result->reserve( timesteps );
	network->reserve( timesteps );

	//input = new vector<vector<weight_t>* >();
	generated_sequence = new Sequence();

	// reset network and activate the context
	network->reset();

	vector<weight_t>* out = NULL;

	for (unsigned t = 0; t < timesteps; t++)
	{
//		cout << "time: " << t << endl;
		vector<weight_t>* in;
		if (t < sequence->size()) {
			in = sequence->input[t];
		} else {
			in = new vector<weight_t>(network->in_size, 0.0);
		}

		if (t >= context_length) {
			/*for (unsigned int i = 0; i < out->size(); i++)
			{
				if (mapping[i] != -1) {
					assert(mapping[i] < in->size());
					(*in)[ mapping[i] ] =  (*out)[ i ] ;
				}

			}*/
			transfer_function->transfer(in, out);
		} else {
			transfer_function->transfer(in);
		}
//		print_vector(in);
		network->_activate( in );
		out = network->get_output(network->timestep-1);
		generated_sequence->add(in, out);

		//print_vector(out);

		result->push_back( out );

		// memory cleanup
		/*if (t >= sequence->size()) {
			delete in;
		}*/

	}

	return result;

}
