#include "AutoAssociationNetwork.h"

#include "../ensembles/Ensemble.h"
#include "../ensembles/FeedforwardEnsemble.h"
#include <vector>
#include "../nodes/Node.h"

Network* AutoAssociationNetwork::createDecompressionNetwork() {
  
  // (1) create network
  
  Network* net = new Network();
  Ensemble* inp = new FeedforwardEnsemble(this->hid_node_type, in_size);
  Ensemble* out = new FeedforwardEnsemble(this->out_node_type, in_size);
  Ensemble* bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
  
  net->add_ensemble(inp);
  net->add_ensemble(out);
  net->add_ensemble(bias);
  
  net->connect_ensembles(inp, out, true);
  net->connect_ensembles(bias, out, true);
  
  net->sort_nodes();
  
  // (2) copy weights
  std::vector<double>* weight_set1 = this->hidden->get_weights(this->output_ensemble);
  std::vector<double>* weight_set2 = this->bias->get_weights(this->output_ensemble);
  
  inp->set_weights(out, weight_set1);
  bias->set_weights(out, weight_set2);
  
  return(net);
}