#include "MultigroupEnsemble.h"



MultigroupEnsemble::MultigroupEnsemble()
{
  init();
}

void MultigroupEnsemble::init()
{

  name = "Multigroup Ensemble";
  
  Node* input = new LinearNode();
  Node* output = new LinearNode();
  Node* pinode = new PiNode();
  Node* constant = new BiasNode();
  constant->name = "Constant";
  
  Node* grp = new LinearNode(); // no incoming connection!
  grp->name = "DataNode";
  
  bool direction = true;
  Connection* c1 = Network::connect(input, pinode, direction);
  Connection* c2 = Network::connect(pinode, output, direction);
  c1->set_identity_and_freeze();
  c2->set_identity_and_freeze();
  
  Connection* c3 = Network::connect(constant, pinode, direction);
  
  // this is the group weight
  Connection* c4 = Network::connect(grp, pinode, direction);
  
  // collect all nodes
  nodes.push_back(input);
  nodes.push_back(output);
  nodes.push_back(pinode);
  nodes.push_back(constant);
  nodes.push_back(grp);
  
  // collect I/O
  inputs.push_back(input);
  outputs.push_back(output);
  
  // set Ensemble references, so that nodes know what family they belong to
  for (unsigned int i=0; i < nodes.size();i++)
  {
    nodes[i]->ensemble = this;
  }
}

MultigroupEnsemble::~MultigroupEnsemble()
{
  
}

std::vector<Node*>* MultigroupEnsemble::get_inputs() {
  return &this->inputs;
}

std::vector<Node*>* MultigroupEnsemble::get_outputs() {
  return &this->outputs;
}