/*
 * ParameterSet.h
 *
 *  Created on: Jan 28, 2010
 *      Author: brandmaier
 */

#ifndef PARAMETERSET_H_
#define PARAMETERSET_H_

#include "Parameter.h"

struct ParameterSet
{
	ParameterSet()
	{

	}

	map<string, Parameter> parameters;

	bool exists(string name)
	{
		return parameters.find(name) != parameters.end();
	}

	void set(string name, double value)
	{
		parameters[name] = Parameter(value);
	}

	void set(string name, int value)
	{
		parameters[name] = Parameter(value);
	}

	void set(string name, bool value)
	{
		parameters[name] = Parameter(value);
	}

	double get_double(string name) {
		return parameters[name].get_double();
	}

	bool get_bool(string name) {
		return parameters[name].get_bool();
	}

	int get_int(string name) {
		return parameters[name].get_int();
	}

	double get_string(string name);

};

#endif /* PARAMETERSET_H_ */
