/*
 * MatchOutputNode.h
 *
 *  Created on: Feb 1, 2010
 *      Author: brandmaier
 */

#ifndef MATCHOUTPUTNODE_H_
#define MATCHOUTPUTNODE_H_

/**
 * A MatchingOutputNode produces additional delta-terms to match
 * the activation of a selected other node. The other node has to
 * have been activated before the given node, otherwise the delta
 * terms are not calculated correctly.
 *
 */
struct MatchingOutputNode : Node
{
	Node* matching_node;

	MatchingOutputNode(Node* other_node)
	{
		assert(other_node->id < this->id);

		name = "MatchingOutput";
		this->matching_node = other_node;
	}

	void activate(unsigned int time)
	{
		this->actbuf[time] = this->inbuf[time];
		this->delta[time] += get_activation_function_derivative(time)
					* (actbuf[time] - matching_node->actbuf[time]) ;
	}

	weight_t  get_activation_function_derivative(unsigned int time)
	{
		return 1.0;
	}

	const int get_type() { return Node::MATCHING_OUTPUT_NODE; }

	string to_string()
	{
		stringstream out;
		out <<  matching_node->id;
		return out.str();
	}

	static MatchingOutputNode* from_string(string str)
	{
		error("Error when loading MachtingOutputNode! Need explicit reference to Network*.");
		//MatchingOutputNode* node = new MatchingOutputNode();
		return NULL;
	}

	static MatchingOutputNode* from_string(Network* network, string str)
	{
		int id = atoi(str.c_str());
		assert( network->nodes[id]->id == id);
		MatchingOutputNode* node = new MatchingOutputNode( network->nodes[id] );

		return node;
	}

};

#endif /* MATCHOUTPUTNODE_H_ */
