/*
 * Parameter.h
 *
 *  Created on: Jan 19, 2010
 *      Author: brandmaier
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_


using namespace std;

struct Parameter
{

	enum parameter_type
	{
		type_double, type_int, type_bool
	};

	parameter_type type;

	union
	{
		double value_double;
		int value_int;
		bool value_bool;
	};

	Parameter() {

	}

	Parameter(double value) {
		set_double(value);
	}

	Parameter(bool value) {
		set_bool(value);
	}

	Parameter(int value) {
		set_int(value);
	}

	void set_double(double value) {
		type = type_double;
		value_double = value;
	}
	void set_int(int value) {
		type = type_int;
		value_int = value;
	}

	void set_bool(bool value) {
		type = type_bool;
		value_bool = value;
	}

	double get_double() {
		return value_double;
	}

	int get_int() {
		return value_int;
	}

	bool get_bool() {
		return value_bool;
	}
};

#endif /* PARAMETER_H_ */
