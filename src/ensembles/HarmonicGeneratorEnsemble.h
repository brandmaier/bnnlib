/*
 * HarmonicGeneratorEnsemble.h
 *
 * The Harmonic Generator is in principle not different to a fully recurrent ensemble of Nodes. The singĺe difference is that
 * the Harmonic Generator has a single designated unit that acts as output unit.
 *
 *  Created on: Jan 4, 2010
 *      Author: brandmaier
 */

#ifndef HARMONICGENERATORENSEMBLE_H_
#define HARMONICGENERATORENSEMBLE_H_

#include "RecurrentEnsemble.h"
#include "FeedforwardEnsemble.h"
#include <cassert>
#include "../Network.h"

struct HarmonicGenerator : FeedforwardEnsemble
{
	std::vector<Node*> outputs;
	std::vector<Node*> inputs;

	HarmonicGenerator(std::vector<Node*>* nodes) : FeedforwardEnsemble( nodes )
		{

		}

	HarmonicGenerator(int node_type, unsigned int size) : FeedforwardEnsemble(node_type,size)
	{
		assert(size >= 2);


		init();
	}

	HarmonicGenerator(unsigned int size) : FeedforwardEnsemble(Node::TANH_NODE,size)
	{
		assert(size >= 2);


		init();

	}

	void init();

	virtual const unsigned int get_type() { return HARMONIC_GENERATOR_ENSEMBLE; }

	virtual std::vector<Node*>* get_outputs()
	{
		return &outputs;
	}

	virtual Node* get_output()
	{
		return outputs[0];
	}

	virtual std::vector<Node*>* get_inputs()
	{
		return &inputs;
	}

	static HarmonicGenerator* from_string(string parameters)
	{
		return static_cast<HarmonicGenerator*>(FeedforwardEnsemble::from_string(parameters));
	}

	static HarmonicGenerator* from_string(string from_string, vector<Node*>* nodes)
	{
		return static_cast<HarmonicGenerator*>(FeedforwardEnsemble::from_string(from_string, nodes));

	}

};

#endif /* HARMONICGENERATORENSEMBLE_H_ */
