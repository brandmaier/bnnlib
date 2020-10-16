/*
 * AutoAssociatonNetwork.h
 *
 *  Created on: Oct 15, 2020
 *      Author: brandmaier
 */

#ifndef AUTOASSOCIATIONNETWORK_H_
#define AUTOASSOCIATIONNETWORK_H_

#include "../ensembles/Ensemble.h"
#include "../ensembles/FeedforwardEnsemble.h"
#include <vector>
#include "../nodes/Node.h"

struct AutoAssociationNetwork : Network
{
  //Ensemble* in;
  Ensemble* hidden;
  //Ensemble* out;
  Ensemble* bias;
  
  int hid_node_type, out_node_type;
  
  AutoAssociationNetwork(unsigned int in_size, unsigned int hid_size,
                     int hid_node_type, int out_node_type,
                     double sparsity)
  {
    this->hid_node_type = hid_node_type;
    this->out_node_type = out_node_type;
    
    input_ensemble = new FeedforwardEnsemble(Node::LINEAR_NODE, in_size);
    hidden = new FeedforwardEnsemble(hid_node_type, hid_size);
    output_ensemble= new FeedforwardEnsemble(out_node_type, in_size);
    
    input_ensemble->rename("Input");
    hidden->rename("Bottleneck");
    output_ensemble->rename("Output");
    
   // hidden->set_sparsity(sparsity);
    
    bias = new FeedforwardEnsemble(Node::BIAS_NODE, 1);
    
    add_ensemble(input_ensemble);
    add_ensemble(hidden);
    add_ensemble(output_ensemble);
    add_ensemble(bias);
    
    connect_ensembles(input_ensemble, hidden, true);
    connect_ensembles(hidden, output_ensemble, true);
    connect_ensembles(bias, output_ensemble, true);
    connect_ensembles(bias, hidden, true);
    

    
    sort_nodes(input_ensemble, output_ensemble);
  }
  
  Network* createDecompressionNetwork();
    
};

#endif /* AUTOASSOCIATIONNETWORK_H_ */
