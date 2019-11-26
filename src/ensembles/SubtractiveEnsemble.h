/*
 * SubtractiveEnsemble.h
 *
 *  Created on: Feb 5, 2010
 *      Author: brandmaier
 */

#ifndef SUBTRACTIVEENSEMBLE_H_
#define SUBTRACTIVEENSEMBLE_H_

#include "FeedforwardEnsemble.h"
#include <vector>
#include "../nodes/Node.h"

struct SubtractiveEnsemble : FeedforwardEnsemble
{

	SubtractiveEnsemble(int nodetype, unsigned int size) : FeedforwardEnsemble(nodetype, size)
	{
		name = "SubtractiveEnsemble";
		init();

	}

	SubtractiveEnsemble(vector<Node*>* nodes) : FeedforwardEnsemble( nodes )
		{

		}

	void init()
	{
		for (unsigned int i=1; i < nodes.size(); i++)
		{
			for (unsigned int j=0; j < i; j++) {
				Connection* c = Network::connect(nodes[j],nodes[i], true, -1.0);
				c->freeze_weight = true;
			}
		}
	}

	virtual const unsigned int get_type() { return SUBTRACTIVE_ENSEMBLE; }

	static SubtractiveEnsemble* from_string(string parameters, vector<Node*>* nodes)
	{
		vector<string> tokens;
	    string sep = std::string(";");
	    split(parameters,sep,tokens);

		 int type = atoi(tokens[0].c_str());
		unsigned int size = atoi(tokens[1].c_str());

		for (unsigned int i=0; i < nodes->size(); i++)
		{
			if ((*nodes)[i]->get_type() != type) {
			 cout << type << "/" << (*nodes)[i]->get_type() << endl;
			 error("Cannot instantiate SubtractiveEnsemble from string! Node type mismatch! ");
			 exit(0);
			}
		}

		if (size != nodes->size())
		{
			 error("Cannot instantiate SubtractiveEnsemble from string! Wrong node size! ");
			exit(0);
		}

		SubtractiveEnsemble* ensemble = new SubtractiveEnsemble(nodes);
		return ensemble;
	}


	string to_string()
	{
		stringstream stream;
		stream << node_type << ";" << nodes.size();
		return stream.str();
	}


};

#endif /* SUBTRACTIVEENSEMBLE_H_ */
