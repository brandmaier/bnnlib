#include "TanhNode.h"
#include "../functions.h"
#include "Node.h"
#include <sstream>
#include <string>

using namespace std;

	TanhNode::TanhNode()
	{
		activation_function = &tanh;
		derivative_activation_function = &dtanh;
		
		stringstream namestream;
		namestream << "Tanh" << id;
		name = namestream.str();

	}
