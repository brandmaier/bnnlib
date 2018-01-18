/*
 * LinearMemoryEnsemble.h
 *
 *  Created on: Nov 17, 2009
 *      Author: brandmaier
 */

#ifndef LINEARMEMORYENSEMBLE_H_
#define LINEARMEMORYENSEMBLE_H_

#include "../nodes/Node.h"
#include <vector>
#include "Ensemble.h"

struct LinearMemoryEnsemble : Ensemble
{
	unsigned int size;

	LinearMemoryEnsemble(vector<Node*>* nodes) : Ensemble(nodes)
	{
		name = "LinearMemory";
		this->size = nodes->size();
	}

	LinearMemoryEnsemble(unsigned int size)
	{
		name = "LinearMemory";
		this->size = size;

		for (unsigned int i=0; i < size; i++)
		{
			Node* node = new LinearNode();
			Connection* conn = Network::connect(node, node, false);
			conn->freeze_weight = true;

			this->nodes.push_back( node );
		}
	}

	virtual const unsigned int get_type() { return LINEAR_MEMORY_ENSEMBLE; }

	static LinearMemoryEnsemble* from_string(string parameters, vector<Node*>* nodes)
	{
		vector<string> tokens;
	    string sep = std::string(";");
	    split(parameters,sep,tokens);

	    unsigned int size = atoi(tokens[0].c_str());

		/*for (unsigned int i=0; i < nodes->size(); i++)
		{
			if ((*nodes)[i]->get_type() != type) {
			 cout << type << "/" << (*nodes)[i]->get_type() << endl;
			 error("Cannot instantiate FeedforwardEnsemble from string! Node type mismatch! ");
			 exit(0);
			}
		}
		*/
		if (size != nodes->size())
		{
			 error("Cannot instantiate FeedforwardEnsemble from string! Wrong node size! ");
			exit(0);
		}

		LinearMemoryEnsemble* ensemble = new LinearMemoryEnsemble(nodes);
		return ensemble;
	}


	string to_string()
	{
		stringstream out;
		out << size;
		return out.str();
	}

};

#endif /* LINEARMEMORYENSEMBLE_H_ */
