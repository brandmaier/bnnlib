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
                     double sparsity);
  
  Network* createDecompressionNetwork();
    
};

#endif /* AUTOASSOCIATIONNETWORK_H_ */
