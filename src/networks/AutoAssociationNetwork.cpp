#include "AutoAssociationNetwork.h"

#include "../ensembles/Ensemble.h"
#include "../ensembles/FeedforwardEnsemble.h"
#include <vector>
#include "../nodes/Node.h"

AutoAssociationNetwork::AutoAssociationNetwork(unsigned int in_size, unsigned int hid_size,
                       int hid_node_type, int out_node_type,
                       double sparsity)
{
  this->hid_node_type = hid_node_type;
  this->out_node_type = out_node_type;
  
  this->in_size = in_size;
  this->hid_size = hid_size;
  
  input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
  hidden = new FeedforwardEnsemble(hid_node_type, hid_size);
  output_ensemble= new FeedforwardEnsemble(out_node_type, in_size);
  
  input_ensemble->rename("Input");
  hidden->rename("Bottleneck");
  output_ensemble->rename("Output");
  
  hidden->set_sparsity(sparsity);
  
  bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
  
  add_ensemble(bias);
  add_ensemble(input_ensemble);
  add_ensemble(hidden);
  add_ensemble(output_ensemble);

  
  connect_ensembles(input_ensemble, hidden, true);
  connect_ensembles(hidden, output_ensemble, true);
  connect_ensembles(bias, output_ensemble, true);
  connect_ensembles(bias, hidden, true);
  
  
  
  sort_nodes(input_ensemble, output_ensemble);
}

Network* AutoAssociationNetwork::createDecompressionNetwork() {
  
  // (1) create network
  
  Network* net = new Network();
  Ensemble* inp = new FeedforwardEnsemble(Node::LINEAR_NODE, hid_size);
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