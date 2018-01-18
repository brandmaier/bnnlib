#include "WinnerTakesAllEnsemble.h"
#include "FeedforwardEnsemble.h"
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <sstream>


WinnerTakesAllEnsemble::WinnerTakesAllEnsemble(int node_type, unsigned int size) 
: FeedforwardEnsemble(node_type, size)
{
//	assert(size > 1);
	if (size <= 1)
		error("WinnerTakesAllEnsemble cannot be used for a single output node (or none at all)!");
}

void WinnerTakesAllEnsemble::_after_activation(unsigned int t)
{
	weight_t sum = 0.0;
//	weight_t max = 0.0;
	
	for (unsigned int i=0; i < nodes.size(); i++)
	{
		//weight_t item =

		sum += exp(nodes[i]->actbuf[t]);
//		if (nodes[i]->actbuf[t] > max) max = nodes[i]->actbuf[t];
	}	

/*	if (isnan(sum) or isinf(sum)) {
		cout << "WTA ERROR! sum="<< sum << endl;
	}
*/
	assert(sum > 0 && sum == sum);

	//cout << "sum:" << sum << " " << nodes[0]->actbuf[t] << " -> " << safeexp(nodes[0]->actbuf[t]) << endl;

	for (unsigned int i=0; i < nodes.size(); i++)
	{
//		cout << i << ": " << nodes[i]->actbuf[t] << endl;
		nodes[i]->actbuf[t] = exp(nodes[i]->actbuf[t] /*- max*/) / sum;
	}	
	
}

WinnerTakesAllEnsemble* WinnerTakesAllEnsemble::from_string(string parameters)
{
	vector<string> tokens;
    string sep = std::string(";");
    split(parameters,sep,tokens);

	int type = atoi(tokens[0].c_str());
	unsigned int size = atoi(tokens[1].c_str());

	WinnerTakesAllEnsemble* ensemble = new WinnerTakesAllEnsemble(type, size);
	return ensemble;
}

WinnerTakesAllEnsemble* WinnerTakesAllEnsemble::from_string(string parameters, vector<Node*>* nodes)
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
		 error("Cannot instantiate FeedforwardEnsemble from string! Node type mismatch! ");
		 exit(0);
		}
	}

	if (size != nodes->size())
	{
		 error("Cannot instantiate FeedforwardEnsemble from string! Wrong node size! ");
		exit(0);
	}

	WinnerTakesAllEnsemble* ensemble = new WinnerTakesAllEnsemble(nodes);
	return ensemble;
}

WinnerTakesAllEnsemble::WinnerTakesAllEnsemble(vector<Node*>* nodes) : FeedforwardEnsemble(nodes)
{
	this->node_type = ((*nodes)[0])->get_type();
	this->name = "WTA Ensemble";
}


string WinnerTakesAllEnsemble::to_string()
{
	stringstream stream;
	stream << node_type << ";" << nodes.size();
	return stream.str();
}
