#include "LnNode.h"
#include "../functions.h"

LnNode::LnNode()
{
		activation_function = &ln;
		derivative_activation_function = &dln;	
		
		name = "Ln";
}
