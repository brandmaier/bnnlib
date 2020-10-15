/*
 * AutoAssociatonNetwork.h
 *
 *  Created on: Oct 15, 2020
 *      Author: brandmaier
 */

#ifndef AUTOASSOCIATIONDNETWORK_H_
#define AUTOASSOCIATIONNETWORK_H_

#include "../Network.h"

struct AutoAssociationNetwork : Network
{
  //Ensemble* in;
  Ensemble* hidden;
  //Ensemble* out;
  Ensemble* bias;
  
  AutoAssociationNetwork(unsigned int in_size, unsigned int hid_size,
                     int hid_node_type, int out_node_type,
                     double sparsity)
  {
    input_ensemble = new FeedForwardEnsemble(Node::LINEAR_NODE, in_size);
    hidden = new FeedForwardEnsemble(hid_node_type, hid_size);
    output_ensemble= new FeedforwardEnsemble(out_node_type, out_size);
    
    input_ensemble->rename("Input");
    hidden->rename("Bottleneck");
    output_ensemble->rename("Output");
    
    hidden->set_sparsity(sparsity);
    
    bias = new FeedForwardEnsemble(Node::BIAS_NODE, 1);
    
    connect_ensembles(input_ensemble, hidden, true);
    connect_ensembles(hidden, output_ensemble, true);
    connect_ensembles(bias, output_ensemble, true);
    connect_ensembles(bias, hidden, true);
    
    add_ensemble(input_ensemble);
    add_ensemble(hidden);
    add_ensemble(output_ensemble);
    add_ensemble(bias);
    
    sort_nodes(input_ensemble, output_ensemble);
  }
  
  Network createDecompressionNetwork() {
    
    // (1) create network
    
    Network* net = new Network();
    Ensemble* inp = new FeedForwardEnsemble(Node::LINEAR_NODE, in_size);
    Ensemble* out = new FeedForwardEnsemble(Node::LINEAR_NODE, in_size);
    Ensemble* bias = new FeedForwardEnsemble(Node::BIAS_NODE, 1);

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
  
};

#endif /* AUTOASSOCIATIONNETWORK_H_ */
