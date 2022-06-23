#include "functions.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstdlib> // for srand
#include <ctime> // for time

weight_t safeexp(weight_t value)
{
	if (value > 100)
		return exp(100);
	else if (value < -100)
		return exp(-100);
	else
		return exp(value);
}

double mean(vector<double>* x)
{
	double sum = 0.0;
	for (unsigned int i=0; i < x->size(); i++)
		sum += (*x)[i];

	return sum/x->size();
}

double mean(vector<unsigned int>* x)
{
	double sum = 0.0;
	for (unsigned int i=0; i < x->size(); i++)
		sum += (*x)[i];

	return sum/x->size();
}

double var(vector<double>* x)
{
	double mu = mean(x);
	double sum = 0.0;
	for (unsigned int i=0; i < x->size(); i++)
		sum += ((*x)[i]-mu)*((*x)[i]-mu);

	return sum/(x->size()-1);
}

unsigned int maxindex(vector<weight_t>* v)
{
	weight_t mx = (*v)[0]; unsigned int idx = 0;
	for (unsigned int i=1; i < v->size(); i++)
		if ((*v)[i] > mx) { mx = (*v)[i]; idx=i; }
	return idx;

}

int rand(int a, int b)
{
	return a+(int)((rand()/(RAND_MAX +1.0))*(b-a+1));
}

float rand_uniform_01()
{
	return rand()/(RAND_MAX+1.0);	
}

float rand_gauss(float mu, float sigma)
{
	float u1 = rand_uniform_01();
	float u2 = rand_uniform_01();
	return (sqrt(-2.0*log(u1))*cos(2.0*M_PI*u2) + mu)*sigma;
}

void save_vector_to_file(std::vector<weight_t>* v)
{
	std::ofstream out("output.txt");
	std::copy((*v).begin(),(*v).end(),std::ostream_iterator<weight_t>(out,"\n"));
	out.close();
}

void save_vector_to_file(std::vector<std::vector<weight_t>*>* vv)
{
	std::ofstream out("output.txt");
	for (unsigned int i=0; i < vv->size(); i++) {
		std::vector<weight_t>* v = (*vv)[i];	
		std::copy((*v).begin(),(*v).end(),std::ostream_iterator<weight_t>(out,"\t"));	
		out << "\n";
	}
	out.close();	
}

void save_vector_to_file_transposed(std::vector<std::vector<weight_t>*>* vv)
{
	unsigned int inner_size = (*vv)[0]->size();
	unsigned int outer_size = vv->size();
	
	std::ofstream out("output.txt");
	for (unsigned int i=0; i < inner_size; i++) {
		for (unsigned int j=0; j < outer_size; j++) {
			out << (*(*vv)[j])[i] << "\t";
		}	
		out << endl;
	}
	out.close();	
}

void save_errors_to_file(std::vector<weight_t>* v, std::vector<weight_t>* v2)
{
	std::ofstream out("output.txt");
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		out << (*v)[i] << "\t" <<(*v2)[i]<< "\n";	
	}
	out.close();	
}


template<typename T> void print_vector(std::vector<T>* v)
{
	std::cout << "vector: ";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << (*v)[i] << " ";
	}
	std::cout << std::endl;	
}

void print_vector(std::vector<float>* v)
{
	std::cout << "vector: ";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << (*v)[i] << " ";
	}
	std::cout << std::endl;	
}

void print_vector(std::vector<double>* v)
{
	std::cout << "vector: ";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << (*v)[i] << " ";
	}
	std::cout << std::endl;	
}

void print_vector(std::vector<unsigned int>* v)
{
	std::cout << "vector: ";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << (*v)[i] << " ";
	}
	std::cout << std::endl;
}


void print_vector(std::vector<std::vector<float>*>* v)
{
	std::cout << "vector of vectors: \n";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << " |--";
		print_vector( (*v)[i] );
	}
	std::cout << std::endl;	
}

void print_vector(std::vector<std::vector<double>*>* v)
{
	std::cout << "vector of vectors: \n";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << " |--";
		print_vector( (*v)[i] );
	}
	std::cout << std::endl;	
}
/*
template<typename T> void print_vector(std::vector<std::vector<T>*>* v)
{
	std::cout << "vector of vectors: ";
	for (unsigned int i=0; i < (*v).size(); i++)
	{
		std::cout << "\n |--";
		print_vector( (*v)[i] );
	}
	std::cout << std::endl;	
}
*/

/**
 see David Elliot
 **/
float sigmoid_efficient(float x)
{
	return x / (1.0f + fabs(x));
}

float dsigmoid(float sx)
{
	return sx*(1.0f-sx);
}

float sigmoid(float x)
{
	return 1.0f/(1.0f+exp(-x));
}

float linear(float x)
{
	return x;
}

float dlinear(float x)
{
	return 1.0f;
}



float tanh_efficient(float x)
{
	if (x > 1.92033f) return 0.96016f;
	if (x <= -1.92033f) return -0.96016f;
	if (0.0f < x) { return 0.96016f - 0.26037f * (x - 1.92033f)*(x - 1.92033f); }
	else { return 0.26037f * (x + 1.92033f)*(x + 1.92033f) - 0.96016f; }
	
}

float dtanh(float tanhx)
{
	return (1.0f-tanhx*tanhx);
}

double sigmoid_efficient(double x)
{
	return x / (1.0 + abs(x));
}

double dsigmoid(double sx)
{
	return sx*(1.0-sx);
}

double sigmoid(double x)
{
	return 1.0/(1.0+exp(-x));
}

double linear(double x)
{
	return x;
}

double dlinear(double x)
{
	return 1.0;
}



double tanh_efficient(double x)
{
	if (x > 1.92033) return 0.96016;
	if (x <= -1.92033) return -0.96016;
	if (0 < x) { return 0.96016 - 0.26037 * (x - 1.92033)*(x - 1.92033); }
	else { return 0.26037 * (x + 1.92033)*(x + 1.92033) - 0.96016; }
	
}

double dtanh(double tanhx)
{
	return (1.0-tanhx*tanhx);
}

/*
void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
*/

void split(string& text, string& separators, vector<string>& words)
{
	int n = text.length();
	int start, stop;

	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n)) 
	{
		stop = text.find_first_of(separators, start);
		if ((stop < 0) || (stop > n)) stop = n;
		words.push_back(text.substr(start, stop - start));
		start = text.find_first_not_of(separators, stop+1);
	}
}


void
stripLeadingAndTrailingBlanks(string& StringToModify)
{
   if(StringToModify.empty()) return;

   int startIndex = StringToModify.find_first_not_of(" ");
   int endIndex = StringToModify.find_last_not_of(" ");
   string tempString = StringToModify;
   StringToModify.erase();

   StringToModify = tempString.substr(startIndex, (endIndex-startIndex+ 1) );
}

double ln(double x) {
	if (x < 0) return -log(x);
	return log(x);	
}

double dln(double x) {
	return 1.0/x;	
}

float ln(float x) {
	if (x < 0) return -log(x);
	return log(x);	
}

float dln(float x) {
	return 1.0f/x;	
}

void error(string message)
{
	cout << "\033[0;31mERROR: " << message << "\033[0m" << endl;
	//exit(0);	
}

void line()
{
	cout <<"\033[0;31m----------------------------------\033[0m" << endl;
}

void warning(string warning)
{
	cout << "\033[1;31mWarning: " << warning << "\033[0m"<< endl;
}

void status(string message)
{
	cout << "\033[42mStatus: " << message <<  "\033[0m" << endl;
}


void message(string message)
{
	cout << "\033[42mMessage: " << message <<  "\033[0m" << endl;
}

double cov(vector<double>* x, vector<double>* y)
{
	double temp = 0;
	assert( x->size()==y->size() );

	double xmu = mean(x);
	double ymu = mean(y);

	for (unsigned int i=0; i < x->size(); i++)
	{
		temp += ((*x)[i]-xmu) * ((*y)[i]-ymu);
	}

	return temp/( var(x)*var(y) );

}

double min(vector<vector<weight_t>*>* v)
{
	double m = (*(*v)[0])[0];
	for (unsigned int i=0; i < v->size(); i++)
		for (unsigned int j=0; j < (*v)[i]->size();j++)
			m = min(m, (*(*v)[i])[j]);
	return m;
}

double max(vector<vector<weight_t>*>* v)
{
	double m = (*(*v)[0])[0];
	for (unsigned int i=0; i < v->size(); i++)
		for (unsigned int j=0; j < (*v)[i]->size();j++)
			m = max(m, (*(*v)[i])[j]);
	return m;
}

//TODO efficient
/*float my_sin(float x) { return sin( 3.14159265f*x);}
float my_cos(float x) { return cos( 3.14159265f*x);}
double my_sin(double x) { return sin(M_PI*x);}
double my_cos(double x) { return cos(M_PI*x);}
*/

double corr(vector<double>* x, vector<double>* y)
{
/*	if (x->size() != y->size()) {
		stringstream eg;
		//errormsg << "Correlation Error! Vector sizes differ" << x->size() << "!=" << y<-size();
		error( errormsg.str() );
	}
*/
	cout << x->size() << "==" << y->size() << endl;
	assert(x->size() == y->size() );

	double result = 0.0;

	double xmu = mean(x);
	double ymu = mean(y);

	unsigned int n = x->size();

	for (unsigned int i=0; i < x->size(); i++)
	{
		result += ( (*x)[i]-xmu )* ( (*y)[i]-ymu );
	}

	result /= ( (n-1)*sqrt(var(x))*sqrt(var(y)) );

	return result;
}

vector<vector<weight_t>*>* transpose(vector<vector<weight_t>*>* v)
{
	vector<vector<weight_t>*>* result = new vector<vector<weight_t>*>();
	for (unsigned int j=0; j < (*v)[0]->size(); j++)
	{
		vector<weight_t>* temp = new vector<weight_t>();
		for (unsigned int i=0; i < v->size(); i++)
		{
			temp->push_back( (*(*v)[i])[j] );
		}
		result->push_back( temp );
	}
	return result;

}

unsigned int randomize_seed() {
	time_t seed = (unsigned)time(NULL);

	//std::cout << "seed: "<< seed << endl;

	srand( seed );

		return seed;

}

unsigned int randomize_seed(unsigned int seed ) {

	srand( seed );
	return seed;

}


