/*
 * EnsembleTransformation.h
 *
 *  Created on: Feb 9, 2010
 *      Author: brandmaier
 */

#ifndef ENSEMBLETRANSFORMATION_H_
#define ENSEMBLETRANSFORMATION_H_

struct EnsembleTransformation
{
	EnsembleTransformation() {};
	virtual ~EnsembleTransformation() {};

	virtual void apply_on(Network* network, Ensemble* ensemble) =0;

};


struct SlownessTransformation : EnsembleTransformation
{
	double slowness;

	SlownessTransformation(double slowness)
	{
		this->slowness = slowness;
	}

	void apply_on(Network* network, Ensemble* ensemble)
	{
		Node* zero_target_node = new PseudoOutputNode();

		for (unsigned int i=0; i < ensemble->nodes.size(); i++) {
			Connection* sc1 = Network::connect( ensemble->nodes[i],
					zero_target_node, true);
			Connection* sc2 = Network::connect( ensemble->nodes[i],
					zero_target_node, false);

			//double factor =  0.0; //0.001;
			sc1->freeze_weight = true;
			sc2->freeze_weight = true;

			double w = 0;

			/*if (increasing_slowness) {
				w = i*slowness;
			} else {*/
				w = slowness;
			//}

			sc1->weight = w;
			sc2->weight = w;
		}


		network->add_node(zero_target_node);
	}
};

struct IncreasingSlownessTransformation : EnsembleTransformation
{
	double slowness_start;
	double slowness_increase;

	IncreasingSlownessTransformation(double slowness_start, double slowness_increase)
	{
		this->slowness_start = slowness_start;
		this->slowness_increase = slowness_increase;
	}

	void apply_on(Network* network, Ensemble* ensemble)
	{
		Node* zero_target_node = new PseudoOutputNode();

		for (unsigned int i=0; i < ensemble->nodes.size(); i++) {
			Connection* sc1 = Network::connect( ensemble->nodes[i],
					zero_target_node, true);
			Connection* sc2 = Network::connect( ensemble->nodes[i],
					zero_target_node, false);

			//double factor =  0.0; //0.001;
			sc1->freeze_weight = true;
			sc2->freeze_weight = true;

			double w = 0;

			/*if (increasing_slowness) {
				w = i*slowness;
			} else {*/
				w = slowness_start + i*slowness_increase;
			//}

			sc1->weight = w;
			sc2->weight = w;
		}


		network->add_node(zero_target_node);
	}
};

#endif /* ENSEMBLETRANSFORMATION_H_ */
