#ifndef __WRAPPER_H_
#define __WRAPPER_H_

#include <vector>
#include <string>
#include "definitions.h"

using namespace std;



std::vector<double> toValue(std::vector<double>* v);

string getString(std::vector<string> x, unsigned int i);
//std::vector<>
//double[] toArray(vector<vector<weight_t>*>* x);
//double get


std::vector<double> getRow(vector<vector<weight_t>*>* x, unsigned int i);

unsigned int getLength(vector<vector<weight_t>*>* x);

void setRandomSeed(int i);

#endif