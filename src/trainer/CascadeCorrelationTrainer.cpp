/*
 * CascadeCorrelationTrainer.cpp
 *
 *  Created on: Dec 17, 2009
 *      Author: brandmaier
 */

#include "CascadeCorrelationTrainer.h"


CascadeCorrelationTrainer::CascadeCorrelationTrainer(Network* network) : Trainer() {
	this->network = network;

}

CascadeCorrelationTrainer::~CascadeCorrelationTrainer() {

}

/*
 * - Bereche deltas für alle Ausgabezellen und speichere diese
 * - Füge neuen Knoten hinzu
 * - Trainiere nur Gewichte zu diesem Knoten, so dass Korrelation
 * zwischen den Deltas und diesem Knoten maximal ist. CascadeCorrelation liefert
 * dabei eine neue Berechnung des Gradienten. Der eigentliche Update
 * wird von einem der anderen Trainer durchgeführt.
 *
 *
 */
void CascadeCorrelationTrainer::step(SequenceSet* sequences,	unsigned int steps = 100)
{
	// berechne deltas
	weight_t deltas[network->timestep][network->out_size];

	for (unsigned int i=0; i < sequences->size(); i++) {
		Sequence* sequence = (*sequences)[i];
		network->reset();
		network->reserve( sequence->size() );
		network->activate(sequence);
		for (unsigned int t=0; t < network->timestep; t++)
			for (unsigned int j=0; j < network->out_size; j++)
			{
				deltas[t][j] = network->nodes[network->out_offset+j]->delta[t];
			}
	}

	// füge CandidateUnit hinzu
	Node* candidate = new TanhNode();
	network->add_node(candidate);

	// verbinde CandidateUnit mit Netzwerk;
	for (unsigned int i=0; i < network->size-1; i++)
	{
		Network::connect(network->nodes[i], candidate, true);
	}
	
	// verbinde CandidateUnit mit OutputUnit

	// sortiere Netzwerk
	network->sort_nodes();

	// trainiere die neuen Kanten
	ImprovedRPropTrainer trainer(network);


	for (unsigned int step=0; step < steps; step++)
	{
		// berechne Gradient für alle neuen Verbindungen
		for (unsigned int i=0; i < network->out_size; i++)
		{
			Node* node = network->nodes[network->out_offset+i];
			for (unsigned int j=0; j < node->incoming_connections.size(); j++)
			{
				Connection* conn = node->incoming_connections[j];

				//conn->derivative =
				trainer.change_weight( conn );
			}
		}
	}



}
