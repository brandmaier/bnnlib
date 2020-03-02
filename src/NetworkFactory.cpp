#include "NetworkFactory.h"
#include "nodes/BiasNode.h"
#include "nodes/TanhNode.h"
#include "Connection.h"
#include "nodes/SigmoidNode.h"
#include "ensembles/Ensemble.h"
#include "ensembles/LSTMEnsemble.h"
#include "ensembles/LSTMCounterEnsemble.h"
#include "nodes/LinearNode.h"
#include "nodes/PiNode.h"
#include "nodes/ProductNode.h"
#include "ensembles/LSTMForgetEnsemble.h"
#include "ensembles/KeepEnsemble.h"
#include "ensembles/LSTMPlusEnsemble.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "ensembles/WinnerTakesAllEnsemble.h"
#include "ensembles/RecurrentEnsemble.h"
#include "ensembles/LSTMCopyRestore.h"
#include "ensembles/LinearMemoryEnsemble.h"
//#include "LinearMemoryEnsemble.h"
#include "ensembles/SecondOrderEnsemble.h"
#include "ensembles/HarmonicGeneratorEnsemble.h"
#include "ensembles/SlowEnsemble.h"
#include "ensembles/SubtractiveEnsemble.h"
#include "ensembles/EnsembleTransformation.h"
#include "ensembles/GateEnsemble.h"
#include "ensembles/ConditionalBiasEnsemble.h"
/**
 * Simple Feed-forward Network with linear output and single tanh-hidden layer.
 * Bias to hidden and output layer
 * 
 * 
 * */
Network* NetworkFactory::createTestNetwork1(unsigned int in_size, unsigned int hid_size,unsigned int out_size, bool self_recurrence)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->bias_offset = 0;
	network->in_offset = 1;
	network->hid_offset = network->in_offset+in_size;
	network->out_offset = network->hid_offset+hid_size;
	
	network->bias_size = 1;

	network->nodes.reserve(network->size);
	
	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );
	
	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node = new TanhNode();
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	if (self_recurrence)
	for (unsigned int i=0; i < hid_size; i++) {
		network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+i], false);				
	}
	/*
	// connect all hidden 
		for (int i=0; i < hid_size; i++) {
				for (int j=0; j < hid_size; j++) {
					connect(nodes[hid_offset+i],nodes[hid_offset+j], i<j);
				}
		}
	*/
	// connect bias to hidden
	for (unsigned int i=0; i < network->hid_size; i++) {	
		network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	for (unsigned int i=0; i < network->out_size; i++) {	
		network->connect(biasnode, network->nodes[network->out_offset+i], true);
	}		
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
}

/**
 * FF "layer" with stacked connections (no recurrence or self-recurrence)
 * output layer is linear, hidden layer is tanh
 * and Bias connections to hidden and output layer
 **/
Network* NetworkFactory::createTestNetwork2(unsigned int in_size, unsigned int hid_size,unsigned int out_size)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );

	network->bias_size = 1;

	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	} 
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node = new TanhNode();
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	// connect bias to hidden
	for (unsigned int i=0; i < network->hid_size; i++) {	
		network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	for (unsigned int i=0; i < network->out_size; i++) {	
		network->connect(biasnode, network->nodes[network->out_offset+i], true);
	}	
	
	// connect all hidden if i < j with backward connections
		for (unsigned int i=0; i < network->hid_size; i++) {
				for (unsigned int j=0; j < network->hid_size; j++) {
					if (i < j)
					 network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+j], true);
				}
		}
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
}

/**
 * 
 *	create a network corresponding to
 *  a single hidden layer that is fully connected with recurrent (also self-recurrent) connections
 *  also including a Bias unit
 *  
 **/
Network* NetworkFactory::createTestNetwork3(unsigned int in_size, unsigned int hid_size,unsigned int out_size)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );

	network->bias_size = 1;

	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	} 
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node = new TanhNode();
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	// connect bias to hidden
	for (unsigned int i=0; i < network->hid_size; i++) {	
		network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	for (unsigned int i=0; i < network->out_size; i++) {	
		network->connect(biasnode, network->nodes[network->out_offset+i], true);
	}	
	
	// connect all hidden with backward connections
		for (unsigned int i=0; i < network->hid_size; i++) {
				for (unsigned int j=0; j < network->hid_size; j++) {
					 network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+j], false);
				}
		}
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
}

/**
 * Recurrent Neural Network
 * including a hidden ensemble of PiNodes and tanh-Nodes
 * bias weights to hidden and output units
 **/
Network* NetworkFactory::createTestNetwork4(unsigned int in_size, unsigned int hid_size,unsigned int out_size, bool bias)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );

	network->bias_size = 1;

	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	} 
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node;
		if (i%4==0)
		 { node = new PiNode(); }
		else
		 { node = new TanhNode(); }
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	// connect bias to hidden but not PiNodes
	if (bias)
	for (unsigned int i=0; i < network->hid_size; i++) {
		if (i%4!=0)	
			network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	for (unsigned int i=0; i < network->out_size; i++) {	
		//network->connect(biasnode, network->nodes[network->out_offset+i], true);
	}	
	
	// connect all hidden with backward connections
		for (unsigned int i=0; i < network->hid_size; i++) {
				for (unsigned int j=0; j < network->hid_size; j++) {
				//	if (i%4!=0)
				//	 network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+j], false);
				}
		}
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
}

/**
 *  Network with only linear units
 *  and self-recurrence
 * 
 **/
Network* NetworkFactory::createTestNetwork5(unsigned int in_size, unsigned int hid_size,unsigned int out_size)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );

	network->bias_size = 1;

	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	} 
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	// connect bias to hidden
	for (unsigned int i=0; i < network->hid_size; i++) {	
		network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	//for (unsigned int i=0; i < network->out_size; i++) {	
	//	network->connect(biasnode, network->nodes[network->out_offset+i], true);
	//}	
	
	// connect all hidden if i < j with backward connections
		for (unsigned int i=0; i < network->hid_size; i++) {
				//for (unsigned int j=0; j < network->hid_size; j++) {
					//if (i < j)
					 network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+i], false);
				//}
		}
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
}

/**
 * LSTM Network structure
 * 
 * Create an input ensemble, a single LSTM ensemble and an output ensemble
 * no bias
 * 
 **/
/*Network* NetworkFactory::createTestNetwork6(unsigned int in_size, unsigned int hid_size,unsigned int out_size)
{
	Network* network = new Network();

	//TODO BUG DIRTY
	hid_size = 7;

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

//	Node* biasnode = new BiasNode();
//	network->nodes.push_back( biasnode );

	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* lstm = new LSTMEnsemble();
	Ensemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( lstm );
	network->add_ensemble( out );
	
	network->connect_ensembles(in, lstm, true);
	network->connect_ensembles(lstm, out, true);
	network->connect_ensembles(bias, lstm, true);	
	
	
//	std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}
*/
/**
 * LSTM Network Structure with multiple cells
 * 
 * 
 **/
Network* NetworkFactory::createTestNetwork7(unsigned int in_size, unsigned int hid_size1, unsigned int hid_size2,unsigned int out_size)
{
	Network* network = new Network();

	hid_size2 = 1;

	//TODO BUG DIRTY
	unsigned int lstm_cells = hid_size2;
	unsigned int hid_size = 7 * lstm_cells + hid_size1;

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);



	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* hidden = new FeedforwardEnsemble(Node::TANH_NODE,hid_size1);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++)
		lstms[i] = new LSTMEnsemble();	
	Ensemble* out = new FeedforwardEnsemble(Node::TANH_NODE,out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( hidden );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( out );

	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);
	network->connect_ensembles(hidden, hidden, false);
	
	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], hidden, false);
		network->connect_ensembles(hidden, lstms[i], false);

		network->connect_ensembles(bias, lstms[i], true);
	//	for (unsigned int j=0; j < lstm_cells; j++) {	
	//		network->connect_ensembles(lstms[i], lstms[j], false );
	//	}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

/**
 * like Network4 only with ProductNodes
 * 
 * Recurrent Neural Network
 * including a hidden ensemble of ProductNodes and tanh-Nodes
 * bias weights to hidden and output units
 **/
Network* NetworkFactory::createTestNetwork8(unsigned int in_size, unsigned int hid_size,unsigned int out_size)
{
	Network* network = new Network();

	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	network->nodes.reserve(network->size);

	Node* biasnode = new BiasNode();
	network->nodes.push_back( biasnode );

	network->bias_size = 1;

	for (unsigned int i=0; i < in_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	} 
	for (unsigned int i=0; i < hid_size; i++) {
		Node* node;
		if (i%4==0)
		 { node = new ProductNode(); }
		else
		 { node = new SigmoidNode(); }
		network->nodes.push_back( node );
	}
	for (unsigned int i=0; i < out_size; i++) {
		Node* node = new LinearNode();
		network->nodes.push_back( node );
	}
	
	// connect bias to hidden but not PiNodes
//	if (bias)
	for (unsigned int i=0; i < network->hid_size; i++) {
		if (i%4!=0)	
			network->connect(biasnode, network->nodes[network->hid_offset+i], true);
	}
	// connect bias to out
	for (unsigned int i=0; i < network->out_size; i++) {	
		//network->connect(biasnode, network->nodes[network->out_offset+i], true);
	}	
	
	// connect all hidden with backward connections
		for (unsigned int i=0; i < network->hid_size; i++) {
				for (unsigned int j=0; j < network->hid_size; j++) {
				//	if (i%4!=0)
				//	 network->connect(network->nodes[network->hid_offset+i],network->nodes[network->hid_offset+j], false);
				}
		}
	
	 // connect hid to out
	for (unsigned int i=0; i < network->hid_size; i++) {
		for (unsigned int j=0; j < network->out_size; j++) {
			network->connect(network->nodes[network->hid_offset+i],network->nodes[network->out_offset+j], true);
		}
	}	
	// connect in to hid
	for (unsigned int i=0; i < network->in_size; i++) {
		for (unsigned int j=0; j < network->hid_size; j++) {
			network->connect(network->nodes[network->in_offset+i],network->nodes[network->hid_offset+j], true);
		}
	}		
	

	return network;
} 


 

Network* NetworkFactory::createTestNetwork9(unsigned int in_size,	unsigned int lstm_cells , unsigned int hid_size, unsigned int out_size, bool peepholes)
{
	Network* network = new Network();
	

	unsigned int tan_size = hid_size;
	hid_size = 10*lstm_cells + hid_size;
	
	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;
	
	network->nodes.reserve(network->size);
	
	
	
	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* hidden = new FeedforwardEnsemble(Node::TANH_NODE,tan_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++)
		lstms[i] = new LSTMForgetEnsemble( peepholes, Node::TANH_NODE );	
		//lstms[i] = new LSTMEnsemble(true);
	Ensemble* out = new FeedforwardEnsemble(Node::TANH_NODE,out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( hidden );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( out );
	
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);
	network->connect_ensembles(bias, hidden, true);
	network->connect_ensembles(in, out, true);
	network->connect_ensembles(hidden, hidden, false);
	
	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], hidden, false);
		network->connect_ensembles(lstms[i], out, true);
		network->connect_ensembles(hidden, lstms[i], false);
		//network->connect_ensembles(hidden, hidden, false);
		//network->connect_ensembles(bias, lstms[i], true, -(0.5*i));
			for (unsigned int j=0; j < lstm_cells; j++) {	
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

Network* NetworkFactory::createTestNetwork11(unsigned int in_size,	unsigned int lstm_cells , unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();
	

	unsigned int tan_size = hid_size;
	hid_size = 9*lstm_cells + hid_size;
	
	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;
	
	network->nodes.reserve(network->size);
	
	
	
	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* hidden = new FeedforwardEnsemble(Node::TANH_NODE,tan_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++)
		lstms[i] = new LSTMForgetEnsemble(true, Node::TANH_NODE);	
		//lstms[i] = new LSTMEnsemble(true);
	Ensemble* hidden2 = new FeedforwardEnsemble(Node::TANH_NODE,out_size);
	Ensemble* out = new FeedforwardEnsemble(Node::TANH_NODE,out_size);
		
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( hidden );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( hidden2 );
	network->add_ensemble( out );
	
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, hidden2, true);
	network->connect_ensembles(bias, hidden, true);
	network->connect_ensembles(in, out, true);
	network->connect_ensembles(hidden, hidden, false);
	network->connect_ensembles(hidden2, out, true);
	
	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], hidden, false);
		network->connect_ensembles(hidden, lstms[i], false);
		//network->connect_ensembles(hidden, hidden, false);
		network->connect_ensembles(bias, lstms[i], true, -(0.5*i));
			for (unsigned int j=0; j < lstm_cells; j++) {	
				network->connect_ensembles(lstms[i], lstms[j], false );
			}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

Network* NetworkFactory::createTestNetwork10(unsigned int in_size,	unsigned int keep_ensemble, unsigned int keep_ensemble_size , unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();
	

	unsigned int tan_size = hid_size;
	hid_size = keep_ensemble*(7+keep_ensemble_size) + hid_size;
	
	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;
	
	network->nodes.reserve(network->size);
	
	
	
	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* hidden = new FeedforwardEnsemble(Node::TANH_NODE,tan_size);
	vector<Ensemble*> keeps( keep_ensemble );
	for (unsigned int i=0; i < keep_ensemble; i++)
		keeps[i] = new KeepEnsemble(keep_ensemble_size);	
	Ensemble* out = new FeedforwardEnsemble(Node::TANH_NODE,out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( hidden );
	for (unsigned int i=0; i < keep_ensemble; i++)
		network->add_ensemble( keeps[i] );
	
	network->add_ensemble( out );
	
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);
	network->connect_ensembles(bias, hidden, true);
	network->connect_ensembles(in, out, true);
	network->connect_ensembles(hidden, hidden, false);
	
	for (unsigned int i=0; i < keep_ensemble; i++) {	
		network->connect_ensembles(in, keeps[i], true);
		network->connect_ensembles(keeps[i], hidden, false);
		network->connect_ensembles(hidden, keeps[i], false);
		//network->connect_ensembles(hidden, hidden, false);
		network->connect_ensembles(bias, keeps[i], true, -(0.5*i));
			for (unsigned int j=0; j < keep_ensemble; j++) {	
				network->connect_ensembles(keeps[i], keeps[j], false );
			}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}


/**
 * a variation of Network 9 
 * with LSTMPlusEnsembles instead of LSTMEnsembles
 **/
Network* NetworkFactory::createTestNetwork13(unsigned int in_size,	unsigned int lstm_cells , unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();
	

	unsigned int tan_size = hid_size;
	hid_size = 9*lstm_cells + hid_size;
	
	network->in_size = in_size;
	network->out_size = out_size;
	network->hid_size = hid_size;
	
	network->size = in_size+hid_size+out_size+1;
	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;
	
	network->nodes.reserve(network->size);
	
	
	
	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1); 
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	Ensemble* hidden = new FeedforwardEnsemble(Node::SIGMOID_NODE, tan_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++)
		lstms[i] = new LSTMPlusEnsemble( false, Node::SIGMOID_NODE );	
		//lstms[i] = new LSTMEnsemble(true);
	Ensemble* out = new FeedforwardEnsemble(Node::SIGMOID_NODE, out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	network->add_ensemble( hidden );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( out );
	
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(hidden, out, true);
	network->connect_ensembles(bias, hidden, true);
	network->connect_ensembles(in, out, true);
	network->connect_ensembles(hidden, hidden, false);
	
	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], hidden, false);
		network->connect_ensembles(hidden, lstms[i], false);
		//network->connect_ensembles(hidden, hidden, false);
		network->connect_ensembles(bias, lstms[i], true, -(0.5*i));
			for (unsigned int j=0; j < lstm_cells; j++) {	
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}


Network* NetworkFactory::createLinLSTMHiddenLayerNetwork(unsigned int lstm_type,
	unsigned int out_type, unsigned int in_size, unsigned int lstm_cells, unsigned int out_size)

{
	return  createLSTMHiddenLayerNetwork(lstm_type,
		Node::LINEAR_NODE, out_type, in_size, lstm_cells, out_size,0,0,0, true, false );

}

Network* NetworkFactory::createTanhLSTMHiddenLayerNetwork(unsigned int lstm_type,
	unsigned int out_type, unsigned int in_size, unsigned int lstm_cells, unsigned int out_size)

{
	return  createLSTMHiddenLayerNetwork(lstm_type,
		Node::TANH_NODE, out_type, in_size, lstm_cells, out_size,0,0,0, true, false );

}

Network* NetworkFactory::createCopyRestoreLSTM(unsigned int in_size, unsigned int cells, unsigned int out_size)
{
	return createLSTMHiddenLayerNetwork( Ensemble::LSTM_COPYRESTORE_ENSEMBLE, Node::TANH_NODE, Node::TANH_NODE,
			in_size, cells, out_size, 0,0,0,true, false);
}

/**
 * a variation of Network 9 
 * with LSTMPlusEnsembles instead of LSTMEnsembles
 **/
Network* NetworkFactory::createLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int lstm_act_type,
	unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size,
	weight_t input_bias_discount, weight_t forget_bias_discount, weight_t output_bias_discount,
	bool peepholes, bool bias_to_lstm_input)
{
	Network* network = new Network();

	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE,in_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++) {
		switch(lstm_type) {
			case Ensemble::LSTM_FORGET_ENSEMBLE:
			 lstms[i] = new LSTMForgetEnsemble( peepholes, lstm_act_type,
					 (i+1)*input_bias_discount, (i+1)*forget_bias_discount,
					 (i+1)*output_bias_discount );
			break;
			case Ensemble::LSTM_ENSEMBLE:
			 lstms[i] = new LSTMEnsemble();
			break;
			case Ensemble::LSTM_PLUS_ENSEMBLE:
			 lstms[i] = new LSTMPlusEnsemble( peepholes, lstm_act_type,
					 i*input_bias_discount, i*forget_bias_discount, i*output_bias_discount  );
			break;
			case Ensemble::LSTM_COUNTER_ENSEMBLE:
			 lstms[i] = new LSTMCounterEnsemble();
			break;
			case Ensemble::LSTM_COPYRESTORE_ENSEMBLE:
				lstms[i] = new LSTMCopyRestore(peepholes, lstm_act_type );
			break;
			default:
			error("Invalid LSTM Layer type given in NetworkFactory!");
		}
		
		network->hid_size += lstms[i]->nodes.size();
	}	
/*	network->in_size = in_size;
	network->out_size = out_size;
	network->size = in_size+network->hid_size+out_size+1;

	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;	
	*/
	
	Ensemble* out = new FeedforwardEnsemble(out_type, out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( out );
	


	network->connect_ensembles(bias, out, true);
	//network->connect_ensembles(in, out, true);


	for (unsigned int i=0; i < lstm_cells; i++) {	

		if (bias_to_lstm_input)
		 network->connect( (bias->nodes[0]), ((LSTMForgetEnsemble*)lstms[i])->get_input() , true);

		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], out, true);
			for (unsigned int j=0; j < lstm_cells; j++) {	
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}	
	}
	
	network->sort_nodes(in, out);
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

Network* NetworkFactory::createLSTMWTANetwork(unsigned int lstm_type, unsigned int lstm_act_type,
	 unsigned int in_size,	unsigned int lstm_cells, unsigned int out_size,
	 weight_t in_disc, weight_t forg_disc, weight_t out_disc, bool out_bias)
{
	Network* network = new Network();
	


	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE,in_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++) {
		switch(lstm_type) {
			case Ensemble::LSTM_FORGET_ENSEMBLE:
			 lstms[i] = new LSTMForgetEnsemble( true, lstm_act_type, i*in_disc, i*forg_disc, i*out_disc );
			break;
			case Ensemble::LSTM_ENSEMBLE:
			 lstms[i] = new LSTMEnsemble();
			break;
			case Ensemble::LSTM_PLUS_ENSEMBLE:
			 lstms[i] = new LSTMPlusEnsemble( true, lstm_act_type,i*in_disc, i*forg_disc, i*out_disc );
			break;
			case Ensemble::LSTM_COUNTER_ENSEMBLE:
			 lstms[i] = new LSTMCounterEnsemble();
			break;
			case Ensemble::LSTM_COPYRESTORE_ENSEMBLE:
				lstms[i] = new LSTMCopyRestore(true, lstm_act_type, in_disc, forg_disc, out_disc );
			break;
			default:
			error("Invalid LSTM Layer type given in NetworkFactory!");
		}
		
		network->hid_size += lstms[i]->nodes.size();
	}	
	network->in_size = in_size;
	network->out_size = out_size;
	network->size = in_size+network->hid_size+out_size+1;

	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;	
	
	Ensemble* out = new WinnerTakesAllEnsemble(Node::LINEAR_NODE, out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( out );
	

	if (out_bias)
	network->connect_ensembles(bias, out, true);
	//network->connect_ensembles(in, out, true);


	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], out, true);
			for (unsigned int j=0; j < lstm_cells; j++) {	
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}	
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

Network* NetworkFactory::createExtendedLSTMHiddenLayerNetwork(unsigned int lstm_type, unsigned int lst_act_type,
	unsigned int out_type, unsigned int in_size,	unsigned int lstm_cells, unsigned int hidden_size, unsigned int out_size)
{
	Network* network = new Network();
	


	network->bias_size = 1;
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE,in_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++) {
		switch(lstm_type) {
			case Ensemble::LSTM_FORGET_ENSEMBLE:
			 lstms[i] = new LSTMForgetEnsemble( true, lst_act_type );
			break;
			case Ensemble::LSTM_ENSEMBLE:
			 lstms[i] = new LSTMEnsemble();
			break;
			case Ensemble::LSTM_PLUS_ENSEMBLE:
			 lstms[i] = new LSTMPlusEnsemble( true, lst_act_type ,0,0,0);
			break;
			case Ensemble::LSTM_COUNTER_ENSEMBLE:
			 lstms[i] = new LSTMCounterEnsemble();
			break;
			error("Invalid LSTM Layer type given in NetworkFactory!");
		}
		
		network->hid_size += lstms[i]->nodes.size();
	}	
	
	network->hid_size += hidden_size;
	
	network->in_size = in_size;
	network->out_size = out_size;
	network->size = in_size+network->hid_size+out_size+1;

	
	network->in_offset = 1;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;	

	Ensemble* hidden = new FeedforwardEnsemble(Node::TANH_NODE, hidden_size);	
	Ensemble* out = new FeedforwardEnsemble(out_type, out_size);
	
	network->add_ensemble( bias );
	network->add_ensemble( in );
	for (unsigned int i=0; i < lstm_cells; i++)
		network->add_ensemble( lstms[i] );
	
	network->add_ensemble( hidden );
	network->add_ensemble( out );
	
	network->connect_ensembles(in, hidden, true);
	network->connect_ensembles(bias, hidden, true);
	
	network->connect_ensembles(hidden, out, true);


	for (unsigned int i=0; i < lstm_cells; i++) {	
		network->connect_ensembles(in, lstms[i], true);

			for (unsigned int j=0; j < lstm_cells; j++) {	
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}	
		network->connect_ensembles(lstms[i], hidden, true);
	}
	
	
	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;
	
	return network;
}

Network* NetworkFactory::createFeedForwardNetwork(unsigned int in_size,unsigned int hid_size, unsigned int out_size)
{
	unsigned int sizes[] = { hid_size };
	return createFeedForwardNetwork(in_size, Node::TANH_NODE,1, sizes, Node::TANH_NODE, out_size);
}

Network* NetworkFactory::createFeedForwardNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size, int out_type)
{
  unsigned int sizes[] = { hid_size };
  return createFeedForwardNetwork(in_size, Node::TANH_NODE,1, sizes, out_type, out_size);
}



Network* NetworkFactory::createSparseAutoencoder(unsigned int in_size, unsigned int hidden_size, double sparsity) {
  Network* net = new Network();
  
  unsigned int num_layers = 1;
  int hid_type = Node::TANH_NODE;
  int out_type = Node::LINEAR_NODE;
  
  
  // create Ensembles
  
  Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);	
  net->add_ensemble(bias);
  
  Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);		
  net->add_ensemble(in);
  //unsigned int total_hid_size = 0;

  Ensemble* hidden = new FeedforwardEnsemble(hid_type, hidden_size);	
    net->add_ensemble(hidden);
    //total_hid_size += layer_sizes[i];
    
    hidden->set_sparsity(sparsity);

  Ensemble* out = new FeedforwardEnsemble(out_type, in_size);
  net->add_ensemble(out);
  
  
  // connect ensembles
  net->connect_ensembles(bias, hidden, true);
  net->connect_ensembles(in, hidden, true);
  net->connect_ensembles(hidden, out, true);
  net->connect_ensembles(bias, out, true);

  
  net->sort_nodes();
  
  return net;  
  
}


Network* NetworkFactory::createFeedForwardNetwork(unsigned int in_size, int hid_type,unsigned int num_layers, unsigned int layer_sizes[], int out_type, unsigned int out_size)
{
	Network* net = new Network();
	
	// create Ensembles
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);	
	net->add_ensemble(bias);
	
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);		
	net->add_ensemble(in);
	Ensemble* hidden[num_layers];
	unsigned int total_hid_size = 0;
	for (unsigned int i=0; i < num_layers; i++)
	{
		hidden[i] = new FeedforwardEnsemble(hid_type, layer_sizes[i]);	
		net->add_ensemble(hidden[i]);
		total_hid_size += layer_sizes[i];
	} 
	Ensemble* out = new FeedforwardEnsemble(out_type, out_size);
	net->add_ensemble(out);
	
	
	// connect ensembles
	net->connect_ensembles(bias, hidden[0], true);
	net->connect_ensembles(in, hidden[0], true);
	for (unsigned int i=0; i < num_layers-1; i++)
	{
		net->connect_ensembles(hidden[i], hidden[i+1], true);
		net->connect_ensembles(bias, hidden[i+1], true);
	}	
	net->connect_ensembles(hidden[num_layers-1], out, true);
	net->connect_ensembles(bias, out, true);
	
	/*net->bias_size = 1;
	net->bias_offset = 0;
	net->in_offset = 1;
	net->in_size = in_size;
	net->hid_offset = net->in_offset+net->in_size;
	net->hid_size = total_hid_size;
	net->out_offset = net->hid_offset+net->hid_size;
	net->out_size = out_size;
	net->size = 1+net->in_size+net->hid_size+net->out_size;
	*/

	net->sort_nodes();
	
	return net;
}

Network* NetworkFactory::createRecurrentNetwork(unsigned int in_size, unsigned int hid_size, unsigned int out_size)
{
	unsigned int sizes[] = { hid_size };
	return createRecurrentNetwork(in_size, Node::TANH_NODE,1, sizes, Node::TANH_NODE, out_size);
}


Network* NetworkFactory::createRecurrentNetwork(unsigned int in_size, int hid_type, unsigned int num_layers, 
			unsigned int layer_sizes[], int out_type, unsigned int out_size)
{
	Network* net = new Network();
	
	// create Ensembles
	
	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);	
	net->add_ensemble(bias);
	
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);		
	net->add_ensemble(in);
	Ensemble* hidden[num_layers];
	unsigned int total_hid_size = 0;
	for (unsigned int i=0; i < num_layers; i++)
	{
		hidden[i] = new RecurrentEnsemble(hid_type, layer_sizes[i]);	
		net->add_ensemble(hidden[i]);
		total_hid_size += layer_sizes[i];
	} 
	Ensemble* out = new FeedforwardEnsemble(out_type, out_size);
	net->add_ensemble(out);
	
	
	// connect ensembles

	net->connect_ensembles(in, hidden[0], true);
	for (unsigned int i=0; i < num_layers-1; i++)
	{
		net->connect_ensembles(hidden[i], hidden[i+1], true);
		net->connect_ensembles(bias, hidden[i+1], true);
	}	
	net->connect_ensembles(hidden[num_layers-1], out, true);

	net->connect_ensembles(bias, hidden[0], true);
	net->connect_ensembles(bias, out, true);
	
/*	net->bias_size = 1;
	net->bias_offset = 0;
	net->in_offset = 1;
	net->in_size = in_size;
	net->hid_offset = net->in_offset+net->in_size;
	net->hid_size = total_hid_size;
	net->out_offset = net->hid_offset+net->hid_size;
	net->out_size = out_size;
	net->size = 1+net->in_size+net->hid_size+net->out_size;
	*/
	net->sort_nodes(in, out);
	
	return net;
}

Network* NetworkFactory::createRecurrentWTANetwork(unsigned int in_size, int hid_type, unsigned int num_layers, unsigned int layer_sizes[],  unsigned int out_size)
{
	Network* net = new Network();

	// create Ensembles

	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	net->add_ensemble(bias);

	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	net->add_ensemble(in);
	Ensemble* hidden[num_layers];
	unsigned int total_hid_size = 0;
	for (unsigned int i=0; i < num_layers; i++)
	{
		hidden[i] = new RecurrentEnsemble(hid_type, layer_sizes[i]);
		net->add_ensemble(hidden[i]);
		total_hid_size += layer_sizes[i];
	}
	Ensemble* out = new WinnerTakesAllEnsemble(Node::LINEAR_NODE, out_size);
	net->add_ensemble(out);


	// connect ensembles
	net->connect_ensembles(bias, hidden[0], true);
	net->connect_ensembles(in, hidden[0], true);
	for (unsigned int i=0; i < num_layers-1; i++)
	{
		net->connect_ensembles(hidden[i], hidden[i+1], true);
		net->connect_ensembles(bias, hidden[i], true);
	}
	net->connect_ensembles(hidden[num_layers-1], out, true);
	net->connect_ensembles(bias, out, true);

	net->bias_size = 1;
	net->bias_offset = 0;
	net->in_offset = 1;
	net->in_size = in_size;
	net->hid_offset = net->in_offset+net->in_size;
	net->hid_size = total_hid_size;
	net->out_offset = net->hid_offset+net->hid_size;
	net->out_size = out_size;
	net->size = 1+net->in_size+net->hid_size+net->out_size;

	return net;
}

Network* NetworkFactory::createFeedForwardWTANetwork(unsigned int in_size,unsigned int num_layers, unsigned int layer_sizes[], unsigned int out_size)
{
	Network* net = new Network();
	
	// create Ensembles
	

	Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);	
	net->add_ensemble(bias);
	
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);		
	net->add_ensemble(in);
	Ensemble* hidden[num_layers];
	unsigned int total_hid_size = 0;
	for (unsigned int i=0; i < num_layers; i++)
	{
		hidden[i] = new FeedforwardEnsemble(Node::TANH_NODE, layer_sizes[i]);	
		net->add_ensemble(hidden[i]);
		total_hid_size += layer_sizes[i];
	} 
	Ensemble* out = new WinnerTakesAllEnsemble(Node::LINEAR_NODE, out_size);
	net->add_ensemble(out);
	
	
	// connect ensembles
	net->connect_ensembles(bias, hidden[0], true);
	net->connect_ensembles(in, hidden[0], true);
	for (unsigned int i=0; i < num_layers-1; i++)
	{
		net->connect_ensembles(hidden[i], hidden[i+1], true);
		net->connect_ensembles(bias, hidden[i], true);		
	}	
	net->connect_ensembles(hidden[num_layers-1], out, true);
	net->connect_ensembles(bias, out, true);
	
	net->bias_size = 1;
	net->bias_offset = 0;
	net->in_offset = 1;
	net->in_size = in_size;
	net->hid_offset = net->in_offset+net->in_size;
	net->hid_size = total_hid_size;
	net->out_offset = net->hid_offset+net->hid_size;
	net->out_size = out_size;
	net->size = 1+net->in_size+net->hid_size+net->out_size;
	
	return net;
}

Network* NetworkFactory::createCLSTM(unsigned int in_size, unsigned int lstm_cells, unsigned int out_size)
{
	Network* network = new Network();

	unsigned int lstm_act_type = Node::TANH_NODE;
	unsigned int lstm_type = Ensemble::LSTM_FORGET_ENSEMBLE;
	bool peepholes = true;
	weight_t input_bias_discount=0;
	weight_t forget_bias_discount=0;
	weight_t output_bias_discount = 0;
	unsigned int out_type = Node::TANH_NODE;

	network->bias_size = 2;

	Ensemble* bias = new ConditionalBiasEnsemble();
		//	new FeedforwardEnsemble(Node::BIAS_NODE, 2);
	Ensemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE,in_size);
	vector<Ensemble*> lstms( lstm_cells );
	for (unsigned int i=0; i < lstm_cells; i++) {
		switch(lstm_type) {
			case Ensemble::LSTM_FORGET_ENSEMBLE:
			 lstms[i] = new LSTMForgetEnsemble( peepholes, lstm_act_type,
					 i*input_bias_discount, i*forget_bias_discount, i*output_bias_discount );
			break;
			case Ensemble::LSTM_ENSEMBLE:
			 lstms[i] = new LSTMEnsemble();
			break;
			case Ensemble::LSTM_PLUS_ENSEMBLE:
			 lstms[i] = new LSTMPlusEnsemble( peepholes, lstm_act_type,
					 i*input_bias_discount, i*forget_bias_discount, i*output_bias_discount);
			break;
			case Ensemble::LSTM_COUNTER_ENSEMBLE:
			 lstms[i] = new LSTMCounterEnsemble();
			break;
			case Ensemble::LSTM_COPYRESTORE_ENSEMBLE:
				lstms[i] = new LSTMCopyRestore(peepholes, lstm_act_type );
			break;
			default:
			error("Invalid LSTM Layer type given in NetworkFactory!");
		}

		network->hid_size += lstms[i]->nodes.size();
	}
	network->in_size = in_size;
	network->out_size = out_size;
	network->size = in_size+network->hid_size+out_size+network->bias_size;


	network->in_offset = network->bias_size;
	network->hid_offset = network->in_offset+network->in_size;
	network->out_offset = network->hid_offset+network->hid_size;
	network->bias_offset = 0;

	Ensemble* out = new FeedforwardEnsemble(out_type, out_size);

	network->add_ensemble( bias );
	network->add_ensemble( in );
	for (unsigned int i=0; i < lstm_cells; i++)
	{
		network->add_ensemble( lstms[i] );
	}

	network->add_ensemble( out );



	//network->connect_ensembles(bias, out, true);
	//network->connect_ensembles(in, out, true);


	for (unsigned int i=0; i < lstm_cells; i++) {
		network->connect_ensembles(in, lstms[i], true);
		network->connect_ensembles(lstms[i], out, true);
		network->connect_ensembles(bias, lstms[i], true);
			for (unsigned int j=0; j < lstm_cells; j++) {
				//if (i != j)
				 network->connect_ensembles(lstms[i], lstms[j], false );
			}
	}


	//std::cout<< " Created Network with " << network->get_num_weights() << " connections and " << network->nodes.size() << " nodes " << endl;

	return network;
}

Network* NetworkFactory::create_random_gate_network(unsigned int in_size, unsigned int hid_size,
		unsigned int out_size, double connectivity, unsigned int num_backward_connections)
{
	Network* network = new Network();

	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

	network->add_ensemble(in);
	network->add_ensemble(bias);


	for (unsigned int k=0; k < hid_size; k++)
	{

		GateEnsemble* ge = new GateEnsemble();


		network->connect_ensembles(in, ge, true);
		network->connect_ensembles(bias, ge, true);

		// connect other Ensembles to new Ensemble with connectivity factor
		for (unsigned int i=2; i < network->ensembles.size(); i++)
		{
			if (rand_uniform_01() <= connectivity) {
				network->connect_ensembles( network->ensembles[i], ge, true);
			}
		}

		network->add_ensemble(ge);
		network->connect_ensembles(ge, out, true);

	}
	network->add_ensemble(out);

	network->size = network->nodes.size();

//	network->sanity_check(true);
	cout << "nodes: " << network->nodes.size() << endl;
	network->sort_nodes();

	// choose random hidden nodes and establish backward connections
	set<string> connections;
	for (int i=0; i < num_backward_connections; i++) {
		unsigned int idx1 = network->hid_offset + rand(0, network->hid_size-1);
		unsigned int idx2 = network->hid_offset + rand(0, network->hid_size-1);
		stringstream s;
		s << idx1 << "->" << idx2;
		if (connections.find(s.str()) == connections.end()) {
		 connections.insert(s.str());
		 network->connect( network->nodes[idx1], network->nodes[idx2], false);
		} else {
			i--; // search again
		}

	}

	cout << "out: " << network->out_size << endl;

	return network;
}

Network* NetworkFactory::create_random_gate_memory_network(unsigned int in_size, unsigned int hid_size,
		unsigned int out_size, double connectivity, unsigned int num_backward_connections)
{
	Network* network = new Network();

	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

	network->add_ensemble(in);
	network->add_ensemble(bias);


	for (unsigned int k=0; k < hid_size; k++)
	{

		Ensemble* ge;

		if (k%2==0)
			ge = new GateEnsemble();
		else
			ge = new LinearMemoryEnsemble(1);


		network->connect_ensembles(in, ge, true);
		network->connect_ensembles(bias, ge, true);

		// connect other Ensembles to new Ensemble with connectivity factor
		unsigned int c = 0;
		for (unsigned int i=2; i < network->ensembles.size(); i++)
		{
			if (rand_uniform_01() <= connectivity) {
				network->connect_ensembles( network->ensembles[i], ge, true);
				c++;
//				cout << "connect " << i << " to " << "current hidden " << k << endl;

			}
		}
//		cout << "connected " << c << endl;

		network->add_ensemble(ge);
		network->connect_ensembles(ge, out, true);

	}
	network->add_ensemble(out);

	network->size = network->nodes.size();

//	network->sanity_check(true);
	cout << "nodes: " << network->nodes.size() << endl;
	if (network->has_cycle())
		error("Network has cycle!");
	network->sort_nodes();

	for (unsigned int i=2; i < network->ensembles.size()-1; i++)
	{
		for (unsigned int j=2; j < network->ensembles.size()-1; j++)
		{
			if (i > j)	// IMPORTANT XXX BUG hier war >= und alles war gut!
			network->connect_ensembles( network->ensembles[i],
					network->ensembles[j],false);
		}
	}
	/*
	// choose random hidden nodes and establish backward connections
	set<string> connections;
	for (int i=0; i < num_backward_connections; i++) {
		unsigned int idx1 = network->hid_offset + rand(0, network->hid_size-1);
		unsigned int idx2 = network->hid_offset + rand(0, network->hid_size-1);
		stringstream s;
		s << idx1 << "->" << idx2;
		if (idx1==idx2) {
			i--;
			continue;
		}
		if (connections.find(s.str()) == connections.end()) {
		 connections.insert(s.str());
		 network->connect( network->nodes[idx1], network->nodes[idx2], false);
		} else {
			i--; // search again
		}

	}
*/
	cout << "out: " << network->out_size << endl;

	return network;
}

Network* NetworkFactory::create_2ndorder(unsigned int in_size, unsigned int out_size)
{
	Network* net = new Network();

	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	net->add_ensemble(in);

	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	net->add_ensemble(out);

	for (unsigned int i=0; i < out_size; i++) {
		SecondOrderEnsemble* second = new SecondOrderEnsemble(&in->nodes, &out->nodes);
		net->add_ensemble( second );
	}

	net->sort_nodes();

	return net;
}

Network* NetworkFactory::create_2ndorder_lstm(unsigned int in_size, unsigned int lstm_size, unsigned int out_size)
{

	Network* net = new Network();

	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	net->add_ensemble(in);

	FeedforwardEnsemble* hidsum = new FeedforwardEnsemble(Node::TANH_NODE, lstm_size);
	net->add_ensemble(hidsum);

	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	net->add_ensemble(out);

	vector<Ensemble*> lstms;
	vector<Node*> lstm_outs;
	for (unsigned int i=0; i < lstm_size; i++)
	{
		LSTMForgetEnsemble* ensemble = new LSTMForgetEnsemble(true, Node::TANH_NODE);
		lstms.push_back(ensemble);
		lstm_outs.push_back( ensemble->get_output() );
		net->add_ensemble(ensemble);

		net->connect_ensembles(in, ensemble, true);
	}

	SecondOrderEnsemble* second = new SecondOrderEnsemble(
			&lstm_outs, hidsum->get_inputs());

	net->add_ensemble(second);

	for (unsigned int i=0; i < lstms.size(); i++)
		net->connect_ensembles(hidsum, lstms[i], false);

/*	for (unsigned int i=0; i < second_size; i++) {
		SecondOrderEnsemble* second = new SecondOrderEnsemble(&lstm_outs, out->get_inputs());
		net->add_ensemble( second );

		net->connect_ensembles(second, out, true);

		for (unsigned int j=0; j < lstms.size(); j++)
		{
			net->connect_ensembles(second, lstms[j], false);
		}
	}
*/

	net->connect_ensembles(hidsum, out, true);

	net->sort_nodes();

	return net;
}

Network* NetworkFactory::create_gers02a()
{
	unsigned int in_size = 1;
	unsigned int out_size = 1;
	bool peepholes = false;
	unsigned int lstm_act_type = Node::LINEAR_NODE;

	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);

	Ensemble* lstm = new LSTMForgetEnsemble( peepholes,Node::LINEAR_NODE, lstm_act_type,
			 0, -2, +2 );

	Network* network = new Network();

	network->connect_ensembles(in, lstm, true);
	network->connect_ensembles(bias, lstm, true);
	network->connect_ensembles(lstm, lstm, false);
	network->connect_ensembles(lstm, out, true);
	network->connect_ensembles(bias, out, true);

	network->add_ensemble(bias);
	network->add_ensemble(in);
	network->add_ensemble(lstm);
	network->add_ensemble(out);

	network->sort_nodes();

	return network;
}

Network* NetworkFactory::create_tsin(unsigned int in_size, unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	FeedforwardEnsemble* in = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	RecurrentEnsemble* hid = new RecurrentEnsemble(Node::TANH_NODE, hid_size);
	FeedforwardEnsemble* sin = new FeedforwardEnsemble(Node::TSIN_NODE, hid_size);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);

	network->add_ensemble(bias);
	network->add_ensemble(in);
	network->add_ensemble(hid);
	network->add_ensemble(out);
	network->add_ensemble(sin);

	network->connect_ensembles(in, hid, true);
	network->connect_ensembles(hid, sin,true);
	network->connect_ensembles(sin, out, true);
	//network->connect_ensembles(bias, hid, true);
	network->connect_ensembles(bias, sin, true);

	for (unsigned int i=0; i < in->nodes.size();i++)
	{
		for (unsigned int j=0; j < in->nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* conn = in->nodes[i]->outgoing_connections[j];
			conn->weight = 0.0;
			conn->freeze_weight = true;
		}
	}

	network->sort_nodes();

	return network;
}

Network* NetworkFactory::create_harmonic_generator(unsigned int num_generators, unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE,1);
	network->add_ensemble(bias);


	FeedforwardEnsemble* hidden = new FeedforwardEnsemble(Node::LINEAR_NODE, hid_size);
	network->add_ensemble(hidden);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	network->add_ensemble(out);

	vector<Ensemble*> generators;

	for (unsigned int i=0; i < num_generators; i++) {
		Ensemble* generator = new HarmonicGenerator(Node::TANH_NODE, 5);
				//new RecurrentEnsemble(Node::TANH_NODE, 3);
		network->add_ensemble(generator);
		generators.push_back(generator);
		network->connect_ensembles(bias, generator, true);
		network->connect_ensembles(generator, hidden, true);
	}


	network->connect_ensembles(bias, hidden, true);
	network->connect_ensembles(hidden, out, true);

	network->sort_nodes();

	network->hid_offset-=network->in_size;
	network->in_size=0;

	network->print_offsets();

	return network;

}

	 
Network* NetworkFactory::create_stacked_slow(unsigned int in_size, unsigned int num_layers,
 unsigned int hid_size[], double hid_slow[], unsigned int out_size)
{
	Network* network = new Network();

	FeedforwardEnsemble* in= new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	network->add_ensemble(in);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	network->add_ensemble(out);

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	network->add_ensemble(bias);


	Ensemble* last = in;
	for (unsigned int i=0; i < num_layers; i++)
	{
		Ensemble* hidden = new SlowEnsemble(Node::TANH_NODE, hid_size[i], hid_slow[i], false);
		network->connect_ensembles(last, hidden, true);
		network->connect_ensembles(bias, hidden, true);
		network->add_ensemble(hidden);
		last = hidden;
	}
	network->connect_ensembles(last, out, true);

	network->sort_nodes(in, out);

	return network;
}

 Network* NetworkFactory::create_slow_feature_network(unsigned int in_size,unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();

	FeedforwardEnsemble* in= new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	network->add_ensemble(in);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	network->add_ensemble(out);

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	network->add_ensemble(bias);

	Ensemble* reduction = new SlowEnsemble(Node::LINEAR_NODE, hid_size, 100, true);
	network->add_ensemble(reduction);

	Ensemble* kernel = new SecondOrderEnsemble(&in->nodes, &reduction->nodes, true);
	network->add_ensemble(kernel);

	network->connect_ensembles(reduction, out, true);
	//Ensemble* reconstruction = new
	//network->add_ensemble(reconstruction);

	network->sort_nodes(in, out);

	return network;
}


Network* NetworkFactory::create_slow_feature_network_v2(unsigned int in_size,unsigned int hid_size, unsigned int out_size)
{
	Network* network = new Network();

	FeedforwardEnsemble* in= new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
	network->add_ensemble(in);
	FeedforwardEnsemble* out = new FeedforwardEnsemble(Node::LINEAR_NODE, out_size);
	network->add_ensemble(out);

	FeedforwardEnsemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
	network->add_ensemble(bias);

	Ensemble* reduction = new SubtractiveEnsemble(Node::LINEAR_NODE, hid_size);
			//new SlowEnsemble(Node::LINEAR_NODE, hid_size, 100, true);
	network->add_ensemble(reduction);

	EnsembleTransformation* slow_transform = new IncreasingSlownessTransformation(0,0.1);
	slow_transform->apply_on( network, reduction );

	Ensemble* kernel = new SecondOrderEnsemble(&in->nodes, &reduction->nodes, true);
	network->add_ensemble(kernel);

	network->connect_ensembles(reduction, out, true);
	//Ensemble* reconstruction = new
	//network->add_ensemble(reconstruction);

	network->sanity_allows_deadends = true;

	network->sort_nodes(in, out);

	return network;
}

