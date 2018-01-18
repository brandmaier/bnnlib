#ifndef GNUPLOTGENERATOR_H_
#define GNUPLOTGENERATOR_H_

#include "Network.h"
#include "ensembles/LSTMEnsemble.h"
#include "ensembles/LSTMForgetEnsemble.h"
#include "ensembles/KeepEnsemble.h"
#include <vector>
#include <string>

struct GnuplotGenerator
{
	static string temp_directory;
	
	static bool grid;
	static bool output_screen;
	
	static string pretext;

	static void clear_temp_directory();

	static void plot_activations(Network* network, Sequence* sequence, vector<Node*>* nodes);

	static void plot_lstm_activations(Network* network, LSTMEnsemble* lstmensemble, Sequence* sequence);
	static void plot_lstm_activations(Network* network, LSTMForgetEnsemble* lstmensemble, Sequence* sequence);
	static void plot_all_cec_activations(Network* network, Sequence* sequence);
	static void plot_input_output_target(Network* network, Sequence* sequence);
	static void plot_input_target(Network* network, Sequence* sequence);

	static void plot_output_target_difference(Network* network, Sequence* sequence);


	static void plot_all_activations(Network* network, Sequence* sequence);
	static void plot_all_lstm_activations(Network* network, Sequence* sequence);
	static void plot_all_lstmplus_activations(Network* network, Sequence* sequence);

	static void plot(stringstream* sstream, vector<string>* titles, string title);
	
	static void plot_keep_ensemble(Network* network, Sequence* sequence, KeepEnsemble* keep);
	
	static void plot_vectors(string title, vector<weight_t>* v1, string n1, vector<weight_t>* v2, string n2);
	static void plot_vector(string title, vector<weight_t>* v1);
	static void plot_vector(string title, vector<vector<weight_t>*>* v1);
	static void plot_vector(string title, vector<vector<weight_t>*>* v1, string t1,vector<vector<weight_t>*>* v2, string t2 );

	/**
	 * plot a given multi-variate timeseries with shading indicated by starting position of the shading and its respective colour.
	 */
	static void plot_shaded_curve(string title, vector<vector<weight_t>*>* wave, vector<unsigned int>* positions, vector<string>* shades);


	static void animate(vector<vector<weight_t>*>* data);
	static void animate(vector<vector<weight_t>*>* data, vector<vector<weight_t>*>* curve, unsigned int rate);

	static void plot_jacobian(Jacobian* jacobian, unsigned int output_node, unsigned int output_time, bool _3d);
	//static void plot_jacobian_3d(Jacobian* jacobian, unsigned int output_node, unsigned int output_time);

};

#endif /*GNUPLOTGENERATOR_H_*/
