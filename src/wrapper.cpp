#include "wrapper.h"

std::vector<double> toValue(std::vector<double>* v) {
  return(*v);
}


/*double[][] toArray(vector<vector<weight_t>*>* x) {
  unsigned int size1 = x->size();
  unsigned int size2 = ((&x)[0]).size();
}*/

std::vector<double> getRow(vector<vector<weight_t>*>* x, unsigned int i) {
  return( *(*x)[i] );
}