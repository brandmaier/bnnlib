#ifndef TRAININGCONFIGURATION_H_
#define TRAININGCONFIGURATION_H_

#include "Network.h"
#include "ensembles/Ensemble.h"
#include "ensembles/LSTMEnsemble.h"
#include "nodes/Node.h"
#include "functions.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "GnuplotGenerator.h"
#include "callbacks/CallbackHandler.h"


/**
 * A Configuration describes a setup of a Network,
 * a training algorithm, a dataset and additional
 * parameters of these.
 *
 * Configurations can be loaded from a file
 * and they allow the exchange of Datasets, network
 * architecture and other parameters without the
 * need to alter program code and recompile.
 *
 */
struct TrainingConfiguration : CallbackHandler
{

	string training_filename;
	string test_filename;
	string save_filename;
	string network_filename;
	
	string comment;
	
	bool normalize;
	bool randomize;
	bool randomize_bias_weights;

	double scale;
	double scale_min, scale_max;

	bool embedded;
	unsigned int embedding_dimension;

	string directory;

	int method;
	bool dots;
	
	unsigned int save_every;
	
	unsigned int report_every;
	
	unsigned int epochs;
	
	Network* network;
	SequenceSet* training_sequence;
	
	TrainingConfiguration();
	
	TrainingConfiguration(string filename);
	
	void init();
	
	void save(string filename);

void load(string filename);
	
	void execute(bool train);
	
	void save_as_image();
	
	void callback(Network* net, Trainer* trainer, SequenceSet* set);
	
};

#endif /*TRAININGCONFIGURATION_H_*/
