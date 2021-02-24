#ifndef MULTIGROUPENSEMBLE_
#define MULTIGROUPENSEMBLE_

#include "Ensemble.h"
#include "../nodes/Node.h"
#include "../nodes/PiNode.h"
#include "../nodes/LinearNode.h"
#include "../Network.h"
#include "../nodes/SigmoidNode.h"
#include "../nodes/TanhNode.h"
#include "../nodes/BiasNode.h"

struct MultigroupEnsemble : Ensemble
{
  std::vector<Node*> outputs;
  std::vector<Node*> inputs;
  
  MultigroupEnsemble();
  ~MultigroupEnsemble();
  
  void init();
  
  virtual std::vector<Node*>* get_inputs();
  virtual std::vector<Node*>* get_outputs();
  
  virtual const unsigned int get_type() { return MULTIGROUP_ENSEMBLE; }	
};

#endif /*MULTIGROUPENSEMBLE_*/
