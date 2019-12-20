#include "Sequence.h"
#include "functions.h"
//#include <strstream>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>


using namespace std;

Sequence::Sequence(weight_t inputs[], weight_t targets[], unsigned int num_samples)
{
	for (unsigned int i=0; i < num_samples; i++)
	{
		vector<weight_t>* input = new vector<weight_t>();
		vector<weight_t>* target = new vector<weight_t>();
		input->push_back( inputs[i] );
		target->push_back( targets[i] );
		add(input, target);
	}
}

Sequence::Sequence()
{
  
};

Sequence::~Sequence() {
  
  cout << "Calling destructor of Sequence '" << name << "'"<< endl;
  
	training_begin=0;
	training_end=0;
	validation_begin=0;
	validation_end=0;
	test_begin=0;
	test_end=0;
	for (unsigned int i=0; i < size(); i++)
	{
		delete input[i];
		delete target[i];
	}
};

void read_line_as_vector(ifstream* ifs, vector<weight_t>* v)
{
	string s;
 	getline( *ifs, s );
  	istringstream iss( s );
  	if ((s.substr(0,1)=="\n") || (s.substr(0,1)=="!")) {
  		//cout<<"Skipping:" << s << endl;
  		return;
  	}
	copy( istream_iterator<weight_t>( iss ), istream_iterator<weight_t>(), back_inserter( *v ) );
}

bool Sequence::load_from_file(string filename)
{
	//istrstream ostr1(buffer1, 2048);
	int count=0;
	
	ifstream ifs(filename.c_str());
	
	while (!ifs.eof()) {
		vector<weight_t>* v = new vector<weight_t>();
		if (v == NULL) continue;
		read_line_as_vector(&ifs, v);

		vector<weight_t>* v2 = new vector<weight_t>();
		for (unsigned int i=0; i < v2->size(); i++)
			v2->push_back( (*v)[i] );

		this->add(v,v2);
		count++;
	}
	
	ifs.close();
	
	std::cout << "read " << count << " lines from file.\n";
	
	assert( target.size() == input.size() );
	
	return true;
}

unsigned int Sequence::get_input_size() { 
  if (input.size() > 0) { 
    return input[0]->size(); 
  } else {
    cout << "Error in computing input size of Sequence";
    return (0);
  }	  
}

unsigned int Sequence::get_target_size() { 
  if(target.size() > 0) 
    return target[0]->size(); 
  else {
    cout << "Error in computing target size of Sequence";
    return (0);    
  }
}


void Sequence::load_raw_sequence_from_file(string filename)
{
	//istrstream ostr1(buffer1, 2048);
	int count=0;

	ifstream ifs(filename.c_str());

	vector<weight_t>* last = NULL;

	while (!ifs.eof()) {
		vector<weight_t>* v = new vector<weight_t>();
		read_line_as_vector(&ifs, v);
		if (v->size() == 0) continue;
		if ((last != NULL) && (v->size()!=0)) {
			/*cout << "ADD" << endl;
			print_vector(last);
			print_vector(v);*/
			this->add(last,v);
			count++;
		}
		//print_vector(v);
		//last = ;
		if (last != NULL) delete last;
		last = new vector<weight_t>();
		for (unsigned int i=0; i < v->size(); i++)
			last->push_back( (*v)[i] );

	}

	ifs.close();

	std::cout << "read " << count << " lines from file.\n";

//	assert( target.size() == input.size() );

	//return true;
}

void Sequence::split_in_training_test_set(double fraction)
{
	training_begin = 0;
	training_end = size()*fraction;
	test_begin = training_end;
	test_end = size();
}

void Sequence::split_in_training_validation_test_set(double fraction_train, double fraction_val)
{
	training_begin = 0;
	training_end = round(size()*fraction_train);
	validation_begin = training_end;
	validation_end = validation_begin+round(size()*fraction_val);
	test_begin = validation_end;
	test_end = size();

/*	cout << "training size " << training_begin << ".." << training_end << " len="<< (training_end-training_begin) << endl;
	cout << "validation size " << validation_begin << ".." << validation_end << " len=" << (validation_end-validation_begin) << endl;
	cout << "test size " << test_begin << ".." << test_end << " len=" <<(test_end-test_begin) << endl;*/
}

std::vector<std::vector<weight_t>*>* Sequence::get_targets_between(unsigned int start, unsigned int end)
{
	std::vector<std::vector<weight_t>*>* temp = new std::vector<std::vector<weight_t>*>();
	temp->reserve(end-start);

	for (unsigned int i=start; i < end; i++)
	{
		temp->push_back ( this->get_target(i) );
	}

	return temp;

}

bool Sequence::sanity_check() 
{
  bool sane = true;
  if (target.size()!=input.size()){ 
    sane = false;
    cout << "Target and input size do not match! Input size "<< input.size() << " Target size: " << target.size() << endl;
  }
    
    
  return(sane);
}

weight_t Sequence::get_target_at(unsigned int index, unsigned int pos) {
  vector<weight_t>* seq = this->get_target(index);
  if (seq == NULL) return(NAN);
  return( (*seq)[pos] );
}

weight_t Sequence::get_input_at(unsigned int index, unsigned int pos) {
  vector<weight_t>* seq = this->get_input(index);
  if (seq == NULL) return(NAN);
  return( (*seq)[pos] );
}

std::vector<weight_t>* Sequence::get_input(unsigned int index) 
{ 
  if(index < input.size() && index >= 0) 
    return input[index]; 
  else
    return NULL;
}

std::vector<weight_t>* Sequence::get_target(unsigned int index) {
	if(index < target.size() && index >= 0)
	  return target[index];
	else
	  return NULL;
	
}

void Sequence::append(Sequence* sequence)
{
	assert( this->get_input_size() == sequence->get_input_size());
	assert( this->get_target_size() == sequence->get_target_size());

	for (unsigned int t=0; t < sequence->size(); t++)
	{
		this->input.push_back ( sequence->input[t] );
		this->target.push_back ( sequence->target[t] );
	}
}

unsigned int Sequence::get_maximum_index(unsigned int time)
{
	assert(time < target.size());
	unsigned int max_index=0;
	for (unsigned int i=1; i < target[time]->size(); i++)
	{
		if ( (*target[time])[i] >  (*target[time])[max_index] )
		{
			max_index = i;
		}
	}

	return max_index;
}

void Sequence::add_from_array(double input[], double target[], unsigned int input_size, unsigned int target_size) {
  
  std::vector<weight_t>* inp  = new std::vector<weight_t>();
  std::vector<weight_t>* tgt = new std::vector<weight_t>();
  
  
  for (int i=0; i< input_size; i++) {
    inp->push_back( input[i]);
 
  }
  for (int i=0; i < target_size; i++) {
    tgt->push_back( target[i]);
  }
  
  this->add(inp, tgt);
  
}

void Sequence::add(std::vector<weight_t>* input, std::vector<weight_t>* target)
{
	this->input.push_back( input );
	this->target.push_back( target );
	this->weight.push_back( 1.0 );

	training_begin = 0;
	training_end = this->input.size();
	validation_begin = training_end;
	validation_end = training_end;
	test_begin = training_end;
	test_end = training_end;

}

double Sequence::get_weight(unsigned int index) {
	if (index < input.size() && index >= 0)
  	return weight[index];
	else
	  return std::numeric_limits<double>::quiet_NaN();
}

unsigned int Sequence::size() { 
  
  if (target.size()==input.size()){ 
    return target.size();
  } else {
    cout << "Error in computing sequence size: Target ("<< target.size() <<") and Input Size ("<< input.size()<<") do not match!" << endl;
    return(0);
  }
  
  }

Sequence* Sequence::embed(unsigned int n)
{
	Sequence* sequence = new Sequence();

	//cout << "LOS" << this->size() << "-"<<n << endl;

	unsigned int input_size = n*this->get_input_size();

	for (unsigned int pos=0; pos < this->size()-n+1; pos++)
	{
		//cout << "i="<<i<<endl;
		//cout <<"pos: " <<pos<<endl;
		vector<weight_t>* target = this->get_target(pos+n-1);
		vector<weight_t>* input = new vector<weight_t>( input_size );
		for (unsigned int window=0; window < n; window++) {
			for (unsigned int elem=0; elem < this->get_input_size(); elem++)
			{
				//cout << i <<","<<j<<","<<k<<endl;
				//cout << (j*n+k) << "/" << input_size << endl;
				//cout << "input" << (pos+window) << endl;
				//cout << "j*n="<<(j*n) << " " << "  k:" <<k << endl;
				(*input)[window*this->get_input_size()+elem] = (*this->input[pos+window])[elem];
				//(*input)[i*n] = (*this->input[i+j])[k];
						//(*(*this->input)[i])[j*n+k];
			}
		}
		sequence->add(input, target);
	}

	return sequence;
}
