/*
 * ConfusionMatrix.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: brandmaier
 */

#include "ConfusionMatrix.h"

ConfusionMatrix::ConfusionMatrix(Network* network, SequenceSet* sequences) {
	this->network = network;
	this->sequences = sequences;

	acc = -1;
	weighted_acc = -1;

}

void ConfusionMatrix::create()
{
	unsigned int dim = network->out_size;
	unsigned int matrix[dim][dim];
	for (unsigned int i=0; i < dim; i++)
		for (unsigned int j=0; j < dim; j++)
				matrix[i][j] = 0;

	cout << "Dimension (number of classes): " << dim << endl;
	cout << "Sequences: " << sequences->size() << endl;
	unsigned int total = 0;
	unsigned int total_correct = 0;

	unsigned int num[dim];
	for (unsigned int i=0; i < dim; i++)
		num[i]=0;

	for (unsigned int k=0; k < sequences->size(); k++) {

		//cout << "sequence " << k << endl;
		Sequence* sequence = (*sequences)[k];

		network->reserve( sequence->size() );
		network->reset();

		unsigned int start = 0;
		unsigned int end = sequence->size();

		for (unsigned int i=start; i < end; i++)
		{
			network->_activate( sequence->get_input(i) );

			std::vector<weight_t>* outputs = network->get_output(network->timestep-1);
			std::vector<weight_t>* targets = sequence->get_target(i);

			int max_t_index = -1;	//this is to capture undefined targets
			int max_y_index = 0;
			for (unsigned int j=0; j < dim; j++)
			{
				if (!isnan((*targets)[j])) {
					if (max_t_index == -1)  { max_t_index=j; }
					else if ((*targets)[j] > (*targets)[max_t_index]) max_t_index=j;
				}
				if ((*outputs)[j] > (*outputs)[max_y_index]) max_y_index=j;

			}

			if (max_t_index == -1) continue;

			matrix[max_t_index][max_y_index]++;

			if (max_t_index==max_y_index) {
				total_correct++;
			}
			total++;

			num[max_t_index]++;
		}



	}

	for (unsigned int i=0; i < dim; i++)
	{
		cout << "Num of targets="<<i<<": " << num[i]<<endl;
	}

	cout << "print" << endl;

	unsigned int sums[dim];
	for (unsigned int i=0; i < dim; i++)
		sums[i] = 0;

	for (unsigned int i=0; i < dim; i++)
	{
		for (unsigned int j=0; j < dim; j++)
		{
			sums[i] = sums[i] + matrix[i][j];
		}
	}

	for (unsigned int i=0; i < dim; i++)
	{
		cout << i << ".: ";
		for (unsigned int j=0; j < dim; j++)
		{
			cout << (matrix[i][j]*100.0/sums[i]) << "\t";
		}
		cout << "\n";
	}

	weighted_acc = 0.0;
	for (unsigned int i=0; i < dim; i++)
	{
		weighted_acc += (matrix[i][i]*100.0/sums[i]);

	}

	weighted_acc = weighted_acc/ dim;
	acc = (total_correct*100.0/total);

	cout << "Total (unweighted) Accuracy: " << acc << endl;
	cout << "Total (weighted) Accuracy: " << (weighted_acc) << endl;

}

void ConfusionMatrix::create_with_entropythreshold(double entropy_threshold)
{
	unsigned int dim = network->out_size;
	unsigned int matrix[dim][dim];
	for (unsigned int i=0; i < dim; i++)
		for (unsigned int j=0; j < dim; j++)
				matrix[i][j] = 0;

	cout << "Dim: " << dim << endl;

	unsigned int total = 0;
	unsigned int total_correct = 0;

	unsigned int num[dim];
	for (unsigned int i=0; i < dim; i++)
		num[i]=0;

	for (unsigned int k=0; k < sequences->size(); k++) {

		//cout << "sequence " << k << endl;
		Sequence* sequence = (*sequences)[k];

		network->reserve( sequence->size() );
		network->reset();

		unsigned int start = 0;
		unsigned int end = sequence->size();

		for (unsigned int i=start; i < end; i++)
		{
			network->_activate( sequence->get_input(i) );

			double p = 0.0;

			std::vector<weight_t>* outputs = network->get_output(network->timestep-1);
			std::vector<weight_t>* targets = sequence->get_target(i);


			for (unsigned int j=0; j < outputs->size(); j++)
			{
				double value = (*outputs)[j];
				p-= value*log(value);
			}

			if (p > entropy_threshold) {
				continue;
			}

			int max_t_index = -1;
			int max_y_index = 0;
			for (unsigned int j=0; j < dim; j++)
			{
				if ((*targets)[j] > (*targets)[max_t_index]) max_t_index=j;
				if ((*outputs)[j] > (*outputs)[max_y_index]) max_y_index=j;

			}

			if (max_t_index == -1) continue;

			matrix[max_t_index][max_y_index]++;

			if (max_t_index==max_y_index) {
				total_correct++;
			}
			total++;

			num[max_t_index]++;
		}



	}

	for (unsigned int i=0; i < dim; i++)
	{
		cout << "Num of targets="<<i<<": " << num[i]<<endl;
	}

	cout << "print" << endl;

	unsigned int sums[dim];
	for (unsigned int i=0; i < dim; i++)
		sums[i] = 0;

	for (unsigned int i=0; i < dim; i++)
	{
		for (unsigned int j=0; j < dim; j++)
		{
			sums[i] = sums[i] + matrix[i][j];
		}
	}

	for (unsigned int i=0; i < dim; i++)
	{
		cout << i << ".: ";
		for (unsigned int j=0; j < dim; j++)
		{
			cout << (matrix[i][j]*100.0/sums[i]) << "\t";
		}
		cout << "\n";
	}

	double weighted_acc = 0.0;
	for (unsigned int i=0; i < dim; i++)
	{
		weighted_acc += (matrix[i][i]*100.0/sums[i]);

	}

	weighted_acc = weighted_acc/ dim;

	cout << "Total (unweighted) Accuracy: " << (total_correct*100.0/total) << endl;
	cout << "Total (weighted) Accuracy: " << (weighted_acc) << endl;

}

ConfusionMatrix::~ConfusionMatrix() {
	// TODO Auto-generated destructor stub
}
