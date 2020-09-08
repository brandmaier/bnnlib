#ifndef NODEFACTORY_H_
#define NODEFACTORY_H_

#include "nodes/Node.h"
#include "nodes/SigmoidNode.h"
#include "nodes/TanhNode.h"
#include "nodes/LinearNode.h"
#include "nodes/BiasNode.h"
#include "nodes/SinNode.h"
#include "nodes/TimedSinNode.h"
#include "nodes/ReLUNode.h"
#include "nodes/OneShotBiasNode.h"
#include "nodes/GaussianNoiseNode.h"
#include "nodes/StochasticSigmoidNode.h"

struct NodeFactory
{

	virtual void i_am_abstract() = 0;
	
  static Node* createNode(int node_type) {
	  Node* node = NULL;
	  switch(node_type) {
	  case Node::TANH_NODE:
	    node = new TanhNode();
	    break;
	  case Node::SIGMOID_NODE:
	    node = new SigmoidNode();
	    break;
	  case Node::LINEAR_NODE:
	    node = new LinearNode();
	    break;
	  case Node::BIAS_NODE:
	    node = new BiasNode();
	    break;
	  case Node::TSIN_NODE:
	    node = new TimedSinNode();
	    break;
	  case Node::SIN_NODE:
	    node = new SinNode();
	    break;
	  case Node::ONE_SHOT_BIAS_NODE:
	    node = new OneShotBiasNode();
	    break;
	  case Node::GAUSSIAN_NOISE_NODE:
	    node = new GaussianNoiseNode();
	    break;
	  case Node::RELU_NODE:
	    node = new ReLUNode();
	    break;
	  case Node::STOCHASTIC_SIGMOID_NODE:
	    node = new StochasticSigmoidNode();
	    break;
	  default:
	    error( "No matching Node type in Node Factory class!");;
	  break;
	  
	
  	}
	  return(node);
  }
	  
};


#endif /*NODEFACTORY_H_*/