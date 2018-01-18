/*
 * Jacobian.h
 *
 *  Created on: Jan 20, 2010
 *      Author: brandmaier
 */

#ifndef JACOBIAN_H_
#define JACOBIAN_H_

#include <vector>

struct Jacobian
{
	unsigned int in_size;
	unsigned int out_size;
	unsigned int t;
	unsigned int num_rows;
	unsigned int num_cols;

	vector<vector<double> >* data;

	Jacobian(unsigned int in_size, unsigned int out_size, unsigned int t)
	{
		this->in_size = in_size;
		this->out_size = out_size;
		this->t = t;

		num_rows = out_size*t;
		num_cols = in_size*t;

		data = new vector<vector<double> >(num_rows, vector<double>(num_cols));

		for (unsigned int i=0; i < num_rows; i++)
			for (unsigned int j=0; j < num_cols; j++)
			{
				(*data)[i][j] = 0.0;
			}
	}

/*	vector<double>* aggregate_to_input_vector()
	{

	}
*/
	vector<double>* get_derivatives_of_output(unsigned int output_node_index, unsigned int timestep)
	{
		return &(*this->data)[out_size*timestep+output_node_index];
	}

	double get_derivative_of_output_to_input(unsigned int output_node_index,unsigned int output_timestep, unsigned int input_node_index, unsigned int input_timestep)
	{
		return (*this->data)[out_size*output_timestep+output_node_index][in_size*input_timestep+input_node_index];
	}

	void _print() { print(); }

	void print()
	{
	// print it
	for (unsigned int t1 = 0; t1 < t; t1++)
	for (unsigned int i=0; i < out_size; i++)
	{
		cout << "sensitivity for output i=" << i << " at t="<<t1<< endl;
		for (unsigned int t2=0; t2 < t; t2++)
		for (unsigned int j=0; j < in_size; j++)
		{
			cout << (*data)[t1*out_size+i][in_size*t2+j] << "\t";

		}
		cout << endl;
	}
	}

};

#endif /* JACOBIAN_H_ */
