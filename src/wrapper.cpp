#include "wrapper.h"
#include <cstdlib> // for srand

std::vector<double> toValue(std::vector<double>* v) {
  return(*v);
}

std::vector<double> getRow(vector<vector<weight_t>*>* x, unsigned int i) {
  return( *(*x)[i] );
}

unsigned int getLength(vector<vector<weight_t>*>* x) {
  return(x->size());
}

string getString(std::vector<string> x, unsigned int i) { return(x[i]);}

void setRandomSeed(int i) { srand(i); }
