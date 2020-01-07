/*
 * AutoPredictor.h
 *
 *  Created on: Dec 8, 2009
 *      Author: brandmaier
 */

#ifndef AUTOPREDICTOR_H_
#define AUTOPREDICTOR_H_

#include "../Network.h"
#include "../Sequence.h"
#include <vector>


/**
 * TransferFunction specifies the way prediction from the output layer
 * are fed back to the input layer.
 */
struct TransferFunction
{


	virtual void transfer(vector<weight_t>* in, vector<weight_t>* out) = 0;
	virtual void transfer(vector<weight_t>* in) = 0;
	
	virtual ~TransferFunction() {};
};

struct MappedTransferFunction : TransferFunction
{
	int* mapping;

	MappedTransferFunction() {}

	MappedTransferFunction(	int* mapping )
	{
		this->mapping = mapping;
	}

	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		for (unsigned int i = 0; i < out->size(); i++)
		{
			if (mapping[i] != -1) {
				assert(mapping[i] < (int)in->size());
				(*in)[ mapping[i] ] =  (*out)[ i ] ;
			}

		}
	}

	 void transfer(vector<weight_t>* in)
	 {
		 //NOOP
	 }

	void set_mapping(int* mapping)
	{
		this->mapping = mapping;
	}

};

struct EmbededTransferFunction : TransferFunction
{
	unsigned int embedding_size;
	vector<weight_t>* last;

	EmbededTransferFunction(unsigned int embedding_size)
	{
		this->embedding_size = embedding_size;
	}

	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		unsigned int block_size = in->size()/embedding_size;

		//cout << "transfer " << block_size << endl;

		// left shift data blocks
		for (unsigned int block=1; block < embedding_size; block++)
		{
			for (unsigned int i=0; i < block_size; i++)
			{
				unsigned int to_offset = (block-1)*block_size + i;
				unsigned int from_offset = block*block_size + i;
			//	cout << block << ":" << from_offset << "," << to_offset << "," << in->size() << endl;
				(*in)[to_offset] = (*last)[from_offset];
			}
		}

		//map new data
		for (unsigned int i=0; i < block_size;i++)
		{
			unsigned int to_offset = (embedding_size-1)*block_size+i;
			//cout << "map" << i << "," << to_offset << endl;
			(*in)[to_offset] = (*out)[i];
		}

		last = in;

//		print_vector(in);

	}

	 void transfer(vector<weight_t>* in)
	 {
		 last = in;
	 }
};

struct ExponentiallyEmbeddedTransferFunction : TransferFunction
{
	vector<vector<weight_t>* > history;
	unsigned int history_capacity;
	unsigned int embedding_size;

	ExponentiallyEmbeddedTransferFunction(unsigned int size)
	{
		this->embedding_size = size;
		this->history_capacity = 2 << (size-1);
	}

	//     history: -4 -3 -2 -1 0

	//				#0	#1	#2 | #3
	//	exp			 2   1   0 | --
	//				-4	-2	-1 | 0
	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{

		unsigned int block_size = in->size()/embedding_size;

		// left shift data blocks
		for (unsigned int block=0; block < embedding_size-1; block++)
		{
			for (unsigned int i=0; i < block_size; i++)
			{
				unsigned int history_offset = history_capacity - (2 << i);

				unsigned int to_offset = (block)*block_size + i;
				//unsigned int from_offset = block*block_size+i;

				cout << history_offset << " " << to_offset << " " << " "  << " " << i;

				(*in)[to_offset] = (*history[history_offset])[i];
			}
		}

		//map new data
		for (unsigned int i=0; i < block_size;i++)
		{
			unsigned int to_offset = (embedding_size-1)*block_size+i;
			//cout << "map" << i << "," << to_offset << endl;
			(*in)[to_offset] = (*out)[i];
		}

		add_to_history(in);

		// TODO !!
	}

	 void transfer(vector<weight_t>* in)
	 {
		add_to_history(in);
	 }

	 void add_to_history(vector<weight_t>* in)
	 {
		 history.push_back(in);
		 if (history.size() > history_capacity) {
			history.erase(history.end());
		 }
	 }
};

struct ConstTransferFunction : TransferFunction
{
	vector<weight_t>* in;

	ConstTransferFunction(vector<weight_t>* in)
	{
		this->in = in;
	}

	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		for (unsigned int i=0; i < this->in->size(); i++)
		{
			(*in)[i] = (*this->in)[i];
		}
	}

	 void transfer(vector<weight_t>* in)
	 {
		 // NOOP
	 }
};

struct LinearTransferFunction : TransferFunction
{
	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		assert(in->size() == out->size());

		for (unsigned int i=0; i < in->size();i++)
		{
			(*in)[i] = (*out)[i];
		}
	}

	 void transfer(vector<weight_t>* in)
	 {
		 // NOOP
	 }
};

struct WinnerTakesAllTransferFunction : TransferFunction
{
  void transfer(vector<weight_t>* in, vector<weight_t>* out)
  {
    assert(in->size() == out->size());
    
    unsigned int max_index = 0;
    for (unsigned int i=0; i < in->size();i++)
    {
      if ((*out)[i] > (*out)[max_index]) max_index=i;
    }
    
    for (unsigned int i=0; i < in->size();i++)
    {
      if (i==max_index)
        (*in)[i] = 1;
      else
        (*in)[i] = 0;
    }
  }
  
  void transfer(vector<weight_t>* in)
  {
    // NOOP
  }
};

struct ProbabilisticWinnerTakesAllTransferFunction : TransferFunction
{
  void transfer(vector<weight_t>* in, vector<weight_t>* out)
  {
    assert(in->size() == out->size());
    
    double r = ((double) rand() / (RAND_MAX));
    double sum = 0.0;
    
    // initialize inputs with 0
    for (unsigned int i=0; i < in->size();i++)
    {
      (*in)[i] = 0;
    }
    
    // find the peak activation
    bool done = false;
    for (unsigned int i=0; i < in->size();i++)
    {
      sum += (*out)[i];
      if (r <= sum) { (*in)[i]=1; i=in->size(); done=true; }
    }
    if (!done) (*out)[in->size()-1] = 1; // catch numeric integration problems
    
  }
  
  void transfer(vector<weight_t>* in)
  {
    // NOOP
  }
};

struct LinearAddedNoiseTransferFunction : TransferFunction
{
  
  double noise_strength;
  
  LinearAddedNoiseTransferFunction(double noise_strength)
  {
    this->noise_strength = noise_strength;
  }
  
  void transfer(vector<weight_t>* in, vector<weight_t>* out)
  {
    assert(in->size() == out->size());
    
    for (unsigned int i=0; i < in->size();i++)
    {
      (*in)[i] = (*out)[i] + rand()/(RAND_MAX+1.0)*noise_strength;
    }
  }
  
  void transfer(vector<weight_t>* in)
  {
    // NOOP
  }
};

struct LinearDampenedTransferFunction : TransferFunction
{
  
  double dampening_strength;
  
  LinearDampenedTransferFunction(double dampening_strength)
  {
  //dampening_strength = max(0, dampening_strength);
    //dampening_strength = min(1, dampening_strength);
    
    this->dampening_strength = dampening_strength;
  }
  
  void transfer(vector<weight_t>* in, vector<weight_t>* out)
  {
    assert(in->size() == out->size());
    
    for (unsigned int i=0; i < in->size();i++)
    {
      (*in)[i] = (*out)[i] * dampening_strength;
    }
  }
  
  void transfer(vector<weight_t>* in)
  {
    // NOOP
  }
};

struct DummyTransferFunction : TransferFunction
{
  //~DummyTransferFunction() {}
  
	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		// NOOP
	}

	 void transfer(vector<weight_t>* in)
	 {
		 // NOOP
	 }
};

struct MappedEmbeddedTransferFunction : TransferFunction
{
	unsigned int embedding_size;
	vector<weight_t>* last;
	vector<int> mapping;

	MappedEmbeddedTransferFunction(unsigned int embedding_size, vector<int>* mapping)
	{
		this->embedding_size = embedding_size;
		this->mapping = *mapping;
	}
	
//	~MappedEmbeddedTransferFunction() {}

	void transfer(vector<weight_t>* in, vector<weight_t>* out)
	{
		if (!(in->size()%embedding_size == 0))
		{
			stringstream err;
			err << "Error in MappedEmbeddedTransferFunction! In size is "<< in->size() <<
					" and is not dividible by embedding size: " << embedding_size << "!" << "Modulus = " << in->size()%embedding_size <<".";
			error(err.str());
		}

		assert(in->size()%embedding_size == 0);
		assert(out->size()==mapping.size());

		unsigned int block_size = in->size()/embedding_size;

//		cout << "transfer " << block_size << "," << embedding_size << "," << in->size() << endl;

		// left shift data blocks
		for (unsigned int block=1; block < embedding_size; block++)
		{
			for (unsigned int i=0; i < block_size; i++)
			{
				unsigned int to_offset = (block-1)*block_size + i;
				unsigned int from_offset = block*block_size + i;
				//cout << block << ":" << from_offset << "," << to_offset << "," << in->size() << endl;
				(*in)[to_offset] = (*last)[from_offset];
			}
		}

//		cout << "proc" << endl;

		//map new data
		for (unsigned int i=0; i < out->size();i++)
		{
//			cout << i << "->" << mapping[i] << endl;
			unsigned int to_offset = (embedding_size-1)*block_size;
			if (mapping[i] != -1) {
				if (mapping[i] >= (int)block_size) {
					cout << "mapping points to " << mapping[i] << " but block size = " << block_size << endl;
				}
				assert(mapping[i] < (int)block_size);
				//(*in)[ mapping[i] ] =  (*out)[ i ] ;
				to_offset += mapping[i];
			}		else {
				to_offset += i;
			}
			(*in)[to_offset] = (*out)[i];
		}

		last = in;

	//	print_vector(in);

	}

	 void transfer(vector<weight_t>* in)
	 {
		 last = in;
	 }
};

struct AutoPredictor
{
	Network* network;
	Sequence* sequence;
	TransferFunction* transfer_function;
	//int* mapping;
	//bool has_mapping;

	vector<vector<weight_t>*>* prediction;
	Sequence* generated_sequence;

	AutoPredictor(Network* network);
	AutoPredictor(Network* network, int* mapping);
	AutoPredictor(Network* network, TransferFunction* transfer_function);

	~AutoPredictor();

	vector<vector<weight_t>*>* predict(Sequence* sequence,unsigned int timesteps,	unsigned int context_length);
	double predict_and_plot(Sequence* sequence, unsigned int timesteps,	unsigned int context_length);


};

#endif /* AUTOPREDICTOR_H_ */
