/*
 * SlowEnsemble.h
 *
 *  Created on: Jan 31, 2010
 *      Author: brandmaier
 */

#ifndef SLOWENSEMBLE_H_
#define SLOWENSEMBLE_H_

#include "RecurrentEnsemble.h"
#include "../nodes/PseudoOutputNode.h"

struct SlowEnsemble : RecurrentEnsemble
{
	double slowness;
	bool increasing_slowness;
	Node* zero_target_node;

	SlowEnsemble(int node_type, unsigned int size, double factor, bool increasing_slowness) : RecurrentEnsemble(node_type, size)
	{
		name = "SlowEnsemble";
		init();
		this->slowness = factor;
		this->increasing_slowness = increasing_slowness;
	}

	SlowEnsemble(vector<Node*>* nodes) : RecurrentEnsemble(nodes)
	{

	}

	virtual const unsigned int get_type() { return SLOW_ENSEMBLE; }

	void init()
	{

		zero_target_node = new PseudoOutputNode();

		for (unsigned int i=0; i < nodes.size(); i++) {
			Connection* sc1 = Network::connect( nodes[i],
					zero_target_node, true);
			Connection* sc2 = Network::connect( nodes[i],
					zero_target_node, false);

			//double factor =  0.0; //0.001;
			sc1->freeze_weight = true;
			sc2->freeze_weight = true;

			double w = 0;

			if (increasing_slowness) {
				w = i*slowness;
			} else {
				w = slowness;
			}

			sc1->weight = w;
			sc2->weight = w;
		}


		this->add_node(zero_target_node);
	}

	static SlowEnsemble* from_string(string parameters, vector<Node*>* nodes)
	{
		vector<string> tokens;
	    string sep = std::string(";");
	    split(parameters,sep,tokens);

	    return new SlowEnsemble( nodes );
	}

	static SlowEnsemble* from_string(string parameters)
	{
		vector<string> tokens;
	    string sep = std::string(";");
	    split(parameters,sep,tokens);

		int type = atoi(tokens[0].c_str());
		unsigned int size = atoi(tokens[1].c_str());

		int node_type = atoi(tokens[2].c_str());
		bool inc = (atoi(tokens[3].c_str())==1);

		return new SlowEnsemble(type, size, node_type, inc);
	}

	string to_string()
	{
		stringstream repr;
		repr << node_type << ";" << nodes.size() << ";" << slowness << ";" << increasing_slowness;
		return repr.str();
	}
};

#endif /* SLOWENSEMBLE_H_ */
