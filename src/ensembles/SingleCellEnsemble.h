/*
 * SingleCellEnsemble.h
 *
 *  Created on: Oct 5, 2009
 *      Author: brandmaier
 */

#ifndef __SINGLECELLENSEMBLE_H_
#define __SINGLECELLENSEMBLE_H_

#include "Ensemble.h"

struct SingleCellEnsemble : Ensemble
{

	SingleCellEnsemble(Node* node)
	{
		add_node(node);
	}

};

#endif /* SINGLECELLENSEMBLE_H_ */
