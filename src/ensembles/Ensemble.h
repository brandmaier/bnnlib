#ifndef __ENSEMBLE_H_
#define __ENSEMBLE_H_

#include <vector>
#include <string>
#include "../nodes/Node.h"
#include "../Network.h"
#include <sstream>
#include <vector>


//struct Node;

using namespace std;

struct Ensemble
{
	std::vector<Node*> nodes;
	string name;
	//std::vector<Connection*> connections;

	static const unsigned int GENERIC_ENSEMBLE = 1;
	static const unsigned int LSTM_ENSEMBLE = 2;
	static const unsigned int LSTM_FORGET_ENSEMBLE = 3;
	static const unsigned int LSTM_PLUS_ENSEMBLE = 4;
	static const unsigned int KEEP_ENSEMBLE = 5;
	static const unsigned int LSTM_COUNTER_ENSEMBLE = 6;
	static const unsigned int FEEDFORWARD_ENSEMBLE = 7;
	static const unsigned int RECURRENT_ENSEMBLE = 8;
	static const unsigned int WINNER_TAKES_ALL_ENSEMBLE = 9;
	static const unsigned int LSTM_COPYRESTORE_ENSEMBLE = 10;
	static const unsigned int CONDITIONAL_BIAS_ENSEMBLE = 11;
	static const unsigned int GATE_ENSEMBLE = 12;
	static const unsigned int LINEAR_MEMORY_ENSEMBLE = 13;
	static const unsigned int SECONDORDER_ENSEMBLE = 14;
	static const unsigned int HARMONIC_GENERATOR_ENSEMBLE = 15;
	static const unsigned int DELAY_ENSEMBLE = 16;
	static const unsigned int SLOW_ENSEMBLE = 17;
	//static const unsigned int TSIN_ENSEMBLE =
	static const unsigned int SUBTRACTIVE_ENSEMBLE = 18;
	
	Ensemble();
	Ensemble(std::vector<Node*>* nodes);
	virtual ~Ensemble();
	
	void add_node(Node* node);	
	bool contains_node(Node* node);
	
	static Ensemble* create_fullrecurrent_ensemble(unsigned int size);
	static Ensemble* create_stacked_ensemble(unsigned int size);	
	static Ensemble* create_selfrecurrent_ensemble(unsigned int size);
	
	virtual std::vector<Node*>* get_inputs() { return &this->nodes; }
	virtual std::vector<Node*>* get_outputs() {return &this->nodes; }

	unsigned int get_num_nodes();
	
	virtual string to_string();
	
	virtual const unsigned int get_type() { return GENERIC_ENSEMBLE; }
	
	virtual void set_frozen(bool state) {
	 for (unsigned int i=0; i < nodes.size(); i++)
	 {
		for (unsigned int j=0; j < nodes[i]->outgoing_connections.size(); j++)
		{
			nodes[i]->outgoing_connections[j]->freeze_weight = state;
		}
	 }
	}
};

#endif /*ENSEMBLE_H_*/
