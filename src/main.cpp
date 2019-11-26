#include "nodes/Node.h"
#include <iostream>
#include "Network.h"
#include <vector>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //for chdir
#include "Sequence.h"
#include "SequenceSet.h"
#include "functions.h"
#include "NetworkFactory.h"
#include "GnuplotGenerator.h"
#include "trainer/GeneticTrainer.h"
#include "trainer/PathtrackingTrainer.h"
#include <algorithm>
#include "TrainingConfiguration.h"
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <ctime>
#include "nodes/LinearNode.h"
#include "networks/FeedForwardNetwork.h"
#include "LSTMCascade.h"
#include "callbacks/CallbackHandler.h"
#include "callbacks/BiasWeightWatcher.h"
#include "callbacks/WeightWatcher.h"
#include <cmath>
#include "trainer/Trainer.h"
#include "trainer/BackpropTrainer.h"
#include "trainer/RPropTrainer.h"
#include "trainer/MyRPropTrainer.h"
#include "callbacks/DebugCallback.h"
#include "callbacks/GradientWatcher.h"
#include "callbacks/DummyCallback.h"
#include "trainer/ImprovedRPropTrainer.h"
#include "trainer/ARPropTrainer.h"
#include "criteria/BinaryClassificationSuccess.h"
#include "callbacks/StateCallback.h"
#include "callbacks/SaveNetworkCallback.h"
#include "criteria/ConvergenceCriterion.h"
#include "DataSet.h"
#include "criteria/EarlyStopping.h"
#include "errorfunctions/ThresholdSquaredErrorFunction.h"
#include "criteria/GeneralizationLoss.h"
#include "criteria/ZeroErrorCriterion.h"
#include "criteria/ZeroValidationErrorCriterion.h"
#include "criteria/ErrorBelowCriterion.h"
#include "criteria/ValidationErrorBelowCriterion.h"
#include "networks/LSTMNetwork.h"
#include "callbacks/ReportCallback.h"
#include "callbacks/ZeroErrorFailReport.h"
#include "errorfunctions/ManhattanError.h"
#include "ensembles/SecondOrderEnsemble.h"
#include "classification/AutoPredictor.h"
#include <signal.h>
#include "trainer/PSOTrainer.h"
#include "nodes/AlternatingBiasNode.h"
#include "trainer/MyRPropTrainer.h"
#include "errorfunctions/WinnerTakesAllErrorFunction.h"
#include "classification/ConfusionMatrix.h"
#include "trainer/QuickpropTrainer.h"
#include "classification/TimeseriesClassification.h"
#include "tasks/Task.h"
#include "tasks/SuperimposedSineTask.h"
#include "tasks/MSDTask.h"
#include "criteria/ConvergenceCriterion.h"
#include "tasks/ClassifyFrequency.h"
#include "HarmonicNetwork.h"
#include "HarmonicGateNetwork.h"
#include "tasks/FacesEmbeddedTask.h"
#include "tasks/FacesEmbeddedTask2.h"
#include "tasks/SineEmbeddedTask.h"
#include "architecture/DelayedSecondOrderArchitecture.h"
#include "callbacks/ProgressDot.h"
#include "nodes/GaussianNoiseNode.h"
#include "nodes/PseudoOutputNode.h"
#include "networks/SlowNetwork.h"
#include "tasks/TentmapTask.h"
#include "tasks/MackeyGlassTask.h"
#include "ensembles/SlowEnsemble.h"
#include "AutoAssociationSequence.h"
#include "callbacks/ClassificationCallback.h"
#include "Sequence.h"
#include "ensembles/ConditionalBiasEnsemble.h"
#include "networks/RecurrentNetwork.h"
#include "networks/LSTMNetwork.h"
#include "callbacks/ClassificationCallback.h"
#include "networks/RandomGateNetwork.h"
#include "trainer/CGTrainer.h"
#include "classification/CrossValidationError.h"
#include "errorfunctions/RecursiveThermometricErrorFunction.h"
#include "classification/interfaces/ClassificatorInterface.h"
#include "classification/interfaces/RecursiveThermometricClassificator.h"
#include "classification/interfaces/WinnerTakesAllClassificator.h"
//#include "trainer/SARPropTrainer.h"
#include "callbacks/PickBestGeneralizingNetwork.h"
// disable secure iterators
#define _SECURE_SCL 0

/**
 BNNlib
 
 This library is a recurrent neural network library that is designed for large, sparse networks. For this reason it drops the notion
 of a network organization in layers and holds only a set of nodes with individual connections.
 
 
 
 **/



using namespace std;

#define SHOW cout << "capacity = " << v.capacity() << "\t size = " << v.size() << endl;

/*
 * seed: 1260264885	worked!
 * 1260264990
 *
 */
void debug_2ndorderlstm()
{
//	srand(1260264990);

	Network* net = NetworkFactory::create_2ndorder_lstm(1,5,1);
	//net->export_to_dot_graph("graph");

	//net->sanity_check(true);
//	SequenceSet* set = new SequenceSet("datasets/grammar-train.dat");

//	SequenceSet* training_set = new SequenceSet("datasets/mackey-glass-5000.dat");
	SequenceSet* training_set = new SequenceSet("datasets/superimposedsin.dat");

	training_set->get(0)->split_in_training_validation_test_set(0.5,0.25);

//	SequenceSet* training_set = set->create_subset(0,12);

	//ErrorFunction* err_func = new ThresholdSquaredErrorFunction(1);
	//net->error_function = err_func;

	Trainer* trainer = new ImprovedRPropTrainer(net);
	trainer->learning_rate = 0.00001;
	trainer->add_callback( new ReportCallback(), 500);
	//trainer->add_abort_criterion( new ErrorBelowCriterion(err_func, 0), 10 );
	trainer->learning_rate = 0.1;
	trainer->train(training_set,10000);

	cout << "Iterations: " << trainer->total_iterations << endl;

//	net->save("mackeyglass.bnn");
	net->save("superimposedsin.bnn");

	delete trainer;
	delete net;
}

void pso()
{
	/*Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 2, 1, 1 ,0.0,0.0,0.0, true, false); //,-1.0,+3.0,-2.0);
*/
	unsigned int layers[] = {1};
	Network* net = NetworkFactory::createRecurrentNetwork(2,Node::TANH_NODE, 1, layers,Node::TANH_NODE,1);
	net->sort_nodes();

	SequenceSet* training_set = new SequenceSet("datasets/parity-noisy.dat");

	PSOTrainer* pso = new PSOTrainer(net, 50);

	pso->train(training_set, training_set, 1000);

	net->test_sequence( training_set, true);

	net->export_to_dot_graph("graph");

	delete pso;
	delete training_set;
	delete net;

}

void debug_2ndorder()
{
	Network* net = NetworkFactory::create_2ndorder(2,1);
	//net->export_to_dot_graph("graph");


	SequenceSet* set = new SequenceSet("datasets/xor.dat");

	Trainer* trainer = new ImprovedRPropTrainer(net);
	trainer->learning_rate = 0.1;
	trainer->train(set,100);


	net->save("2nd.bnn");
	delete net;
	net = new Network("2nd.bnn");

	net->test_sequence(set, true);

	net->export_to_dot_graph("graph");

	delete net;
	delete trainer;

	cout << "DONE" << endl;
}

void train_faces_embed()
{
	FacesEmbeddedTask2 mytask;
	mytask.init();
	mytask.train();


}

void test_faces_embed()
{
	FacesEmbeddedTask2 mytask;
	mytask.restore();
	mytask.test();
}

void test_wta()
{
	new WinnerTakesAllClassificator();
}

bool test()
{
	srand(2009);

	SequenceSet* input = new SequenceSet();
	input->load_from_file("datasets/gradient.dat");

	unsigned int hid[] = {10};

	Network* net = NetworkFactory::createRecurrentNetwork(
			input->input_size, Node::TANH_NODE, 1, hid,
			Node::LINEAR_NODE,
			input->target_size
	);

	Trainer* trainer = new BackpropTrainer(net);

	trainer = new RPropTrainer(net);

	trainer->learning_rate = 0.001;

	trainer->train(input, 10000);
	vector<weight_t>* error = &trainer->error_train;

	GnuplotGenerator::plot_vector("Training Error", error);

	net->test_sequence(input, true);

//	net->export_to_dot_graph();

	delete net;
	delete trainer;
	delete input;

	return true;
}

void biomag_test()
{
	cout << "* restoring biomagtemp-temp.bnn " << endl;
	cout << "* loading datasets/biomag-alltrials-69-200-2class.dat " << endl;

	Network* network = new Network("biomagtemp-temp.bnn");

	SequenceSet* input = new SequenceSet("datasets/biomag-alltrials-69-200-2class.dat");

	input->normalize(true, false);

	ConfusionMatrix cm(network, input);

	cm.create();

	delete network;
	delete input;
}


void mushroom()
{
	//SequenceSet* sequences = new SequenceSet();
	DataSet* dataset = SequenceSet::load_from_proben_file("datasets/proben1/mushroom/mushroom1.dt");

	//SequenceSet* t

	unsigned int layers[] = {20};
	Network* network = NetworkFactory::createFeedForwardNetwork(125,Node::TANH_NODE, 1, layers, Node::SIGMOID_NODE, 2);

//	ImprovedRPropTrainer trainer(network);
//	MyRPropTrainer trainer(network);
	CGTrainer trainer(network);

	//trainer.add_callback( new ReportCallback(), 10);

	//ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.5);
	//trainer.add_abort_criterion( new ErrorBelowCriterion(err_func,0),1);
	trainer.add_abort_criterion( new ConvergenceCriterion(), 10);

	trainer.train( dataset->training_set, dataset->validation_set, 1000);

	trainer.plot_errors();

	network->save("mushroom.bnn");

	cout << "Validation Set Size: " << dataset->validation_set->size() << endl;
	cout << "Validation Error: " << network->evaluate_error( dataset->validation_set ) << endl;

	ErrorFunction* error_function = new WinnerTakesAllErrorFunction();
	cout << "Iterations: " << trainer.total_iterations << endl;
	cout << "Test Error: " << network->evaluate_error( dataset->test_set, error_function ) << endl;

	delete dataset;
	delete network;
}

void soybean()
{
	//SequenceSet* sequences = new SequenceSet();
	DataSet* dataset = SequenceSet::load_from_proben_file("/home/brandmaier/Documents/proben1/soybean/soybean1.dt");

	//SequenceSet* t

	unsigned int layers[] = {32};
//	Network* network = NetworkFactory::createFeedForwardWTANetwork(82, 1, layers, 19);
	Network* network = NetworkFactory::createFeedForwardNetwork(82, Node::TANH_NODE, 1, layers, Node::TANH_NODE, 19);

//	ImprovedRPropTrainer trainer(network);
	MyRPropTrainer trainer(network);

	trainer.add_callback( new ReportCallback(), 10);

	//ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.5);
	//trainer.add_abort_criterion( new ErrorBelowCriterion(err_func,0),1);
	trainer.add_abort_criterion( new ConvergenceCriterion(), 20);
	trainer.add_abort_criterion( new GeneralizationLoss(dataset->validation_set,5), 5);

	trainer.train( dataset->training_set, dataset->test_set, 1000);

	network->save("mushroom.bnn");

	cout << "Validation Set Size: " << dataset->validation_set->size() << endl;
	cout << "Training Error: " << network->evaluate_error( dataset->training_set ) << endl;
	cout << "Validation Error: " << network->evaluate_error( dataset->validation_set ) << endl;
	cout << "Test Error: " << network->evaluate_error( dataset->test_set ) << endl;

	cout << "Test%: " <<
		(dataset->test_set->target_max-dataset->test_set->target_min)/dataset->test_set->element_count* network->evaluate_error( dataset->test_set ) << endl;


	GnuplotGenerator::plot_vectors("Error",&trainer.error_train, "Training", &trainer.error_validation, "Validation");


	//ErrorFunction* err_func = new WinnerTakesAllErrorFunction();

	cout << "Test-Error: " << network->evaluate_error(dataset->test_set);

	//network->error_function = err_func;

	//cout << "Acc: " <<*/
	//ConfusionMatrix cm(network, dataset->training_set);

//	cm.create();

//ConfusionMatrix cm2(network, dataset->test_set);

//	cm2.create();

	delete dataset;
	delete network;
}

/* switch (20)
 * ca. 3% improvement für c=0.5,alpha=0.001,mom=0.99
 *		"	"			"	    ALPHA=0.01
 *
 */
unsigned int switch_train(bool condition)
{
	//srand(7177583551);
	srand(3793818757);
	//unsigned int c = 0.1;
	Network* net;

	/*if (!condition) {
		message("using slow network");
		net = new SlowNetwork(1,20,1, 0.0001);
		net->print_offsets();
	} else {
		net = new SlowNetwork(1,20,1, 0.0);
		*/
		message("using LSTM network");
		float c = 0.1;
		net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
		 Node::TANH_NODE, Node::TANH_NODE, 1, 3, 1, -c,+c,-c, true, true);
		net->sort_nodes();
	//}

/*	warning("random gate at work!");
	Network* net = NetworkFactory::create_random_gate_memory_network(
			1,condition?8:10,1, 1.0,50);
*/


/*	if (use_initial) {
	 net->use_initial_states = true;
	 net->randomize_initial_states();
	}
*/

	SequenceSet* sequence_set = new SequenceSet("datasets/switch-50.dat");
	sequence_set->scale_to_minmax(-1,+1);

	DataSet* dataset = sequence_set->toDataSet(0.8,0.1);

	Trainer* trainer =// new BackpropTrainer(net);
	//new ImprovedRPropTrainer(net);
	//		new MyRPropTrainer(net);
			new CGTrainer(net);
	trainer->learning_rate=0.01;
	trainer->momentum=1;

	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(1);
	net->error_function = err_func;

	trainer->add_abort_criterion(
			new ZeroErrorCriterion(err_func)
			,1);

	trainer->add_callback(
			new ReportCallback(),
			10
	);


	trainer->train(dataset->training_set, dataset->validation_set,1000);

	GnuplotGenerator::plot_vectors("Error", &trainer->error_train,"train",
			&trainer->error_validation, "validation");

//	net->test_sequence(sequence_set, true);
	cout << "Total iterations: " << trainer->total_iterations << endl;
	cout << "Error:" << net->evaluate_training_error(sequence_set);

	net->save("switch.bnn");

	return trainer->total_iterations;
}

void switch_test()
{
	Network* net = new Network("switch.bnn");
	SequenceSet* sequence_set = new SequenceSet("datasets/switch-50.dat");
	sequence_set->scale_to_minmax(-1,+1);

	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(1);
	net->error_function = err_func;

//	net->export_to_dot_graph("switch");
/*	GnuplotGenerator::plot_all_lstm_activations(net, sequence_set->get(8));
	GnuplotGenerator::plot_all_lstm_activations(net, sequence_set->get(9));
	GnuplotGenerator::plot_all_lstm_activations(net, sequence_set->get(10));

	GnuplotGenerator::plot_all_cec_activations(net, sequence_set->get(8));
	GnuplotGenerator::plot_all_cec_activations(net, sequence_set->get(9));
	GnuplotGenerator::plot_all_cec_activations(net, sequence_set->get(10));
*/
	GnuplotGenerator::plot_all_activations(net, sequence_set->get(5));
	GnuplotGenerator::plot_all_activations(net, sequence_set->get(6));
	GnuplotGenerator::plot_all_activations(net, sequence_set->get(7));

	net->test_sequence(sequence_set, true);

	// generate animation //

	// 13 is a spike, 18 is a spike
	Sequence* sequence = sequence_set->get(18);
	net->activate( sequence );

	vector<Node*> outputs;
	for (unsigned int i=0; i < net->out_size; i++) {
		outputs.push_back( net->nodes[net->out_offset+i] );
	}
	vector<vector<weight_t>*>* data = net->collect_node_outputs(outputs);

	GnuplotGenerator::animate(data, &sequence->input, 1 );
}

void pfg_train()
{
	//srand(23248);

	bool use_initial = false;

	SequenceSet* input = new SequenceSet("datasets/pfg-cos.dat");
	/*Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 1, 5, 1, -0.1,0.1,-0.1, true, false);
*/

//	input->scale_to_minmax(-1, +1);

	//input->scale_to_minmax(-1,+1);
	//input->normalize();

	Network* net = NetworkFactory::create_harmonic_generator(1,1,1);


	//net->randomize(true, 0.000001);

	//net->sort_nodes();/*
/*	for (unsigned int i=0; i < 1;i++)
	{
		Node* node = new AlternatingBiasNode(2*(i+1),+1,0);
		net->add_node(node);
		for (unsigned int j=0; j < net->ensembles; j++)
		{
			net->connect( node, net->nodes[net->hid_offset+j], true);
		}
	}
*/


/*		Network* net = NetworkFactory::create_random_gate_memory_network(
			1,12, 1, 1.0,50);

		net->sort_nodes();
*/
/*	unsigned int hid[] = {12};
	Network* net = NetworkFactory::createRecurrentNetwork(1,Node::TANH_NODE,
			1, hid, Node::TANH_NODE, 1);
*/
//	net->washout_time = 20;

	if (!net->sanity_check(false))
		error("Network is insane!");

	if (use_initial) {
		net->use_initial_states = true;
		net->randomize_initial_states();
	}
//	net->export_to_dot_graph();

//	Trainer* trainer = new BackpropTrainer(net);

	//ErrorFunction* err_func = new MinkowskiError();
	//net->error_function = err_func;
	//input->get(0)->split_in_training_test_set(0.75);

//	RPropTrainer::eta_plus = 1.2;
//	RPropTrainer::eta_minus = 0.3;
	Trainer* trainer =  //new BackpropTrainer(net);
			//new MyRPropTrainer(net);
			new ImprovedRPropTrainer(net);
//	Trainer* trainer = new ARPropTrainer(net);
	trainer->momentum = 0.9;
	trainer->learning_rate = 0.001;
	//net->sort_nodes();
	//input->scale_to_minmax(-1, +1);

	trainer->learning_rate = 0.001;

//	trainer->add_abort_criterion( new ConvergenceCriterion(10e-9),10 );

	trainer->add_callback( new ReportCallback(), 100);
	trainer->add_callback( new SaveNetworkCallback("pfg"),500);

	trainer->train(input, input, 10000);
	vector<weight_t>* error = &trainer->error_train;

//	net->export_to_dot_graph();

	GnuplotGenerator::plot_vector("Training Error", error);

//	net->test_sequence(input, true);

	net->save("pfg.bnn");

	//cout << "s"

	delete net;
	delete input;
	//delete error;
}

void pfg_test()
{
	Network* net = new Network("pfg.bnn");
	SequenceSet* input = new SequenceSet("datasets/pfg-cos.dat");

//	net->export_to_dot_graph("pfg");
//	input->scale_to_minmax(-1,+1);

	//input->scale_to_minmax(-1, +1);

	double err = net->test_sequence(input,false);

	cout << "Error: " << err << endl;

	GnuplotGenerator::plot_input_output_target(net, (*input)[0] );

//	net->get_timeseries_generalization_error((input->set[0]),200);

	AutoPredictor* predictor = new AutoPredictor(net);
	predictor->predict_and_plot( input->get(0), 1000,100);

	delete net;
	delete input;

}



void test_mackeyglass()
{

	MackeyGlassTask task;
	task.restore();
	task.test();

}

void copyrestore()
{
	SequenceSet* input = new SequenceSet("datasets/pfg-cos.dat");
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_PLUS_ENSEMBLE,
	 Node::SIGMOID_NODE, Node::SIGMOID_NODE, 1, 10, 1,0,.0,.0, true, false);

	//net->export_to_dot_graph();
	//net->sort_nodes();

	input->scale_to_minmax(0,+1);

	Trainer* trainer = new RPropTrainer(net);

	trainer->train(input, 5000);
	vector<weight_t>* errors = &trainer->error_train;

	GnuplotGenerator::plot_vector("Training Error", errors);

	net->save("copyrestore.net");

	GnuplotGenerator::plot_input_output_target(net, (*input)[0]);
}

/**
 *
 * speed measurement
 *
 * current measurement on Andys VAIO Laptop:
 *
 * Name						[s]			[Mega clockticks]
 * -------------------------------------------
 * RELEASE	28.Sep 2009		9			8.550
 * DEBUG	28.Sep 2009		30		   29.810
 * RELEASE	28.Sep 2009		8			7.900   (tanh_efficient)
 * RELEAE	19.Sep 2009		8			7.960	(inlines)
 * RELEASE  20.Oct 2009		6			5.900 / 5.480 / 5.820
 * RELEASE	20.Oct 2009		5			5.270 / 5.390 / 5.650
 **/
void profile()
{
	std::cout << "speed test" << endl;
	Network* net = NetworkFactory::createTestNetwork9(1,20, 40,1, true);
	net->save("net9.bnn");

	net->sanity_check(true);


	SequenceSet* input = new SequenceSet();
	try {
		input->load_from_file("datasets/009-train.dat");
	} catch (const int e) {
		std::cout << "File not Found!" << endl;
		exit(-1);
	}
	std::cout << " start training " << endl;

	input->normalize();

	//net->train_until_convergence(input, val);
	Trainer* trainer = new BackpropTrainer(net);
	trainer->learning_rate = 0.0005;
	trainer->learning_rate_discount = 0.99999;
	trainer->momentum = 0.99; //0.999; //0.99; //0.999;
	trainer->batch_learning = false;


	time_t start, end;
	clock_t start_c, end_c;
	time(&start);
	start_c = clock();
	//clock_t start2 = clock
	trainer->train(input, 1);
	time(&end);
	end_c = clock();
	cout << "Elapsed time = " << difftime(end,start) << " and "
	 << (end_c-start_c) << endl;
	return;
}

void embed_train()
{
	unsigned int dim = 8; // embedding dimension

	cout << "H"<<endl;
	SequenceSet* input = new SequenceSet("datasets/superimposedsin2.dat");

	Sequence* sequence = input->get(0);
	//cout << "pre" << endl;
	Sequence* e = sequence->embed(dim);

	SequenceSet* input2 = new SequenceSet();
	//cout << "preadd" <<endl;
	input2->add_sequence(e);
	//cout << "presave"<<endl;
	input2->save_to_file("embed.dat");

	//e->split_in_training_validation_test_set(0.8,0.1);

/*	unsigned int num_layers[] = {30};
	Network* network = NetworkFactory::createFeedForwardNetwork(dim,
			Node::TANH_NODE, 1, num_layers, Node::LINEAR_NODE,1);
*/

	//Network* network = NetworkFactory::create_2ndorder_lstm(dim,10,1);
	Network* network = NetworkFactory::create_2ndorder(dim,1);

	Trainer* trainer = new ImprovedRPropTrainer(network);

	trainer->add_callback( new ReportCallback(),10 );
	trainer->add_callback( new SaveNetworkCallback("embed"), 500);

	trainer->train( input2, 2000 );

	network->test_sequence(input2, true);

	network->save("embed.bnn");

	cout << "DONE" << endl;
}

void embed_test()
{
	Network* network = new Network("embed.bnn");
	SequenceSet* input = new SequenceSet("embed.dat");

	TransferFunction* tf = new EmbededTransferFunction(8);
	AutoPredictor* predictor = new AutoPredictor(network, tf);

	predictor->predict_and_plot( input->get(0), 2500,500);

}

/*
void line_test()
{
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
			 Node::TANH_NODE, Node::TANH_NODE, 1, 10, 1 ,0.,-0.,0., true, false); //,-1.0,+3.0,-2.0);

	SequenceSet* input = new SequenceSet("datasets/superimposedsin.dat");



}
*/
void doodle()
{
	Network* net = new Network();

	Node* node1 = new TanhNode();
	Node* node2 = new TanhNode();

	Node* output = new LinearNode();

	net->add_node(node1);
	net->add_node(node2);
	net->add_node(output);

	Connection* w11 = net->connect(node1,node1,false);
	/*Connection* w12 =*/ net->connect(node1,node2,false);
	/*Connection* w21 =*/ net->connect(node2,node1,false);
	Connection* w22 = net->connect(node2,node2,false);

	net->connect(node1, output, true, 1.0);

	//net->sort_nodes();

	net->bias_offset=0;
	net->in_offset=0;
	net->hid_offset=0;
	net->hid_size=2;
	net->out_offset=2;
	net->out_size=1;

	double phi = 0.1;
	w11->weight = cos(phi);
	w22->weight = cos(phi);

	AutoPredictor predictor(net, new DummyTransferFunction());

	Sequence* seq = new Sequence();
	vector<weight_t>* input = new vector<weight_t>();

	vector<weight_t>* target = new vector<weight_t>();
	target->push_back(0.0);
	seq->add(input, target);
	predictor.predict_and_plot(seq, 100, 1);
}

double train_superimposedsins(bool initial_state)
{
	//srand(1262703546);
	//srand(9945456);
	//initial_state=true;

/*	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
			 Node::TANH_NODE, Node::TANH_NODE, 1, 10, 1 ,0.01,-0.0,0.0, true, true); //,-1.0,+3.0,-2.0);
*/
/*	LSTMNetwork* net = new LSTMNetwork(1,10,1);
	net->sort_nodes();
*/
	HarmonicNetwork* net = new HarmonicNetwork(0, 1, 1, 1);


//	Network* net = NetworkFactory::create_tsin(1,2,1);


/*	unsigned int layers[1] = {23};
	Network* net = NetworkFactory::createRecurrentNetwork(1,Node::TANH_NODE,1,
			layers, Node::TANH_NODE,1);
*/
//	warning("random gate architecture!");
/*	Network* net = NetworkFactory::create_random_gate_network(1,
			90,1,1.0,1);
*/
	//Network* net = NetworkFactory::create_2ndorder_lstm(1,10,1);

	//net->sort_nodes();
	//net->add_output_to_input_feedback( false );
	// gut: -0.5 +0.5 -0.5
	//net->export_to_dot_graph("harmonic");

/*	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(
			Ensemble::LSTM_PLUS_ENSEMBLE,
			 Node::TANH_NODE, Node::TANH_NODE, 1, 10, 1 ,-0.5,+0.5,-0.5, true); //,-1.0,+3.0,-2.0);
*/

//	net->save("superimposedsin.bnn");

//	net = new Network("superimposedsin.bnn");

	//backprop
	// w/o IS Training: 19.7412 Validation: 10.4052 Test: 10.138
	// with IS Training: 19.7364 Validation: 10.9291 Test: 10.3018

	// iRprop

	// w/o IS
	// epochs 26
	//Training: 4.74505 Validation: 2.72293 Test: 2.52764
	// with IS epochs 32	(-=delta)
	// Training: 4.34633 Validation: 2.89193 Test: 2.39736
	//  (+=delta)


	if (initial_state) {
		cout << "using Initial State" << endl;
		net->use_initial_states = true;
		net->randomize_initial_states();
	}

	//SequenceSet* input = new SequenceSet("datasets/superimposedsin.dat");
	SequenceSet* input = new SequenceSet("datasets/superimposedsin-5sin.dat");

	//	SequenceSet* input = new SequenceSet("datasets/mackey-glass.dat");

	//	SequenceSet* input = new SequenceSet("datasets/superimposedsin3.dat");

//	input = input->embed(3);

//	cout << "embed" <<endl;
//	cout << input->set.size() <<endl;
	input->get(0)->split_in_training_validation_test_set(0.8,0.1);



	input->scale_to_minmax(-1,+1);



	bool method = true;

	Trainer* trainer;
	if (method) {
	 trainer =
			// new MyRPropTrainer(net);
			new ImprovedRPropTrainer(net);
	// ImprovedRPropTrainer::eta_plus = 1.3;
	 //Trainer* trainer = new BackpropTrainer(net);
	// trainer->learning_rate = 0.00001;
	// trainer->momentum = 0.99;
	 trainer->batch_learning = true;
	} else {
	 trainer = new BackpropTrainer(net);
	 trainer->learning_rate = 0.001;	// 0.0005 geht
	 trainer->momentum = 1;
	 //trainer->batch_learning = true;
	}

/*	vector<Node*>* nodes = new vector<Node*>();
	for (unsigned int i=0; i < net->lstms.size(); i++)
	{
		nodes->push_back( net->lstms[i]->get_output() );
	}
	trainer->decorrelation_nodes = *nodes;
	trainer->decorrelation_alpha = 0.1;
	//trainer->decorrelation_enabled = true;
*/

		vector<Node*>* nodes = new vector<Node*>();
		vector<Ensemble*>* ensembles = net->get_ensembles_by_type(Ensemble::HARMONIC_GENERATOR_ENSEMBLE);
		cout << "adding " << ensembles->size() << " ensembles for decorrelation"<< endl;
		for (unsigned int i=0; i < ensembles->size(); i++)
		{
			nodes->push_back(
					(*(*ensembles)[i]->get_outputs())[0]
					);
		}
		trainer->decorrelation_nodes = *nodes;
		trainer->decorrelation_alpha = 1;
	//	trainer->decorrelation_enabled = true;



	CallbackHandler* cb = new SaveNetworkCallback("superimposedsin");
	trainer->add_callback(cb, 200);

	CallbackHandler* rp = new ReportCallback();
	trainer->add_callback(rp, 100);
	/*
	trainer->add_abort_criterion(
			new ZeroValidationErrorCriterion(
					new ThresholdSquaredErrorFunction(0.1)
			),10
	);*/

	trainer->add_abort_criterion(
			new ErrorBelowCriterion(
					new SquaredErrorFunction(),0.001// 0.01*10000
			), 1
	);

	trainer->add_abort_criterion(
			new ConvergenceCriterion(),10
	);

//	CallbackHandler* cb = new StateCallback();
//	trainer->train(input,input, 5000, 1, cb);
	cout << "START" << endl;


	trainer->train(input, 2000);

	for (unsigned int i=0; i < 8; i++) {

		//net->freeze_all_generators();
		net->add_generator();
//	trainer = new ImprovedRPropTrainer(net);
		//net->reset_gamma(0.1);
		trainer->reset();
		trainer->train(input, 2000);
	}

	GnuplotGenerator::plot_input_output_target(net, input->get(0));

	net->save("superimposedsin.bnn");

	double error = net->test_sequence( input );
	double total_error = net->evaluate_error(input, new SquaredErrorFunction() );
	double training_error = net->evaluate_training_error( input );
	double validation_error = net->evaluate_validation_error( input );
	double test_error = net->evaluate_test_error( input );

	cout << "Epochs: " << trainer->total_iterations <<endl;
	cout << "Total Error: " << error << "/" << total_error << endl;
	cout << "Training: " << training_error <<" Validation: " << validation_error << " Test: " << test_error << endl;

//	net->export_to_dot_graph("sup");

	delete net;
	delete input;

	//return test_error;
	return trainer->total_iterations;
}

void test_faces()
{

	Network* net = new Network("faces.bnn");

	SequenceSet* input = new SequenceSet("ExperimentFaces/faces-nca.dat");

	input->normalize();
//	input->scale_to_minmax(-1,+1);
//	vector<weight_t>* first = input->get(0)->input[0];

/*	int mapping[71];
	for (unsigned int i=0; i < 71; i++)
		mapping[i] = -1;
	// map confederate shape to input
	for (unsigned int i=0; i < 12; i++)
		mapping[10+i] = i;
	// map confederate app to input
	for (unsigned int i=0; i < 26; i++)
		mapping[43+i] = 12+i;
*/
	int mapping[12];
	for (unsigned int i=0; i < 12; i++)
			mapping[i] = -1;
	for (unsigned int i=0; i < 12; i++)
		mapping[i] = 10+i;
	// map confederate app to input
/*	for (unsigned int i=0; i < 26; i++)
		mapping[12+i] = 43+i;
*/


	MappedTransferFunction* transfer_function = new MappedTransferFunction();

	for (unsigned int i=0; i < 12; i++)
			mapping[i] = -1;
	transfer_function->set_mapping(mapping);

	AutoPredictor* predictor = new AutoPredictor(net, transfer_function);
	predictor->predict_and_plot( input->get(0),
			input->get(0)->size(),200);

	for (unsigned int i=0; i < 12; i++)
			mapping[i] = 10+i;
	transfer_function->set_mapping(mapping);
	predictor->predict_and_plot( input->get(0),
			input->get(0)->size(),200);

	SequenceSet* wrapset = new SequenceSet(predictor->generated_sequence);
	wrapset->denormalize(input);
	wrapset->save_to_file("test");


	delete net;
	delete input;
	delete predictor;
}

void train_faces()
{
	unsigned int in=71;
	unsigned int out=12; //38;
	unsigned int hid = 100;


/*	Network* net = NetworkFactory::create_random_gate_memory_network(
			in,30,out,  0.8,50);
*/
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
			 Node::TANH_NODE, Node::TANH_NODE, in, hid, out ,0.1,-0.1,0.1, true, false);
	net->sort_nodes();

	net->sanity_check(true);


	SequenceSet* input = new SequenceSet();
	try {
		input->load_from_file("datasets/faces-shapeonly.dat");
	} catch (const int e) {
		error("File not found!");
	}

	for (int i=0; i < (int)input->size(); i++)
	{
		Sequence* seq = input->get(i);
		seq->split_in_training_validation_test_set(0.8,0.1);
	}

	std::cout << "normalizing input "<< endl;
	//input->normalize();
	input->scale_to_minmax(-1,+1);
	input->save_to_file("datasets/faces-shapeonly-scaled.dat");
	std::cout << " start training " << endl;


	Trainer* trainer = new ImprovedRPropTrainer(net);

	trainer->add_callback( new ReportCallback(), 50);
	trainer->add_callback( new SaveNetworkCallback("faces"), 100);

	trainer->train(input, 20000);

	net->save("faces.bnn");

	delete net;

	delete input;
}

void train_mackeyglass(bool condition)
{

	MackeyGlassTask task;
	task.init();
	task.train();
/*
	Criterion* criterion = new ErrorBelowCriterion(
			new SquaredErrorFunction(),2500*0.00001
	);

	trainer->add_abort_criterion(criterion,1);

	trainer->add_callback(
			new ReportCallback(), 10
			);

	trainer->add_callback(new SaveNetworkCallback("mackeyglass"),100);

	trainer->train(training_set, 3000);
//	vector<weight_t>* error = &trainer->error_train;

	//net->test_sequence(input, false);

	cout << "Epochs: " << trainer->total_iterations << endl;
	//cout << "Final error " << err << endl;

	net->save("mackeyglass.bnn");

	cout << "Training error: " << net->evaluate_validation_error(training_set) << endl;
	cout << "Validation error: " << net->evaluate_validation_error(training_set) << endl;
	cout << "Test error:       " << net->evaluate_test_error(training_set) << endl;

*/
}


void arch()
{
	SequenceSet* sequence_set = new SequenceSet("datasets/superimposedsin.dat");
	DataSet* dataset = new DataSet(sequence_set); //sequence_set->toDataSet(1.0,0.0);
	DelayedSecondOrderArchitecture arch(dataset);

	arch.train();
	arch.test();

	arch.network->save("arch.bnn");
	//arch.network->export_to_dot_graph("arch");
}

/**
 * sanity check for feedforward network activations
 */
void activation_check()
{
	Network* network = new Network();

	Node* tanh = new TanhNode();
	Node* lin1 = new LinearNode();
	Node* pi = new PiNode();
	Node* lin2 = new LinearNode();
	Node* lin3 = new LinearNode();
	Node* pi2 = new PiNode();

	network->add_node(tanh);
	network->add_node(lin1);
	network->add_node(lin2);
	network->add_node(lin3);
	network->add_node(pi);
	network->add_node(pi2);

	network->connect(lin1, lin2, true, 1.0);
	network->connect(lin1, pi, true, 0.5);
	network->connect(lin2, pi, true, 0.25);
	network->connect(pi, lin3, true, 0.25);
	network->connect(pi, tanh, true, 1.0);
	network->connect(lin1, lin3, true, 0.1);
	network->connect(lin2, lin3, true, 1.2);
	network->connect(tanh, lin3, true, 1.3);
	network->connect(tanh, pi2, true, 10.0);

	network->connect(lin3, pi2, true, 0.5);



	/*
	 * pi = (0.5*in * 0.25*in)			-> 0.125
	 * tanh = tanh(pi)					-> 0.124353
	 * lin3 = (0.25)*pi+0.1*in+1.2*in+1.3*tan ->  1.4921
	 * pi2 = 10*tanh * 0.5*lin3		-> 0.928239
	 *
	 *
	 * result = 0.5*(0.25*0.125+1.3+1.3*tanh(0.125))*10*tanh(0.125)
	 */
	double result = 0.928239;

	network->sort_nodes();

	double in[] = {1.0, 1.0};
	Sequence* sequence = new Sequence(in,
			in, 2);

	network->activate(sequence);

	network->debug();

	vector<weight_t>* output = network->get_output(0);
	cout << (*output)[0] << endl;

	if (fabs((*output)[0]-result) > 10e-5) {
		error("Error in Network activations, t=0!");
	}

	delete output;

	output = network->get_output(1);
		cout << (*output)[0] << endl;

		if (fabs((*output)[0]-result) > 10e-5) {
			error("Error in Network activations, t=1!");
		}

}

void gradient_check()
{
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::LINEAR_NODE, 1, 3, 1 ,0,0,0, true, false); //,-1.0,+3.0,-2.0);

	vector<weight_t>* weights = net->get_all_weights();
	(*weights)[0]=0.0346;
	(*weights)[1]=0.053; // in>input
	(*weights)[2]=0.046; // in->input-gate
	(*weights)[3]=-0.035; // in->forget-gate
	(*weights)[4]=-0.157; // in->output-gate
	(*weights)[13] = -0.028;	//peephole in
	(*weights)[14] = -0.037;	//peephole forget
	(*weights)[15] = -0.140;	//peephole out
	(*weights)[17] = 0.149;	// out
	// LSTM rückkoppelung
	(*weights)[18] = -0.094;	// input
	(*weights)[19] = 0.049;	// in
	(*weights)[20] = 0.069;	// forg
	(*weights)[21] = -0.057;	// out


	net->set_all_weights(weights);

	net->sort_nodes();



	if (!net->sanity_check(false))
		error("Network did not pass sanity check!");

	SequenceSet* sequence = new SequenceSet("datasets/gradient-simple2.dat");


	net->check_gradient( (*sequence)[0], 10e-1);
	net->check_gradient( (*sequence)[0], 10e-3);
	net->check_gradient( (*sequence)[0], 10e-5);
	net->check_gradient( (*sequence)[0], 10e-7);
	net->check_gradient( (*sequence)[0], 10e-9);
	net->check_gradient( (*sequence)[0], 10e-9);
	net->check_gradient( (*sequence)[0], 10e-11);

	//net->check_gradient( (*sequence)[0], 10e-7, true);
	//net->check_gradient((*sequence)[0], 10e-8);

	//net->jacobian_numerical( sequence->get(0), 10e-8);

	delete net;
	delete sequence;
}


void debug()
{
	//srand(1272543715);
	srand(1272544015);
	//cout << "Try loading COMP ML" << endl;
	SequenceSet* sequence_set = new SequenceSet("datasets/switch.dat");
	DataSet* dataset = new DataSet(sequence_set);

	Network* network = new LSTMNetwork( dataset->training_set->get_input_size(), 1, dataset->training_set->get_target_size());

//network->add_wta_output_ensemble(network->input_ensemble, network->output_ensemble);

	Trainer* mytrainer =
			//new SARPropTrainer(network);
			new ImprovedRPropTrainer(network);


	//mytrainer->add_abort_criterion( new ConvergenceCriterion(),1 );
	mytrainer->add_callback( new ReportCallback(), 100);

	mytrainer->train(dataset->training_set, 5000);

	mytrainer->plot_errors();
	cout << "ERROR: " << network->test_sequence(dataset->test_set, false) << endl;

	cout << "FERTIG" << endl;

	return;

/*
	cout << "Try loading COMP ML" << endl;
	DataSet* dataset = SequenceSet::load_from_compml_file("/Users/andreas/Downloads/172");

	Network* network = new FeedForwardNetwork( dataset->training_set->get_input_size(), 10, dataset->training_set->get_target_size());

//network->add_wta_output_ensemble(network->input_ensemble, network->output_ensemble);

	Trainer* mytrainer =
			new ARPropTrainer(network);
			//new ImprovedRPropTrainer(network);


	mytrainer->add_abort_criterion( new ConvergenceCriterion(),1 );
	mytrainer->add_callback( new ReportCallback(), 10);

	mytrainer->train(dataset->training_set, 100);

	cout << "ERROR: " << network->test_sequence(dataset->test_set, false) << endl;

	cout << "FERTIG" << endl;

	return;
*/
	/*
	SequenceSet set("datasets/xor.dat");
	Network* network = NetworkFactory::createFeedForwardNetwork(2,6,1);
	network->save("temp.bnn");

	CGTrainer cgt(network);

	cgt.train(&set,&set,30);

	network->test_sequence(&set, true);

	return;*/

	/*
	Sequence* sequence2 = new Sequence();
	sequence2->load_raw_sequence_from_file("datasets/D1short.dat");

	SequenceSet* set = new SequenceSet();
	set->add_sequence(sequence2);
	set->scale_to_minmax(0,1);

//	set->print_sequences();

	DataSet dataset(set);

	vector<weight_t>* i = new vector<weight_t>();
	i->push_back( 1.0 );
	vector<weight_t>* t = new vector<weight_t>();
	t->push_back( 2.0);
	dataset.training_set->get(0)->add(i, t);

	dataset.training_set->print_sequences();
	dataset.validation_set->print_sequences();

	return;*/
/*	unsigned int layers[] = {10};
	Network* network = NetworkFactory::createRecurrentNetwork(2, Node::TANH_NODE,1, layers, Node::TANH_NODE, 2);
	SequenceSet* set = new SequenceSet("datasets/autopredict.dat");

	Trainer* trainer = new ImprovedRPropTrainer(network);

	CallbackHandler* cb =  new ReportCallback();
	trainer->add_callback( cb, 100);
	trainer->train(set, 1000);

	network->save("debug.bnn");

//	network->test_sequence(set, 100);

//	network->get_timeseries_generalization_error(set->get(0),2);

	int mapping[] = {0,1};

	MappedTransferFunction* transfer_function = new MappedTransferFunction();
	AutoPredictor* predictor = new AutoPredictor(network, transfer_function);

	transfer_function->set_mapping(mapping);

	predictor->predict_and_plot(set->get(0),100,1);

	delete(trainer);
	delete(network);
	delete(cb);
	delete(set);
	delete(predictor);
*/

/*
	 Network* network = NetworkFactory::create_random_gate_memory_network(
			1,2,1,  1.0,50);

	 network->save("test.bnn");
	 network->export_to_dot_graph("dot1");
	 cout << "weights original num = " << network->get_num_weights()<<endl;
	 cout << "nodes original num = " << network->nodes.size()<<endl;

	 delete network;

	 network = new Network("test.bnn");
	 network->export_to_dot_graph("dot2");

	 cout << "weights original num = " << network->get_num_weights()<<endl;
	 cout << "nodes original num = " << network->nodes.size()<<endl;

	 network->save("test2.bnn");

	 cout << "DONE" << endl;

	return;
*/
	srand(100);

	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::LINEAR_NODE, 1, 1, 1 ,0,0,0, true, false); //,-1.0,+3.0,-2.0);


	vector<weight_t>* weights = net->get_all_weights();


	/* vector: 0.0346206 0.052654 0.0455438 -0.0354643 -0.157163 1 1 1 1 1 1 1 1 -0.0281884 -0.036754 -0.139946 1 0.14865 -0.0942457 0.0486173 0.0682971 -0.0567869
	 */

	(*weights)[0]=0.0346;

	(*weights)[1]=0.053; // in>input
	(*weights)[2]=0.046; // in->input-gate
	(*weights)[3]=-0.035; // in->forget-gate
	(*weights)[4]=-0.157; // in->output-gate

	(*weights)[13] = -0.028;	//peephole in
	(*weights)[14] = -0.037;	//peephole forget
	(*weights)[15] = -0.140;	//peephole out

	(*weights)[17] = 0.149;	// out

	// LSTM rückkoppelung
	(*weights)[18] = -0.094;	// input
	(*weights)[19] = 0.049;	// in
	(*weights)[20] = 0.069;	// forg
	(*weights)[21] = -0.057;	// out


	net->set_all_weights(weights);

	net->sort_nodes();

//	net->export_to_dot_graph();

	if (!net->sanity_check(false))
		error("Network did not pass sanity check!");





	SequenceSet* sequence = new SequenceSet("datasets/gradient-simple.dat");

	Trainer* trainer = new BackpropTrainer(net);
	trainer->train(sequence,1);
	line();
	net->check_gradient( (*sequence)[0], 10e-5, true);

	net->debug();


	vector<weight_t>* derivs = net->get_all_previous_partial_derivatives();
	cout << "Derivatives:" << endl;
	print_vector(derivs);
	//print_vector(net->get_output(0));
	//print_vector(net->get_output(1));

	vector<Ensemble*>* lstms = net->get_ensembles_by_type(Ensemble::LSTM_FORGET_ENSEMBLE);

	LSTMForgetEnsemble* lstm = (LSTMForgetEnsemble*)(*lstms)[0];

	cout << "Input gate " << endl;
	cout << lstm->get_inputgate()->actbuf[0] << ", "<<  lstm->get_inputgate()->actbuf[1] <<endl;
	cout << lstm->get_forgetgate()->actbuf[0] << ", "<<  lstm->get_forgetgate()->actbuf[1]<<endl ;
	cout << lstm->get_outputgate()->actbuf[0] << ", "<<  lstm->get_outputgate()->actbuf[1] <<endl;
	cout << lstm->get_cec()->actbuf[0] << ", "<<  lstm->get_cec()->actbuf[1]<<endl ;

	cout << "peep_i" << (*derivs)[13] << endl;
	cout << "peep_f" <<(*derivs)[14] << endl;
	cout << "peep_o" <<(*derivs)[15] << endl;

	cout << "peep_i " << net->find_connection(lstm->get_cec(), lstm->get_inputgate())->previous_derivative << endl;
	cout << "peep_f " << net->find_connection(lstm->get_cec(), lstm->get_forgetgate())->previous_derivative << endl;
	cout << "peep_o " << net->find_connection(lstm->get_cec(), lstm->get_outputgate())->previous_derivative << endl;

	Node* out = net->nodes[12];
	Node* in = net->nodes[2];

	cout << "hid->out " << net->find_connection(lstm->get_output(), out)->previous_derivative << endl;

	cout << "in->inpgate" << net->find_connection(in, lstm->get_inputgate())->previous_derivative <<    "(*)" << endl;
	cout << "in->input" << net->find_connection(in, lstm->get_input())->previous_derivative << endl;
	cout << "in->forpgate" << net->find_connection(in, lstm->get_forgetgate())->previous_derivative << endl;
	cout << "in->outgate" << net->find_connection(in, lstm->get_outputgate())->previous_derivative << endl;

	cout << "hid->inpgate" << net->find_connection(lstm->get_output(), lstm->get_inputgate())->previous_derivative << endl;
	cout << "hid->input" << net->find_connection(lstm->get_output(), lstm->get_input())->previous_derivative << endl;
	cout << "hid->forpgate" << net->find_connection(lstm->get_output(), lstm->get_forgetgate())->previous_derivative << endl;
	cout << "hid->outgate" << net->find_connection(lstm->get_output(), lstm->get_outputgate())->previous_derivative << endl;


	net->check_gradient( (*sequence)[0], 10e-3);
	net->check_gradient( (*sequence)[0], 10e-5);
	net->check_gradient( (*sequence)[0], 10e-7);
	net->check_gradient( (*sequence)[0], 10e-9);
	net->check_gradient( (*sequence)[0], 10e-11);

	net->check_gradient( (*sequence)[0], 10e-11, true);
	//net->check_gradient((*sequence)[0], 10e-8);

	//net->jacobian_numerical( sequence->get(0), 10e-8);

	delete net;
	delete sequence;
}

void test_plot()
{
	SequenceSet* sequence = new SequenceSet("datasets/superimposedsin.dat");

	vector<unsigned int> p;
	vector<string> c;

	p.push_back(10);
	c.push_back("#550011");
	p.push_back(50);
	c.push_back("#2222FF");
	p.push_back(100);
	c.push_back("#550011");
	GnuplotGenerator::plot_shaded_curve("Test", &sequence->get(0)->input, &p, &c);

}

double pseudo_output_check(bool condition)
{
	Network* network;

	if (condition) {
		network = new SlowNetwork(1, 20,1, 0.01);
		//network->export_to_dot_graph("cond_slow");
	} else {
		unsigned int sizes[] = {20};
		network = NetworkFactory::createRecurrentNetwork(1,Node::TANH_NODE,1,sizes, Node::LINEAR_NODE,1);
		//network->export_to_dot_graph("cond_rec");
	}

	//SequenceSet* set = new SequenceSet("datasets/simple2.dat");
//	SequenceSet* set = new SequenceSet("datasets/superimposedsin.dat");
	SequenceSet* set = new SequenceSet("datasets/mackey-glass-rk4.dat");
	set->normalize();

	//	SequenceSet* set = new SequenceSet("datasets/switch-50.dat");
	set->get(0)->split_in_training_test_set(0.8);

	//DataSet* dataset = set->toDataSet(0.7,0.15);

	//network->export_to_dot_graph("pseudo_pre");
	Trainer* trainer = new BackpropTrainer(network);
	trainer->learning_rate = 0.0001;
			//new ImprovedRPropTrainer(network);
	trainer->add_callback(new ReportCallback(), 500);
	trainer->train(set, 1000);

	//network->test_sequence( dataset->test_set, true );

//	network->export_to_dot_graph("pseudo");

//	network->save("superimposedsin.bnn");

	double result = network->evaluate_test_error(set);
	delete network;
	delete trainer;
	delete set;

	return result;
	//trainer->plot_errors();
	//return trainer->error_train[trainer->error_train.size()-1];
}

void pseudo_output_check_old()
{
	Network* network = new Network();
	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, 1);
	FeedforwardEnsemble* hidden = new FeedforwardEnsemble(Node::LINEAR_NODE,2);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE,1);
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);
	network->add_ensemble(in);
	network->add_ensemble(hidden);
	network->add_ensemble(out);

	Node* pseudo = new PseudoOutputNode();
	network->add_node(pseudo);
	network->connect( hidden->nodes[1], pseudo, true);

	network->sanity_allows_deadends = true;

	network->sort_nodes(in, out);

	SequenceSet* set = new SequenceSet("datasets/simple2.dat");
	network->export_to_dot_graph("pseudo_pre");
	Trainer* trainer =// new BackpropTrainer(network);
	//trainer->learning_rate = 0.01;
			new ImprovedRPropTrainer(network);
	trainer->add_callback(new ReportCallback(), 2000);
	trainer->train(set, 10);

	network->test_sequence( set, true );

	network->export_to_dot_graph("pseudo");
}


void train_clstm()
{
	SequenceSet* set = new SequenceSet("datasets/pfg-cos.dat");

	Network* net = NetworkFactory::createCLSTM(
	 set->input_size,
	 5,
	 set->target_size
	);

	//net->export_to_dot_graph();

	net->sort_nodes();

	set->scale_to_minmax(-0.9, +0.9);

	Trainer* trainer = new RPropTrainer(net);

//	Trainer* trainer = new BackpropTrainer(net);
	trainer->learning_rate = 0.0001;

	trainer->train(set,1000);

//	net->export_to_dot_graph();

	net->save("pfg.net");

	delete set;
	delete net;
}

void generate()
{
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::LINEAR_NODE, Node::LINEAR_NODE, 1, 2, 1 ,-0.1,0.1,-0.1, true, false); //,-1.0,+3.0,-2.0);

	net->sort_nodes();

	vector<weight_t>* weights = net->get_all_weights();

	SequenceSet* set = new SequenceSet();
	Sequence* seq = new Sequence();
	vector<double>* input = new vector<double>();
	//vector<double>* target = new vector<double>();
	for (unsigned int i=0; i < 1; i++) {
	double r = rand_gauss(0,0.1);
	 net->reset();
	 input->push_back(r);
	 net->_activate(input);
	 vector<double>* output = net->get_output(net->timestep-1);
	 seq->add(input, output);
	 cout << input << " -> " << output << endl;
	}
	set->add_sequence(seq);

	cout << "training"<<endl;

	net->randomize();
	Trainer* trainer = new RPropTrainer(net);
	trainer->train(set, 1000);

	print_vector(weights);
	print_vector(net->get_all_weights());



	delete net;
	delete set;
}

void train_prime()
{
	unsigned int layers[1] = {80};
	unsigned int num = 1;
	Network* net = NetworkFactory::createFeedForwardNetwork(8,Node::TANH_NODE,num, layers, Node::SIGMOID_NODE, 1);

	SequenceSet* data = new SequenceSet("datasets/prime.dat");

	Trainer* trainer = new ImprovedRPropTrainer(net);

	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.5);
	net->error_function = err_func;

	trainer->add_callback( new ReportCallback(), 100 );

	trainer->train(data, 20000);

	net->test_sequence( data, true);

	net->save("prime.bnn");

	delete net;
}

/**
 *
 *
 * trying
 *  	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 1, 5, 1 ,0.1,0.1,0.1, true)

	 the best result was given by:

	 3000 epochs, lr=0.001, batch=false

--------------

 * 3 nodes
 * 1257967227....0.451299 0.584402
 * 1257967442....1.0439 1.95228
 * 1257967728....0.351381 0.686517
 * 1257967899....0.310158 1.12911
 * 1257974108....0.506021 1.45533
 * 1258014104....0.500689 0.549913
 *
 *
 *
 *2 nodes
 *1257973265....0.377736 0.554114
 *
 *---------------------
 *
 *	SequenceSet* input = new SequenceSet("datasets/mackey-glass-5000.dat");
	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 1, 2, 1,-1.0,+1.0,-1.0, true);
 */

/*lr = 0.0004, bias -1,+3,-2
 *
 * srand 1234:	improvedRprop
 * backprop+initial states: 161 (reproduced 1x)
 * backprop vanilla: no convergence after 2000 steps
 * (scheitert offenbar daran die I=0 Bedingung darzustellen!
 *
 * srand 9988, 12345,:
 * backprop is: (terminated by user - no convergence?)
 * backprop vanilla :
 *
 * 5612345	[903 error items]
 * kein Erfolg mit [initial states]
 * - lr=0.0005
 * - lr=0.0009
 * - lr=0.0004, mom=0.999
 * - lr=0.0004, mom=0.9
 * - lr=0.0002, mom=0.9
 * - lr=0.0001
 *
 *
 * (vanilla)
 * 4315572385 iprop nach 1000 EPochen: 183 error items (not normalized)
 * 										83	(normalized)
 *
 *
 *
 *
 * norep
 *
 * 4315572385
 *
 * vanilla iProp <101 epochs
 * vanilla backprop alpha=0.0008 <361
 * 						  0.008	 no >1000
 * is 		backprop alpha=0.008 no >1000
 *
 * vanilla backprop alpha=0.0004 <521
 * is		backprop			no >1000
 *
 *
 * 114315575
 *
 * vanilla backprop alpha=0.0004 <481
 *						  0.0008 <481
 *						  0.0008, mom=0.9 <281
 *
 *
 *						  1,2,1,-0.5,-0.5,-0.5
 *						 1421 vanilla
 */

void train_msd()
{

	MSDTask task;
	task.init();
	task.train();

	return;
}


double train_parity(bool initial_state)
{

 	Network* net = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 2, 1, 1 ,0.0,0.0,0.0, true, false); //,-1.0,+3.0,-2.0);
	net->sort_nodes();

	SequenceSet* training_set = new SequenceSet("datasets/parity-noisy.dat");


	training_set->scale_to_minmax(-1,+1);

	if (initial_state) {
		net->use_initial_states = true;
		net->randomize_initial_states();
	}

	Trainer* trainer = new BackpropTrainer(net);
			//new ImprovedRPropTrainer(net); //new BackpropTrainer(net);
	trainer->learning_rate = 0.01;
	//trainer->batch_learning = true;

	//CallbackHandler* callback = new DummyCallback();
	//print_vector( net->get_all_initialstates() );
	Criterion* bc = new BinaryClassificationSuccess(training_set);
	trainer->add_abort_criterion(bc,1);

	//print_vector( net->get_all_weights() );
	//trainer->add_callback()
	trainer->train(training_set, training_set, 5000);
	print_vector( net->get_all_weights() );

	net->test_sequence( training_set , false);

	//print_vector( net->get_all_initialstates() );
	double correct = net->get_percent_correct_binary_decision( training_set , 0)*100.0;
	cout << "Correct: " << correct << "% Iterations: "<<trainer->total_iterations<< endl;

	unsigned int l = trainer->total_iterations;

	delete trainer;
	delete training_set;
	delete net;

	return l;

}


void train_undef()
{
	unsigned int hid[1] = {2};
	Network* net = NetworkFactory::createFeedForwardNetwork(1,
	Node::LINEAR_NODE, 1, hid, Node::LINEAR_NODE, 1);

	SequenceSet* training_set = new SequenceSet("datasets/undef.dat");

//	net->export_to_dot_graph();

	training_set->scale_to_minmax(1,2);

	training_set->print_sequences();

	Trainer* trainer = new BackpropTrainer(net);

	trainer->learning_rate = 0.01;

//	trainer->train(training_set, 10000);

	net->test_sequence( training_set, true);

	delete net;
	delete training_set;

}

void simple_generator()
{
	Network* network = new Network();
	HarmonicGenerator* hg = new HarmonicGenerator(Node::TANH_NODE,3);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE,1);
	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1);

	network->add_ensemble(hg);
	network->add_ensemble(out);
	network->add_ensemble(bias);
	network->connect_ensembles(hg, out, true, 1.0);
	network->connect_ensembles(bias, hg, true, 1.0);

	vector<Node*>* empty = new vector<Node*>();
	network->sort_nodes(empty, &out->nodes);

	network->export_to_dot_graph("simple");

	SequenceSet* sequence_set = new SequenceSet();
	Sequence* seq = new Sequence();

	vector<weight_t>* input = new vector<weight_t>();
	vector<weight_t>* target = new vector<weight_t>();
	seq->add(input, target);
	sequence_set->add_sequence(seq);

/*	for (unsigned int i=0; i < 3; i++)
	{
		bias->nodes[0]->outgoing_connections[i]->weight = 0.5;
	}
*/
	AutoPredictor* predictor = new AutoPredictor(network);
	predictor->predict_and_plot( sequence_set->get(0),100, 1);

}

void test_superimposed()
{
	Network* net = new Network("superimposedsin.bnn");
	SequenceSet* input = new SequenceSet("datasets/superimposedsin.dat");

	input->get(0)->split_in_training_validation_test_set(0.5,0.25);

	input->scale_to_minmax(-1,+1);

	//net->test_sequence(input, true);
	//net->export_to_dot_graph("sin");
//	input = input->embed(3);

	GnuplotGenerator::plot_input_output_target(net, input->get(0));
//	GnuplotGenerator::plot_input_target(net, input->get(0));

//	GnuplotGenerator::plot_all_activations(net, input->get(0));

/*	cout << "GE : " << net->get_timeseries_generalization_error(
			input->get(0), 1);
*/
	//AutoPredictor* predictor = new AutoPredictor(net);
	//predictor->predict_and_plot( input->get(0),800, 500);

	double total_error = net->evaluate_error(input, new SquaredErrorFunction() );
	double training_error = net->evaluate_training_error( input );
	double validation_error = net->evaluate_validation_error( input );
	double test_error = net->evaluate_test_error( input );

	cout << "Total Error: " <<  total_error << endl;
	cout << "Training: " << training_error <<" Validation: " << validation_error << " Test: " << test_error << endl;


	// choose Sequence
	Sequence* sequence = input->get(0);

	// plot activations over time
	net->activate( sequence );

	// generate animation
/*	cout << net->ensembles.size() << endl;

		vector<Node*> outputs;
		for (unsigned int i=0; i < net->ensembles[e]->nodes.size(); i++) {
			outputs.push_back( net->ensembles[e]->nodes[i] );
		}
		vector<vector<weight_t>*>* data = net->collect_node_outputs(outputs);
		cout << "weiter" << endl;
		GnuplotGenerator::animate(data, &sequence->input, 10);
*/
	unsigned int e = 1;//net->ensembles.size()-1;
	GnuplotGenerator::plot_activations(net, sequence, &net->ensembles[e]->nodes);

	vector<Node*>* nodes = net->get_nodes_with_name("hgout");
	GnuplotGenerator::plot_activations(net, sequence, nodes);

//	GnuplotGenerator::plot_output_target_difference(net, input->get(0));

//	GnuplotGenerator::plot_all_lstm_activations(net, input->get(0));

//	net->get_timeseries_generalization_error( input->get(0), 4000);

	delete net;
	delete input;
}

void train_hgn()
{
	HarmonicGateNetwork* network = new HarmonicGateNetwork(2, 1, 1, 1);
	SequenceSet* sequence_set = new SequenceSet("datasets/hgn.dat");

	sequence_set->scale_to_minmax(-1,+1);

	Trainer* trainer = new ImprovedRPropTrainer(network);

//	network->add_generator();


//	return;

	CallbackHandler* cb = new SaveNetworkCallback("hgn");
	trainer->add_callback(cb, 200);

	CallbackHandler* rp = new ReportCallback();
	trainer->add_callback(rp, 10);

	trainer->add_abort_criterion(new ConvergenceCriterion(10e-7), 10);

	cout << "Start" << endl;

	trainer->train(sequence_set, 1000);

	for (unsigned int i=0; i < 5; i++) {

		//net->freeze_all_generators();
		network->add_generator();
		trainer->reset();
		trainer->train(sequence_set, 1000);
	}

	network->save("hgn");

}

void test_hgn()
{
	Network* network = new Network("hgn.bnn");
	SequenceSet* sequence_set = new SequenceSet("datasets/hgn2.dat");

	network->export_to_dot_graph("hgn");

	sequence_set->scale_to_minmax(-1,+1);

	vector<Node*>* nodes = new vector<Node*>();
	nodes->push_back(network->nodes[7]);
	nodes->push_back(network->nodes[10]);
	nodes->push_back(network->nodes[13]);
	nodes->push_back(network->nodes[15]);
	GnuplotGenerator::plot_activations(network, sequence_set->get(0),  nodes);


	GnuplotGenerator::plot_input_output_target(network, sequence_set->get(0));
//	GnuplotGenerator::plot_input_output_target(network, sequence_set->get(1));

	delete network;
	delete sequence_set;
}

void experimental_random_gate_parity_noise()
{
	Network* network = NetworkFactory::create_random_gate_network(2,5,1,
			1.0,10);

	SequenceSet* training_set = new SequenceSet("datasets/parity-noisy.dat");

	training_set->scale_to_minmax(-1,+1);

	Trainer* trainer = new ImprovedRPropTrainer(network);
			//new BackpropTrainer(network);
	trainer->learning_rate = 0.01;
	trainer->batch_learning = true;

	trainer->train(training_set, 1000);

	trainer->abort_criteria.push_back( new BinaryClassificationSuccess(0) );
	//network->export_to_dot_graph();

	network->test_sequence( training_set, true );

	delete network;
	delete training_set;
	delete trainer;
}

void proben()
{
//	SequenceSet* test = new SequenceSet();
	DataSet* data = SequenceSet::load_from_proben_file("/home/brandmaier/Documents/proben1/diabetes/diabetes1.dt");

	Network* network = NetworkFactory::create_random_gate_network(
			data->get_input_size(), 10, data->get_target_size(),
			1.0,0
	);

//	cout << "in size: " << network->in_size <<" " << data->get_input_size() <<endl;

	Trainer* trainer = new ImprovedRPropTrainer(network);

	//trainer->abort_criteria.push_back( new EarlyStopping() );

	trainer->batch_learning=true;
	trainer->train(data->training_set,data->test_set,500);

	network->test_sequence( data->test_set,true);

	delete network;
	delete trainer;


}

/*
 * works for
 * 4 LSTM cells, alpha=0.01, mom=0.9
 *
 *
 */
unsigned int grammar(bool initial_state)
{
/*	Network* network = NetworkFactory::create_random_gate_network(4,10,4,
			1,50);
*/


/*	Network* network = NetworkFactory::createLSTMWTANetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
		Node::TANH_NODE, 4,10, 4, -0.5,0.5,-0.5, true);
*/

 /*	Network* network = NetworkFactory::createLSTMHiddenLayerNetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
	 Node::TANH_NODE, Node::TANH_NODE, 4, 4, 4 ,1,-1,1, true, true); //,-1.0,+3.0,-2.0);
*/
//	srand(1259059007);	//12387
//	srand(1259089975);
//srand(1259090716);
//	srand(1259091121);
	//initial_state = true;

	LSTMNetwork* network = new LSTMNetwork(
		4,3,4,
		Node::TANH_NODE,Node::TANH_NODE,
		0.51,-0.51,0.51
	);

//	initial_state = true;
/*	srand(1259494427);
	Network* network = NetworkFactory::create_random_gate_memory_network(
			4,25,4,  0.70,50);
*/

//	network->sort_nodes();

	// 191 epochs		washout 10
	// 61				washout 0
	// 121 epochs		washout 0
	// 161						10
	// 231						100
	network->washout_time =0;

//	network->add_wta_output_ensemble( network->ensembles[network->ensembles.size()-1]);

	SequenceSet* set = new SequenceSet("datasets/grammar-train.dat");

	SequenceSet* training_set = set->create_subset(0,12);
	SequenceSet* test_set = set->create_subset(15,set->size());


	//training_set->scale_to_minmax(-0.9,+0.9);
	//training_set->normalize();

	Trainer* trainer = //new BackpropTrainer(network);
			new ImprovedRPropTrainer(network);
			//new LPropTrainer(network);

	//cout << network->out_size << endl;
	if (initial_state) {
		network->use_initial_states = true;
		network->randomize_initial_states();
	}

	//trainer->batch_learning = true;
//	training_set->shuffle();

	trainer->learning_rate = 0.001;
	//trainer->learning_rate_discount=1;
	trainer->momentum = 0.9; //0.999;
//	trainer->batch_learning = true;

	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.3);

	network->error_function = err_func;

	trainer->add_abort_criterion(
			new ZeroErrorCriterion(
					err_func
			), 10
	);

	trainer->add_callback(
			new ReportCallback(), 50
	);

	cout << "calc" <<endl;
	cout << "weights: " << network->get_num_weights() <<endl;
	cout << "weights: " << network->get_num_trainable_weights() <<endl;

	//for (unsigned int i=0; i < 7;i++) {
		//network->add_lstm_cell();
		trainer->train(training_set, 5000);
	//}
		network->save("grammar.bnn");

	//GnuplotGenerator::plot_vector("Training", &trainer->error_train);

	//network->test_sequence( training_set, true );

	//cout << "training-set: " << network->evaluate_error(training_set, err_func)<<endl;
	cout << "test-set: " << network->evaluate_error(test_set, err_func)<<endl;
	cout << "Epochs:" << trainer->total_iterations << endl;


	unsigned int tot_it = trainer->total_iterations;

	/*delete network;
	delete trainer;
	delete set;
	delete err_func;
*/
	return tot_it;
}

void grammar_test()
{
	Network* network = new Network("grammar.bnn");
	SequenceSet* set = new SequenceSet("datasets/grammar-train.dat");

	SequenceSet* training_set = set->create_subset(0,12);
	SequenceSet* test_set = set->create_subset(12,20);

	//training_set->scale_input_to_minmax(0,1);
	//training_set->scale_to_minmax(-0.9,+0.9);


	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(1);
	network->error_function = err_func;

	network->test_sequence( training_set, true );

	//cout <<network->test_sequence( training_set, false);
	cout << "Error on training set : " << network->evaluate_error( training_set,err_func )<<endl;
	cout << "Error on test set : " << network->evaluate_error( test_set, err_func )<<endl;

//	network->activate(training_set->get(0));
//	print_vector(network->get_outputs_until( 4 ));

/*	GnuplotGenerator::plot_all_cec_activations(network, training_set->get(6));
	GnuplotGenerator::plot_all_cec_activations(network, training_set->get(8));
	GnuplotGenerator::plot_all_cec_activations(network, training_set->get(7));
*/
/*	GnuplotGenerator::plot_output_target_difference(network, training_set->get(6));
	GnuplotGenerator::plot_output_target_difference(network, training_set->get(8));
	GnuplotGenerator::plot_output_target_difference(network, training_set->get(9));
*/
}

void experimental()
{
	Network* network = NetworkFactory::create_random_gate_network(1,10,1,
			1,50);

	SequenceSet* training_set = new SequenceSet("datasets/mackey-glass.dat");

	training_set->scale_to_minmax(-1,+1);
//	training_set->normalize();

	Trainer* trainer = new ImprovedRPropTrainer(network);
			//new BackpropTrainer(network);
	trainer->learning_rate = 0.0001;
//	trainer->momentum = 0.999;
	trainer->batch_learning = true;

	trainer->abort_criteria.push_back( new ConvergenceCriterion(10e-7) );


	trainer->train(training_set, 2000);

//	trainer->abort_criteria.push_back( new BinaryClassificationSuccess(0) );
	//network->export_to_dot_graph();

	GnuplotGenerator::plot_vector("Training Error", &trainer->error_train);

	network->test_sequence( training_set, true );

	network->save("mackeyglass.bnn");

	delete network;
	delete training_set;
	delete trainer;
}

void experimental_msd_broken()
{
	Network* network = NetworkFactory::create_random_gate_network(1,10,1,
			1,25);

	SequenceSet* training_set = new SequenceSet("datasets/msd-train-F20-0-10.dat");

//	training_set->scale_to_minmax(-1,+1);
//	training_set->normalize();

	Trainer* trainer = new ImprovedRPropTrainer(network);
			//new BackpropTrainer(network);
	trainer->learning_rate = 0.000001;
//	trainer->momentum = 0.999;
//	trainer->batch_learning = true;

	trainer->abort_criteria.push_back( new ConvergenceCriterion(10e-7) );


	trainer->train(training_set, 500);

//	trainer->abort_criteria.push_back( new BinaryClassificationSuccess(0) );
	//network->export_to_dot_graph();

	GnuplotGenerator::plot_vector("Training Error", &trainer->error_train);

	network->test_sequence( training_set, true );

	network->save("msd.bnn");

	delete network;
	delete training_set;
	delete trainer;
}

void experimental_superimposedsin()
{
	Network* network = NetworkFactory::create_random_gate_network(1,10,1,
			1.0,40);

	SequenceSet* training_set = new SequenceSet("datasets/superimposedsin.dat");

	training_set->scale_to_minmax(-1,+1);

	Trainer* trainer = new ImprovedRPropTrainer(network);
			//new BackpropTrainer(network);
	trainer->learning_rate = 0.01;
	trainer->batch_learning = true;

	trainer->train(training_set, 2000);

	trainer->abort_criteria.push_back( new BinaryClassificationSuccess(0) );
	//network->export_to_dot_graph();

	network->test_sequence( training_set, true );

	network->save("superimposedsin.bnn");

	delete network;
	delete training_set;
	delete trainer;
}


void systematic_test_parity_noise()
{
	unsigned int repetitions = 50;

	vector<unsigned int> epochsA;
	vector<unsigned int> epochsB;


	for (unsigned r=0; r < repetitions; r++)
	{
		unsigned int seed = (unsigned int)(3.14529*time(NULL));

		srand(seed);
		double errorA = train_parity(true);
		srand(seed);
		double errorB = train_parity(false);

		epochsA.push_back(errorA);
		epochsB.push_back(errorB);
	}

	// cleanup

	cout << "group values " <<endl;
	for (unsigned int i=0; i < epochsA.size(); i++)
	{
		cout << epochsA[i] << "\t" << epochsB[i] << endl;
	}

	cout << "Mean A : " <<mean(&epochsA)<<endl;
	cout << "Mean B : " <<mean(&epochsB)<<endl;


}


void speed_check()
{
	LSTMNetwork* network = new LSTMNetwork(1,10,1);
	SequenceSet* sequence_set = new SequenceSet("datasets/mackey-glass-rk4.dat");
	//Trainer* trainer = new ImprovedRPropTrainer(network);
	Trainer* trainer = new BackpropTrainer(network);

	message("Speed Check started");
	time_t time_start = time(NULL);

	trainer->train(sequence_set, 10);

	time_t time_end = time(NULL);

	double time_per_epoch = (time_end-time_start)/10.0;

	message("Speed Check finished");
	cout << "time per epoch: " << time_per_epoch << "s" << endl;

	delete network;
	delete sequence_set;
	delete trainer;


}

void sfn_test()
{
	unsigned int embed = 8;

	Network* network = new Network("sfn.bnn");
	SequenceSet* sequence_set = new SequenceSet("datasets/tentmap.dat");

	SequenceSet* newset = new SequenceSet();

	for (unsigned int i=0; i < 10; i++) {

	Sequence* seq =	new AutoAssociationSequence(
				sequence_set->get(i)->embed(8)
					);


	newset->add_sequence(seq);

	}

	network->activate( newset->get(0) );

//	vector<Ensemble*>* slow = network->get_ensembles_by_type(Ensemble::SUBTRACTIVE_ENSEMBLE);
	//vector<Ensemble*>* slow = network->

	//vector<Node*> nodes =
	/*if (slow->size() == 0) {
		error("Could not find Ensemble!");
	}*/
	unsigned int ns = network->nodes.size();
	/*//for (unsigned int i=0; i < (*slow)[0]->nodes.size(); i++)
		nodes.push_back( (*slow)[0]->nodes[ns-2] );
		*/
	/*vector<weight_t> slow_node_act = (*slow)[0]->nodes[0]->actbuf;
	vector<vector<weight_t>* >* slowvec = new vector<vector<weight_t>* >();
	for (unsigned int i=0; i < slow_node_act.size(); i++) {
		vector<weight_t>* temp = new vector<weight_t>();
		temp->push_back( slow_node_act[i]);
		slowvec->push_back( temp );
	}
	vector<vector<weight_t>*> targetvec = sequence_set->get(0)->target;
*/


	//GnuplotGenerator::plot_input_target(network, sequence_set->get(0));
	GnuplotGenerator::plot_activations(network,
			newset->get(0), &network->nodes );

	//GnuplotGenerator::plot_vector("Target", &targetvec);
	//GnuplotGenerator::plot_vector("test", &slow_node_act);


//	GnuplotGenerator::plot_vector("",slowvec,"slow",
//			&targetvec, "target");

	// calculate correlations between signals

	unsigned int max_index = -1;
	double max_value = 0;

	//vector<vector<weight_t>*>* tt = transpose(&newset->get(0)->target);
	vector<vector<weight_t>*>* tt = transpose(&sequence_set->get(0)->target);

	vector<weight_t>* row = (*tt)[0];
	for (unsigned int i=0; i < embed-1; i++)
		row->erase(row->end()-1);

	for (unsigned int i=0; i< ns; i++ ) {
		double cv = corr(&network->nodes[i]->actbuf, (*tt)[0] );
		cout << "Correlation: " << cv << endl;
		if (abs(cv) > abs(max_value)) {
//		if (cv > max_value) {
			max_index = i;
			max_value = cv;
		}
	}

	cout << "Max Value " << max_value << endl;
	vector<weight_t> slow_node_act2 = network->nodes[max_index]->actbuf;
	GnuplotGenerator::plot_vectors("",&slow_node_act2,"slow",
			(*tt)[0], "target");

	delete network;
}

/**
 * Slow Feature Network
 */
void sfn()
{
	unsigned int embed = 8;

	unsigned int in_size = embed;
	unsigned int hid_size = 16;
	unsigned int out_size = embed;

	Network* network = NetworkFactory::create_slow_feature_network(
			in_size, hid_size, out_size);

	//Network* network = new RecurrentNetwork(in_size, hid_size, out_size);

	SequenceSet* sequence_set = new SequenceSet("datasets/tentmap.dat");

	SequenceSet* newset = new SequenceSet();

	for (unsigned int i=0; i < sequence_set->size(); i++) {

	Sequence* seq =	new AutoAssociationSequence(
				sequence_set->get(i)->embed(embed)
					);



	newset->add_sequence(seq);

	}

	CallbackHandler* sn = new SaveNetworkCallback("sfn");


	Trainer* trainer = new ImprovedRPropTrainer(network);
	trainer->add_callback( new ReportCallback(),100);
	trainer->add_callback( sn , 100);
	trainer->train(newset, 3000);

	//network->test_sequence(newset, true);

	network->save("sfn.bnn");

	vector<Node*> nodes;
	nodes.push_back(network->nodes[0]);

	vector<Ensemble*>* slow = network->get_ensembles_by_type(Ensemble::SLOW_ENSEMBLE);
	//vector<Node*> nodes =
	unsigned int ns = (*slow)[0]->nodes.size();
	//for (unsigned int i=0; i < (*slow)[0]->nodes.size(); i++)
		nodes.push_back( (*slow)[0]->nodes[ns-2] );

	GnuplotGenerator::plot_activations(network, newset->get(0), &nodes);

	//GnuplotGenerator::plot_input_target(network, sequence_set->get(0));

	vector<vector<weight_t>*> targetvec = sequence_set->get(0)->target;

	delete network;
	delete sequence_set;
}

void systematic_test()
{
	unsigned int repetitions = 1;

	vector<double> epochsA;
	vector<double> epochsB;
	vector<unsigned int> rands;

	unsigned int seed = 0;

	for (unsigned r=0; r < repetitions; r++)
	{
		seed = (unsigned int)(seed+time(NULL));
		rands.push_back(seed);



		srand(seed);
		message("slow training");
		//double error = train_mackeyglass(true);
		//double error = switch_train(true);
		//double error = grammar(true);
		//double error = train_superimposedsins(true);
		//double error = pseudo_output_check(true);
		//TentmapTask task;
		MSDTask task;
		task.parameter_set.set("network",0);
		task.parameter_set.set("slowness",true);
		task.init();
		task.train();
		//double error = task.network->evaluate_test_error(task.dataset->test_set);
		double error= task.test_error;

		srand(seed);
		message("vanilla recurrent");
		//double error2 = train_mackeyglass(false);
		//double error2 = switch_train(false);
		//double error2 = grammar(false);
		//double error2 = train_superimposedsins(false);
		//double error2 = pseudo_output_check(false);
		//TentmapTask task2;
		MSDTask task2;
		task2.parameter_set.set("network",1);
		task2.parameter_set.set("slowness",false);
		task2.init();
		task2.train();
		//double error2 = task2.network->evaluate_test_error(task2.dataset->test_set);
		double error2 = task2.test_error;

		epochsA.push_back( error );
		epochsB.push_back( error2 );

		stringstream result;
		result << seed << "...." <<error << " " << error2;
		message(result.str());


	}

	cout << "group values " <<endl;
	cout << "SLOW\tVanilla\tseed\n";
	for (unsigned int i=0; i < epochsA.size(); i++)
	{
		cout << epochsA[i] << "\t" << epochsB[i] << "\t"<< rands[i]<< endl;
	}

	double ma = mean(&epochsA);
	double mb = mean(&epochsB);
	double va = var(&epochsA);
	double vb = var(&epochsB);
	double n = epochsA.size()*2;

	cout << "Mean slow : " <<mean(&epochsA) << "," << var(&epochsA) <<endl;
	cout << "Mean VA : " <<mean(&epochsB)<< "," << var(&epochsB) << endl;
	cout << "effect size: " << (ma-mb) / sqrt((va+vb)/n) << endl;
	cout << "df = " << (n-2) << endl;
}

void slow_associations()
{
	SequenceSet* sequence_set = new SequenceSet("datasets/slow.dat");

	unsigned int in_size = 1;
	unsigned int out_size = sequence_set->get_target_size();
	unsigned int hidden_size = 20;

	Network* network = new Network();
	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	Ensemble* hidden = new SlowEnsemble(Node::LINEAR_NODE, hidden_size, 0.01, true);
	network->add_ensemble(in);
	network->add_ensemble(hidden);
	network->add_ensemble(out);

	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);

	network->sort_nodes(in, out);

	Trainer* trainer = new ImprovedRPropTrainer(network);

	trainer->train(sequence_set, 1000);

	network->save("slow.bnn");

	delete trainer;
	delete network;
	delete sequence_set;
}


void test_msd()
{
	MSDTask task;
	task.restore();
	task.test();

	return;

	Network* net = new Network("msd.bnn");
	SequenceSet* input = new SequenceSet();
	try {
		//input->load_from_file("datasets/msd-train-F20-0-10.dat");
		input->load_from_file("datasets/nmsd-train-F30-0-10.dat");
	} catch (const int e) {
		std::cout << "File not Found!" << endl;
		exit(-1);
	}

	if (!net->sanity_check(false))
		error("Network did not pass sanity check!");
	//input->normalize();
	//input->scale_to_minmax(-1,+1);

	ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.5);
/*	net->test_sequence( input, true );
	vector<vector<weight_t>*>* outs = net->get_outputs_until(net->timestep-1);
	GnuplotGenerator::plot_vector("Outputs", outs);
*/
	//GnuplotGenerator::plot_input_output_target(net, input->get(0));
	//GnuplotGenerator::plot_input_output_target(net, input->get(1));
	//GnuplotGenerator::plot_input_output_target(net, input->get(2));
/*	GnuplotGenerator::plot_input_output_target(net, input->get(9));
	GnuplotGenerator::plot_input_output_target(net, input->get(5));
	GnuplotGenerator::plot_input_output_target(net, input->get(11));
	GnuplotGenerator::plot_input_output_target(net, input->get(12));
	GnuplotGenerator::plot_input_output_target(net, input->get(1));
	GnuplotGenerator::plot_input_output_target(net, input->get(3));
*/
	//	GnuplotGenerator::plot_output_target_difference(net, input->get(9));

	/*for (unsigned int i=0; i < 4; i++)
	{
		GnuplotGenerator::plot_output_target_difference(net, input->get(i));
	}*/

//	GnuplotGenerator::plot_all_cec_activations(net, input->get(0));

	GnuplotGenerator::plot_all_activations(net, input->get(0));

	net->reset();
	net->activate(input->get(1));
	vector<Node*> outputs;
	for (unsigned int i=0; i < net->out_size; i++) {
		outputs.push_back( net->nodes[net->out_offset+i] );
	}
	vector<vector<weight_t>*>* data = net->collect_node_outputs(outputs);
	GnuplotGenerator::animate(data, &input->get(1)->input, 1);

	net->test_sequence(input, true);

	cout << "Total Threshold Error:"<< net->evaluate_error(input,err_func) << endl;

	cout << "DONE" << endl;

	//net->jacobian_numerical( input->get(0),10e-9);

	delete net;
	delete input;

}

/*
void hmm()
{
	HMM* hmm = new HMM(3, 1);
	vector<StateObservation*>* sample = hmm->sample(10);
	for (unsigned int i=0; i < sample->size();i++)
	{
		cout << i << ": " << (*sample)[i]->state << endl;
	}
	HMM::test();
}*/

void freq_train()
{
	ClassifyFrequencyTask task;
	task.init();
	task.train();
}

void freq_test()
{
	ClassifyFrequencyTask task;
	task.restore();
	task.test();
}

void debug_exponentialembedding()
{
	SequenceSet* sequence_set = new SequenceSet("datasets/pfg-cos.dat");
	SequenceSet* emb = sequence_set->embed(3);
	Sequence* sequence = emb->get(0);
	unsigned int ls[] = {10};
	Network* network = NetworkFactory::createRecurrentNetwork(3,Node::TANH_NODE,1,ls, Node::TANH_NODE, 1);

	TransferFunction* transfer = new ExponentiallyEmbeddedTransferFunction(4);

	AutoPredictor predictor(network, transfer);

	predictor.predict_and_plot(sequence, 2, 1);

}

void train_activity()
{

	Network* net = NetworkFactory::createLSTMWTANetwork(Ensemble::LSTM_FORGET_ENSEMBLE,
		Node::TANH_NODE, 4,100, 5, 0,0,0, true);
	//unsigned int hid[1] = {80};
	//unsigned int hidsize = 1;
//	Network* net = NetworkFactory::createRecurrentWTANetwork(4,Node::TANH_NODE, hidsize,hid,  5);
	SequenceSet* input = new SequenceSet("ExperimentActivity/activity.dat");

	net->sort_nodes();
//	net->sanity_check(true);

//	net->export_to_dot_graph();


/*	ErrorFunction* err_func = new CrossEntropyErrorFunction();
	net->error_function = err_func;
*/

	Trainer* trainer = new MyRPropTrainer(net);
			//new ImprovedRPropTrainer(net);

	trainer->learning_rate = 1e-5;
	trainer->momentum = 0.9;

	//ErrorFunction* err_func = new ThresholdSquaredErrorFunction(0.2);
	ErrorFunction* err_func = new WinnerTakesAllErrorFunction();
	Criterion* abort = new ErrorBelowCriterion(err_func, 0.0);

	trainer->add_abort_criterion(abort, 50);

	//CallbackHandler* callback = new DebugCallback();
	//input->normalize(true, false);
	//input->scale_input_to_minmax(-1.0,1.0);
	ReportCallback* rp = new ReportCallback();
	trainer->add_callback(rp, 10);

	CallbackHandler* save = new SaveNetworkCallback("tempactivity");
	trainer->add_callback(save, 10);

	trainer->train(input,input,10000);
	vector<double>* errors = &trainer->error_train;
	GnuplotGenerator::plot_vector("Training Error", errors);

	//net->test_sequence_wta(input, true);

	net->save("activity.bnn");

	delete net;
	//delete errors;
	delete input;

	//test_activity();
}


void corr2()
{

	LSTMNetwork* network = new LSTMNetwork(2,2,2);
	network->sort_nodes();
	SequenceSet* set = new SequenceSet("datasets/corr2.dat");

	Trainer* trainer = new ImprovedRPropTrainer(network);
			//new BackpropTrainer(network);

	vector<Node*>* nodes = new vector<Node*>();
	for (unsigned int i=0; i < network->lstms.size(); i++)
	{
		nodes->push_back( network->lstms[i]->get_output() );
	}
	trainer->decorrelation_nodes = *nodes;

	trainer->decorrelation_enabled = true;

	trainer->add_callback( new ReportCallback(), 10);
	trainer->train(set, 1000);

	delete network;
	delete set;

}


/*
 * Feedforward-Netz mit linearen Einheiten.
 *
 * 	o o
 * 	|x|
 * 	o o
 *	|x|
 * 	o o
 *
 */
void corr()
{
	//unsigned int layers[] = {2};
	Network* network = new Network(); //NetworkFactory::createFeedForwardNetwork(2, Node::LINEAR_NODE, 1, layers, Node::LINEAR_NODE, 2);

	LinearNode ln1;
	LinearNode ln2;
	LinearNode ln3;
	LinearNode ln4;

	network->add_node( &ln1 );
	network->add_node(&ln2);
	network->add_node(&ln3);
	network->add_node(&ln4);

	Network::connect(&ln1, &ln3, true);
	Network::connect(&ln1, &ln4, true);
	Network::connect(&ln2, &ln3, true);
	Network::connect(&ln2, &ln4, true);

	network->sort_nodes();

	SequenceSet* set = new SequenceSet("datasets/corr.dat");

	network->export_to_dot_graph("corr_oirg");

	BackpropTrainer trainer(network);
	trainer.learning_rate = 0.001;

	vector<Node*>* nodes = new vector<Node*>();
	for (unsigned int i=0; i < network->out_size; i++)
	{
		nodes->push_back( network->nodes[network->out_offset+i] );
	}
	trainer.decorrelation_nodes = *nodes;

	trainer.decorrelation_enabled = true;

	trainer.train(set, 500);

	network->test_sequence(set,true);

	network->export_to_dot_graph("corr");
}

void test_activity()
{
	Network* net = new Network("activity.bnn");
	SequenceSet* input = new SequenceSet("ExperimentActivity/activity.dat");

	for (unsigned int i=0; i < input->size(); i++) {

		net->reserve( (*input)[i]->size() );
	}

	//ErrorFunction* err_func = new WinnerTakesAllErrorFunction();

//	net->test_sequence(input, true);

//	cout << "Error: " << net->evaluate_error(input, err_func) << endl;
//	cout << "--" <<endl;

	for (unsigned int i=0; i < input->size();i++)
	{
		input->get(i)->training_begin = 100;
	}

	TimeseriesClassification tc(net);
	tc.classify(input->get(1));
	tc.plot();
	tc.classify(input->get(20));
	tc.plot();
	tc.classify(input->get(21));
	tc.plot();
	tc.classify(input->get(19));
	tc.plot();
	tc.classify(input->get(16));
	tc.plot();

	ConfusionMatrix cm(net, input);
	cm.create();




	delete net;
	delete input;
}


void test_autoassoc()
{
	unsigned int hid[1] = {2};
	Network* net = NetworkFactory::createFeedForwardNetwork(4,
	Node::LINEAR_NODE, 1, hid, Node::LINEAR_NODE, 4);

	SequenceSet* input = new SequenceSet();
	try {
		input->load_from_file("datasets/autoassoc.dat");
	} catch (const int e) {
		std::cout << "File not Found!" << endl;
		exit(-1);
	}

	Trainer* trainer =
			new ImprovedRPropTrainer(net);
			//new BackpropTrainer(net);

	trainer->learning_rate = 0.01;
	trainer->momentum = 1;

	trainer->train(input, 5000);
	//vector<weight_t>* error = &trainer->error_train;

	net->test_sequence(input, true);

	net->save("assoc.bnn");
	net->export_to_dot_graph("assoc");

	delete net;
	delete input;
	//delete error;
}

void gers()
{
	Network* network = NetworkFactory::create_gers02a();
	SequenceSet* training_set = new SequenceSet("datasets/nmsd-train-F10-0-1.dat");
//	SequenceSet* training_set = new SequenceSet("datasets/nmsd-trivial.dat");

//	training_set->normalize();

	Trainer* trainer =
			//new ImprovedRPropTrainer(network);
			new BackpropTrainer(network);
	trainer->momentum = 0.9;
	trainer->learning_rate = 0.001;

	trainer->add_abort_criterion(
			new ZeroErrorCriterion( new ThresholdSquaredErrorFunction(0.5) )
		,200
	);

	trainer->train(training_set, 20000);

	network->test_sequence(training_set, true);

//	network->export_to_dot_graph();

	cout << "Epochs: " << trainer->total_iterations << endl;

	network->save("msd.bnn");
}



void biomag()
{
	Network* network = //new LSTMNetwork(2,20,2);
			NetworkFactory::createLSTMWTANetwork(Ensemble::LSTM_FORGET_ENSEMBLE, Node::TANH_NODE, 2,
					60, 2, -0.01, 0.01, -0.01, true);
	network->sort_nodes();



	SequenceSet* input = new SequenceSet("datasets/biomag-alltrials-69-200-2class.dat");

	input->normalize(true, false);

	DataSet* dataset = input->toDataSet(0.7,0.15);



	ImprovedRPropTrainer trainer(network);

	ErrorFunction* err_func = new WinnerTakesAllErrorFunction();
	//network->error_function = err_func;
	CallbackHandler* zero = new ZeroErrorFailCallback(err_func);

	ReportCallback* report = new ReportCallback();
	SaveNetworkCallback save("biomagtemp");
	trainer.add_callback(report, 1);
	trainer.add_callback(&save, 10);
	trainer.add_callback(zero,1);

	trainer.train(dataset->training_set, dataset->validation_set, 10000);
}

void animation()
{
	//LSTMNetwork *net = new LSTMNetwork(1,10,1);

	Network* net = new Network("activity.bnn");

	SequenceSet* input = new SequenceSet("ExperimentActivity/activity.dat");

//	unsigned int i=27;

	Sequence* sequence = input->get(4);
	sequence->append( input->get(1) );
	sequence->append( input->get(27) );

	net->activate( sequence );

	vector<Node*> outputs;
	for (unsigned int i=0; i < net->out_size; i++) {
		outputs.push_back( net->nodes[net->out_offset+i] );
	}
	vector<vector<weight_t>*>* data = net->collect_node_outputs(outputs);

	GnuplotGenerator::animate(data, &sequence->input, 50 );

	return;
/*
	Network* net = new Network("superimposedsin.bnn");

	SequenceSet* input = new SequenceSet("datasets/superimposedsin.dat");

	net->activate( input->get(0) );

	// find LSTM outputs
	vector<Node*> outputs;

	for (unsigned int i=0; i < net->ensembles.size();i++)
	{
		//LSTMForgetEnsemble* lstmensemble = net->lstms[i];
		LSTMForgetEnsemble* lstmensemble = dynamic_cast<LSTMForgetEnsemble*>(net->ensembles[i]);
		if (lstmensemble != 0)
		{
			//cout << "x";
			outputs.push_back( lstmensemble->get_outputgate() );

		}
	}
	if (outputs.size() == 0)
	{
		warning("There were no matching nodes found for generating the animation!");
		return;
	}

	vector<vector<weight_t>*>* data = net->collect_node_outputs(outputs);


	GnuplotGenerator::animate(data, &input->get(0)->input );
*/
	cout << "DONE" << endl;
}

unsigned int choose_node_type()
{
	unsigned int type;
	cout << "Choose node type" << endl;
	cout << "1... Tanh" << endl;
	cout << "2... Sigmoid" << endl;
	cout << "3... Linear" << endl;

	cin >> type;

	return type;
}

void test_set()
{
	SequenceSet set("datasets/testset.dat");

	set.print_sequences();

	//set.normalize();
	set.scale_to_minmax(-1,+1);

	//cout << "" << set.in
	//print_vector(set.input_means);
	//print_vector(set.input_stds);

	//set.denormalize();
	set.scale_to_original();

	cout << "========" << endl;

	// next test

	set.normalize_scale_and_clip(1);
	set.print_sequences();

	set.print_sequences();
}


/*
void sigfunc(int sig)
{
	cout << "Manual abort! Network could not be saved!"<<endl;
	exit(0);
}
*/
void train_sinee()
{
	SineEmbeddedTask task;
	task.init();
	task.train();
}


void test_sinee()
{
	SineEmbeddedTask task;
	task.restore();
	task.test();
}

void test_exp()
{
	Network* network = new Network("faces-embed.bnn");
	SequenceSet* sequence_set = new SequenceSet("ExperimentFaces/faces-nca.dat");

	cout << "create transfer" << endl;
	TransferFunction* transfer =
			new ExponentiallyEmbeddedTransferFunction(10);

	AutoPredictor predictor(network, transfer);
	cout << "pred" << endl;
	predictor.predict(sequence_set->get(0), 100, 1);

	delete network;
	delete transfer;
}

void tent()
{
	TentmapTask task;
	task.init();
	task.train();
}

void tent_test()
{
	TentmapTask task;
	task.restore();
	task.test();
}

void jacobian_check()
{
	Network* network = new RecurrentNetwork(1,4,1);
	//Network* network = NetworkFactory::createFeedForwardNetwork(1,4,1);
	SequenceSet* sequence_set = new SequenceSet("datasets/gradient.dat");
	Sequence* seq = sequence_set->get(0);
	Jacobian* j1 = network->jacobian_numerical( seq , 10e-5 );
	Jacobian* j2 = network->jacobian_backprop(seq);

	j1->print();
	j2->print();
}

int main (int argc, char * const argv[]) {
 
//	signal(SIGINT, sigfunc);

	GnuplotGenerator::clear_temp_directory();

	time_t seed = (unsigned)time(NULL);
 
 	std::cout << "seed: "<< seed << endl;
 
  	srand( seed );
 
 	if (argc == 1) {
 		
 		std::cout << "No arguments given. Starting default task." << endl;
 		
 	} else {
 	
 	 ///stringstream stream;
 	 //stream << *argv;
 	 //string args = stream.str();
 	 
     //vector<string> tokens;
     //string sep = std::string(" ");
     //split(args,sep,tokens); 
 	 if (strcmp(argv[1],"help") == 0) {
 		 std::cout << "Available modes:" << endl;
 		 std::cout << "train 		- " << endl;
 		 std::cout << "exportdot	- save network graph as image" << endl;
 		 cout << "create	-	run training from *.conf file" << endl;
 		 cout << "run	-	 run built in modes" << endl;
 		 exit(-1);
 	 }

     if (strcmp(argv[1],"train") == 0) {
     	
     	if (argc != 3) {
     		std::cout << "Please specify a training configuration file" << endl;
     		exit(-1);
     	}
     	
     	std::cout << "Load Configuration File:" << argv[2] << endl;
     	
     	TrainingConfiguration tfc = TrainingConfiguration( argv[2] );
     	
     	tfc.execute(true);
     	

     	exit(-1);	
     }  
     
     else if (strcmp(argv[1],"testtemp") == 0) {
     
     	if (argc != 3) {
     		std::cout << "Please specify a training configuration file" << endl;
     		exit(-1);
     	}
     	      
     	TrainingConfiguration tfc = TrainingConfiguration( argv[2] );
     	
     	tfc.execute(false);
     	

     	exit(-1);
     	
     } 

 	 else if (strcmp(argv[1],"continue") == 0) {

      	TrainingConfiguration tfc = TrainingConfiguration( argv[2] );


 		string dir = tfc.directory;
		int retval = chdir( dir.c_str() );
		if (retval != 0) {
			// TODO
			warning("chdir did not work");
		}
 		time_t best_time = 0;
 		string best_name = "";

 		DIR *dp;
 		struct dirent *dirp;
 		dp = opendir("./");
 		do
 		{

 			dirp = readdir(dp);
			if (dirp) {
	 			string d_name = dirp->d_name;

//				cout << dirp->d_name <<  " " << d_name.find(".")  << endl;

 				if (d_name.find(".") ==0) continue;

  				struct stat Status;

 				stat( dirp->d_name, &Status );

 				if (Status.st_mtime > best_time)
 				{
 					best_time = Status.st_mtime;
 					best_name = dirp->d_name;
 				}
 			}

 		} while(dirp);

 		cout << "Most recent file: " << best_name << endl;

 		retval = chdir("../");
 		if (retval != 0)
 		{
 			//TODO
 			warning("CHDIR");
 		}
     	tfc.execute(true);


 		exit(-1);

 	 } else if (strcmp(argv[1],"exportdot") == 0) {
 	 
      	TrainingConfiguration tfc = TrainingConfiguration( argv[2] );
 		tfc.save_as_image();	 
 	 	
 		exit(-1);

 	 } else if (strcmp(argv[1],"create") == 0) {

 		 cout << "choose network type" << endl
		<< "1... Feedforward network" << endl
		<< "2... Recurrent network" << endl
		<< "3... LSTM network" << endl
		<< "4... RGM network" << endl;

 		unsigned int choice;
 		cin >> choice;

 		unsigned int in,hid, out;
 		cout << "Choose input size" << endl;
 		cin >> in;
 		cout << "Choose hidden size" << endl;
 		cin >> hid;
 		cout << "Choose output size" << endl;
 		cin >> out;

 		unsigned int out_type = -1;

 		cout << "choose output node type"<<endl;
 		if (choice != 4)
 			out_type = choose_node_type();

 		cout << " Size: " << in << "," << hid<<"," << out << endl;

 		Network* net = NULL;
		unsigned int hids[] = {hid};

 		switch (choice)
		{
 		case 1:

 			net = NetworkFactory::createFeedForwardNetwork(in, Node::TANH_NODE,
 					1, hids, out_type,out);
 			//NetworkFactory::createFeedForwardNetwork()
 		break;
 		case 2:
 			net = NetworkFactory::createRecurrentNetwork(in, Node::TANH_NODE,
 					1, hids, out_type, out
 					);
 		break;
 		case 3:
			net = NetworkFactory::createTanhLSTMHiddenLayerNetwork( Ensemble::LSTM_FORGET_ENSEMBLE, out_type, in, hid, out);
 		break;
 		case 4:
 			double connectivity = 1.0;
 			unsigned int num_backwards = hid;
 			net = NetworkFactory::create_random_gate_memory_network(in, hid, out, connectivity, num_backwards);
 		break;
		}

 		net->sort_nodes();

 		net->save("network.bnn");

 		exit(-1);

 	 } else if (strcmp(argv[1],"run") == 0) {

 		 int mode = -1;
 		 if (argc >= 3) {
 		     mode = atoi(argv[2]);
 		 }
 		 cout << "Run built-it experiments #" << mode << endl;

 		 switch(mode)
 		 {
			 case 0:
				 gradient_check();
 			 break;
			 case 1:
				 //activation_check();
				 jacobian_check();
			 break;
			 case 2:
				 speed_check();
			 break;
			 case 4:
				// cout << "* Training Mackey-Glass series";
				 train_mackeyglass(false);
			 break;
			 case 5:
				// cout << "* Testing Mackey-Glass series";
				 test_mackeyglass();
			 break;
			 /*case 5:
				 copyrestore();
			break;*/
			 case 6:
				 train_msd();
				break;
			 case 7:
				 test_msd();
				break;
			 case 8:
				 train_activity();
			 break;
			 case 9:
				// train_clstm();
				 test_activity();
			 break;
			 default:
				 cout << "Please specify a valid training number!" << endl;
				 cout << "0,1... Gradient Check / Jacobian/Activation Check" <<endl;
				 cout << "2 ... Speed check" << endl;
				 cout << "40,41... PFG train,test" << endl;
				 cout << "4,5....Mackey Glass train"<<endl;
				 //cout << "5....Copy Restore"<<endl;
				 cout << "6,7...MSD train/test" << endl;
				 cout << "8,9....Train activity" <<endl;
				 //cout << "9....Train CLSTM" << endl;
				 cout << "12,13...Superimposed Sin train/test" << endl;
				 cout << "15...Train Parity" << endl;
				 cout << "16...Systematic Test" << endl;
				 cout << "18...Experimental" << endl;
				 cout << "20,21...Grammar train/test" << endl;
				 cout << "22,23... Switch train/test" << endl;
				 cout << "25,26... Faces train/test" << endl;
				 cout << "27... PSO" << endl;
				 cout << "28.... embed" << endl;
				 cout << "30,31... freq train/test" << endl;
				 cout << "33.... HGN" << endl;
				 cout << "36,37... Faces embed train/test" << endl;
				 cout << "42,43... Tent Map" << endl;
				 cout << "44... -" << endl;
				 cout << "50.... SFN test" << endl;
				 cout << "97,98.. soybean, mushroom" << endl;
			break;
			case 10:
				generate();
			break;
			case 11:
				jacobian_check();
			break;
			case 12:
				train_superimposedsins(false);
			break;
			case 13:
				test_superimposed();
			break;
			case 14:
				train_undef();
			break;
			case 15:
				train_parity(false);
			break;
			case 16:
				systematic_test();
			break;
			case 17:
				systematic_test_parity_noise();
			break;
			case 18:
				experimental();
			break;
			case 19:
				proben();
			break;
			case 20:
				grammar(false);
			break;
			case 21:
				grammar_test();
			break;
			case 22:
				switch_train(false);
			break;
			case 23:
				switch_test();
			break;
			case 24:
				train_prime();
			break;
			case 25:
				train_faces();
			break;
			case 26:
				test_faces();
			break;
			case 27:
				pso();
			break;
			case 28:
				embed_train();
			break;
			case 29:
				embed_test();
			break;
			case 30:
				freq_train();
			break;
			case 31:
				freq_test();
			break;
			case 32:
				test_autoassoc();
			break;
			case 33:
				train_hgn();
			break;
			case 34:
				test_hgn();
			break;
			case 35:
				simple_generator();
			break;
			case 36:
				train_faces_embed();
			break;
			case 37:
				test_faces_embed();
			break;
			case 38:
				train_sinee();
			break;
			case 39:
				test_sinee();
			break;
			 case 40:
				 pfg_train();
			 break;
			 case 41:
				 pfg_test();
			 break;
			 case 42:
				 tent();
			 break;
			 case 43:
				 tent_test();
			break;
			case 44:
				//doodle();
				//sfn();
				debug_exponentialembedding();
			break;
			case 50:
				sfn();
			break;
			case 51:
				sfn_test();
			break;
			case 55:
				arch();
			break;
			case 66:
				train_prime();
			break;
			case 88:
				//embed();
				test_set();
			break;
			case 90:
				pseudo_output_check(false);
			break;
			case 97:
				soybean();
			break;
			case 98:
				mushroom();
			break;
			case 99:
				//hmm();
				//animation();
				//debug_2ndorderlstm();
				//debug_2ndorder();
			break;
			case 100:
				debug();
			break;
			case 101:
				corr();
			break;
			case 102:
				corr2();
			break;
			case 1000:
				//test_plot();
				biomag();
			break;
			case 1001:
				//test_plot();
				biomag_test();
			break;
 		 }

 		 exit(-1);
 	 }

 	 /*if (argv[0]=="train") {
 	 
 	  
 	  	
 	 }	*/
 		
 	}
 
 	train_activity();
 	//animation();
 	//mushroom();
 	//debug();
 	//train_mackeyglass(false);
 	//test_exp();
 	//pseudo_output_check(false);
 	//test_faces();
 	//debug();
 	//arch();
 	//embed();
 	//pso();
 	//debug();
 	//test_msd();
 	//systematic_test();
 	//train_parity(false);
 	//embed_test();
 	//train_superimposedsins(false);
	//train_sinee();
 	//doodle();
 	//train_hgn();
 	//train_msd();
 	//test_activity();
 	//train_superimposedsins(false);
 	//test_superimposed();
 	//train_parity();
 	//gradient_check();
 	//train_clstm();
 	//train_simple_recurrent();
 	//generate();
 	//pfg_train();
 	//pfg_test();
 	//auto_test();
 	//train_mackeyglass();
 	//profile();
 	//test_mackeyglass();
 	//test1();
 	//test5();
 	//profile();
	//train_msd();
	//test_autoassoc();	
	//train_msd_counter();
	//train_faces();
	//test_activity();
	//train_activity();
 	//test_del();
	//test_rprop();
 	//test_sort();
 	//test4();
 	//test_cascade();
	return -1;		
		

}
