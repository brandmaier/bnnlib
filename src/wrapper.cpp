#include "wrapper.h"

std::vector<double> toValue(std::vector<double>* v) {
  return(*v);
}

std::vector<double> getRow(vector<vector<weight_t>*>* x, unsigned int i) {
  return( *(*x)[i] );
}

unsigned int getLength(vector<vector<weight_t>*>* x) {
  return(x->size());
}