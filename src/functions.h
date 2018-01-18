/*
 *  functions.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 19.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <vector>
#include <ostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <string>

using namespace std;

#define __BNNLIB_VERSION 0.7

//#define weight_t double
typedef double weight_t;
typedef std::string string_t;


/*
float tanh(float x)
{
	return -1;
}*/

float rand_uniform_01();
int rand(int a, int b);
float rand_gauss(float mu, float sigma);

void save_vector_to_file(std::vector<weight_t>* v);
void save_vector_to_file(std::vector<std::vector<weight_t>*>* v);
void save_vector_to_file_transposed(std::vector<std::vector<weight_t>*>* v);
void save_errors_to_file(std::vector<weight_t>* v, std::vector<weight_t>* v2);

//template<typename T> void print_vector(std::vector<T>* v);
//template<typename T> void print_vector(std::vector<std::vector<T>*>* v);

void print_vector(std::vector<unsigned int>* v);
void print_vector(std::vector<float>* v);
void print_vector(std::vector<std::vector<float>*>* v);
void print_vector(std::vector<double>* v);
void print_vector(std::vector<std::vector<double>*>* v);

void stripLeadingAndTrailingBlanks(string& StringToModify);
/*
int my_random_int(int lowest, int highest)
{
	return lowest+int((highest-lowest+1)*rand()/(RAND_MAX + 1.0)); 
}*/

/*
bool random_bool()
{
	return (rand()/(RAND_MAX+1.0)) > 0.5;	
}
*/

weight_t safeexp(weight_t x);

extern float sigmoid(float x);
extern float dsigmoid(float x);
extern float linear(float x);
extern float dlinear(float x);
float tanh_efficient(float x);
float dtanh(float x);
float ln(float x);
float dln(float x);
extern double sigmoid(double x);
extern double dsigmoid(double x);
extern double linear(double x);
extern double dlinear(double x);
double tanh_efficient(double x);
double dtanh(double x);
double ln(double x);
double dln(double x);

void split(string& text, string& separators, vector<string>& words);

void error(string message);
void warning(string message);
void status(string message);
void message(string message);
void line();

unsigned int maxindex(vector<weight_t>* v);

double mean(vector<double>* x);
double mean(vector<unsigned int>* x);
double var(vector<double>* x);
double cov(vector<double>* x, vector<double>* y);
double corr(vector<double>* x, vector<double>* y);
vector<vector<weight_t>*>* transpose(vector<vector<weight_t>*>* v);

double min(vector<vector<weight_t>*>* v);
double max(vector<vector<weight_t>*>* v);

unsigned int randomize_seed();
unsigned int randomize_seed(unsigned int seed);
#endif
