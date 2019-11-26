#include "SequenceSet.h"
#include <limits>
#include <cmath>
#include <map>

string SequenceSet::UNDEF_CHARACTER("?");

SequenceSet::SequenceSet(const char* filename)
{
	stringstream filenamestream;
	filenamestream << filename;
	init();
	this->load_from_file(filenamestream.str());
}

SequenceSet::SequenceSet(const std::string &filename)
{
	init();
	this->load_from_file(filename);	
}

SequenceSet::SequenceSet()
{
	init();
}

SequenceSet::SequenceSet(Sequence* sequence)
{
	init();
	add_sequence(sequence);
}

SequenceSet::SequenceSet(SequenceSet &sequence_set)
{
	/*this->set =
			new std::vector<Sequence*>();
	for (unsigned int i=0;i < sequence_set.set.size(); i++)
	{
		set.push_back( sequence_set.set[i] );
	}*/
	this->set = sequence_set.set;

	name = sequence_set.name;

	input_is_normalized = sequence_set.input_is_normalized;
	target_is_normalized = sequence_set.target_is_normalized;

	if (sequence_set.target_means != NULL) {
		target_means = new std::vector<weight_t>();
		copy( sequence_set.target_means->begin(),sequence_set.target_means->end(),target_means->begin() );
	}
	if (sequence_set.target_stds!= NULL) {
		target_stds = new std::vector<weight_t>();
		copy( sequence_set.target_stds->begin(),sequence_set.target_stds->end(),target_stds->begin() );

	}
	if (sequence_set.input_means != NULL) {
		input_means = new std::vector<weight_t>();
		copy( sequence_set.input_means->begin(),sequence_set.input_means->end(),input_means->begin() );

	}
	if (sequence_set.input_stds != NULL) {
		input_stds = new std::vector<weight_t>();
		copy( sequence_set.input_stds->begin(),sequence_set.input_stds->end(),input_stds->begin() );

	}

	//TODO/BUG fill this vectors

	target_min = sequence_set.target_min;
	target_max = sequence_set.target_max;
	input_min =sequence_set.input_min;
	input_max = sequence_set.input_max;
	original_target_min =sequence_set.original_target_min;
	original_target_max = sequence_set.original_target_max;
	original_input_min = sequence_set.original_input_min;
	original_input_max = sequence_set.original_input_max;

	target_size = sequence_set.target_size;
	input_size = sequence_set.input_size;
	element_count = sequence_set.element_count;

}

void SequenceSet::init()
{
	this->name = "Sequence Set";

	this->input_size = 0;
	this->target_size = 0;
	this->element_count = 0;
	this->target_is_normalized = false;
	this->input_is_normalized = false;

	this->original_input_min = NAN;
	this->original_input_max = NAN;
	this->original_target_min = NAN;
	this->original_target_max = NAN;

	target_means = NULL;
	target_stds = NULL;
	input_means = NULL;
	input_stds = NULL;

}

unsigned int SequenceSet::size()
{

	return this->set.size();
}

SequenceSet::~SequenceSet()
{
//	cout << "norm " <<  << endl;
	if (this->input_is_normalized == true)
	{
		delete input_means;
		delete input_stds;
	}
	if (this->target_is_normalized == true)
	{
		delete target_means;
		delete target_stds;
	}


	for (unsigned int i=0; i < set.size(); i++)
	{
		delete set[i];
	}

}

void SequenceSet::find_minmax()
{
	input_min = std::numeric_limits<double>::max();
	input_max = std::numeric_limits<double>::min();
	target_min = std::numeric_limits<double>::max();
	target_max = std::numeric_limits<double>::min();
	// find minimum and maximum input
	for (unsigned int i=0; i < this->size(); i++)
	{
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			for (unsigned int k=0; k < input_size; k++)
			{
		 	 input_min = min(input_min, (*set[i]->get_input(j))[k]);
			 input_max = max(input_max, (*set[i]->get_input(j))[k]);
			}
		}
	}
	// find minimum and maximum output
	for (unsigned int i=0; i < this->size(); i++)
	{
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			for (unsigned int k=0; k < target_size; k++)
			{
		 	 target_min = min(target_min, (*set[i]->get_target(j))[k]);
			 target_max = max(target_max, (*set[i]->get_target(j))[k]);
			}
		}
	}

	//if isnan(input_min) or isnan(target_min) or isnan()

	/*cout << "Minmax"<<endl;
	cout << "input: " << input_min << " : " << input_max << endl;
	cout << "output: " << target_min << " : " << target_max << endl;
*/
}

void SequenceSet::scale_to_original()
{
	double total_min = min(input_min, target_min);
	double total_max = max(input_max, target_max);

	scale_to_minmax( total_min, total_max );

}

void SequenceSet::scale_input_to_minmax(double new_min, double new_max)
{



	if (abs(input_min-input_max) > 10e-8)
	{
	for (unsigned int i=0; i < this->size(); i++)
	{
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			for (unsigned int k=0; k < input_size; k++)
			{
				 (*set[i]->get_input(j))[k] =  ((*set[i]->get_input(j))[k] - input_min) / (input_max-input_min)
								 *(new_max-new_min) + new_min;
			}
		}
	}
	} else {
		cout << "skipping input scaling" << endl;
	}

	find_minmax();

}

void SequenceSet::scale_to_minmax(double new_min, double new_max)
{

	/*double total_min = min(input_min, target_min);
	double total_max = max(input_max, target_max);
	*/

	cout << input_min << " vs " << input_max << endl;
	cout << target_min << " vs " << target_max << endl;

	original_input_min = input_min;
	original_input_max = input_max;
	original_target_min = target_min;
	original_target_max = target_max;

	if (abs(input_min-input_max) > 10e-8)
	{
	for (unsigned int i=0; i < this->size(); i++)
	{
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			for (unsigned int k=0; k < input_size; k++)
			{
				//cout << "convert:"<< (*set[i]->get_input(j))[k] ;// << "->" <<((*set[i]->get_input(j))[k] - input_min) / (input_max-input_min) << endl;
				 (*set[i]->get_input(j))[k] =  ((*set[i]->get_input(j))[k] - input_min) / (input_max-input_min)
							 *(new_max-new_min) + new_min;
				 //cout << " to:"<< (*set[i]->get_input(j))[k] << endl;
			}
		}
	}
	} else {
		cout << "skipping input scaling" << endl;
	}

	if (abs(target_min - target_max)>10e-8) {
	for (unsigned int i=0; i < this->size(); i++)
	{
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			for (unsigned int k=0; k < target_size; k++)
			{
				 (*set[i]->get_target(j))[k] =  ((*set[i]->get_target(j))[k] - target_min) / (target_max-target_min)
								 * (new_max-new_min) + new_min ;
			}
		}
	}
	} else {
		cout << "skipping output scaling" <<  (abs(target_min - target_max)) << endl;
	}

	find_minmax();
}

void SequenceSet::scale(double factor)
{

	if (factor==1) return;

	for (unsigned int i=0; i < this->size(); i++)
	{
	//cout << "Sequence #" <<i << " length: "<< this->set[i]->size() <<endl;
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{	
			//cout << " t=" << j << " " << set[i]->get_input(j)->size() << " " << (*set[i]->get_target(j)).size() << endl;
			for (unsigned int k=0; k < input_size; k++) 
				(*set[i]->get_input(j))[k] = ( (*set[i]->get_input(j))[k] * factor );
			for (unsigned int k=0; k < target_size; k++) 
				(*set[i]->get_target(j))[k] = ( (*set[i]->get_target(j))[k] * factor );
		
		}
	}		

	find_minmax();
}

	void SequenceSet::normalize(SequenceSet* other_set)
	{
		assert(other_set->input_is_normalized == true or other_set->target_is_normalized);
		
		this->apply_normalization(other_set->input_means, other_set->input_stds, 
			other_set->target_means, other_set->target_stds);	
	}

	DataSet* SequenceSet::load_from_compml_file(string directory)
	{
		DataSet* dataset = new DataSet();



		// check if file exists
		struct stat stFileInfo;
		int status = stat((directory+"/info").c_str(),&stFileInfo);

		if (status != 0) {
			error("Directory does not exist or no info file existing!");
		}

		string line;

		unsigned int num_features, num_labels;

		// load info
		ifstream ifs((directory+"/info").c_str());
		while(!ifs.eof())
		{
			getline(ifs, line);

	      	vector<string> tokens;
	      	string sep = std::string("\t");
	        split(line,sep,tokens);

	        if (tokens.size()==0) continue;

			cout << "info: " << tokens[0] << endl;
			if (tokens[0]=="numFeatures") {
				num_features = atoi(tokens[1].c_str() );
			}
			if (tokens[0]=="numLabels") {
				num_labels = atoi(tokens[1].c_str() );
			}

			if (tokens[0]=="format") {
				if (tokens[1] != "MulticlassClassification") {
					error("Only MulticlassClassification supported at the moment!");
				}
			}
		}
		ifs.close();

	//	cout << "*" << endl;

		// load label-values

		ifstream ifs2((directory+"/label-values").c_str());
		unsigned int label_count = 0;
		map<string, unsigned int> label_map;
		while(!ifs2.eof())
		{
			getline(ifs2, line);

			if ((line == "")  or (line =="\n")) break;

			//cout << "label:" << line << "." << endl;
			label_map[line] = label_count;
			label_count++;
		}
		ifs2.close();

		//cout << "*" << endl;

		// load training data

		SequenceSet* val = new SequenceSet();
		dataset->training_set = _load_compml_sequence(directory+"/train", num_labels, num_features, label_map);
		dataset->test_set = _load_compml_sequence(directory+"/test", num_labels, num_features, label_map);
		dataset->validation_set = new SequenceSet();

		return dataset;
	}

	SequenceSet* SequenceSet::_load_compml_sequence(string filename, unsigned int num_labels, unsigned num_features, map<string, unsigned int> label_map)
	{
		SequenceSet* set = new SequenceSet();

		string line;

		ifstream ifs3(filename.c_str());
		while(!ifs3.eof())
		{
			getline(ifs3, line);

			if ((line == "")  or (line =="\n")) break;

	      	vector<string> tokens;
	      	string sep = std::string(" ");
	        split(line,sep,tokens);
	        //cout << line << endl;
	        //cout << "class: " << tokens[0] << endl;
	       // vector<vector<weight_t>* >* input = new vector<vector<weight_t>* >();
	       // vector<vector<weight_t>* >* target = new vector<vector<weight_t>* >();

	        if (tokens.size() < 2) {
	        	warning("Misshaped line in file: "+line);
	        	break;
	        }

	        // target coding

	        vector<weight_t>* target = new vector<weight_t>(num_labels);
	        for (unsigned int i=0; i < num_labels; i++) (*target)[i] =0.0;
	        //cout << label_map[tokens[0]] << " was " << tokens[0]<<endl;
	        (*target)[label_map[tokens[0]] ] = 1;

	        // input coding
	        vector<weight_t>* input = new vector<weight_t>(num_features);

	        for (unsigned int i = 1; i < tokens.size(); i++) {
		      	vector<string> tokens2;
		      	string sep = std::string(":");
		        split(tokens[i],sep,tokens2);
	        	//cout << "feature: " << tokens2[0] << ":" << tokens2[1] << endl;
		        unsigned int f_id = atoi(tokens2[0].c_str())-1;
		        double f_value = atof(tokens2[1].c_str());

		        if (f_id >= num_features) {
		        	error("Error in file! Tried to set non-existant feature!");
		        }

		        (*input)[f_id] = f_value;
	        }

	       // print_vector(input);
	       // print_vector(target);

	        Sequence* sequence = new Sequence();
	        sequence->add(input, target);
	        set->add_sequence(sequence);
		}
		ifs3.close();

	//	cout << "*"<<endl;

		return set;
	}

	DataSet* SequenceSet::load_from_proben_file(string filename)
	{
		std::ifstream ifs(filename.c_str());
		string line;

		if (!ifs)
		{
			error("Sequence File not found!");
			return(NULL);
		}

		// read header

		getline(ifs, line);
		unsigned int bool_in = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int real_in = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int bool_out = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int real_out = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int training_examples = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int validation_examples = atoi( get_property_value(line).c_str() );
		getline(ifs, line);
		unsigned int test_examples = atoi( get_property_value(line).c_str() );

		unsigned int in = bool_in+real_in;
		unsigned int out = bool_out+real_out;
		unsigned int examples = training_examples+validation_examples+test_examples;

		cout << "in: "<< in <<" out:"<< out << " examples:"<< examples << endl;


		unsigned int linecount = 0;

		DataSet* dset = new DataSet();
		SequenceSet* sset = new SequenceSet();
		dset->training_set = sset;

		sset->target_size = out;
		sset->input_size = in;

		while (!ifs.eof()) {

			Sequence* cur_seq = new Sequence();

			getline(ifs, line);
			vector<weight_t>* target = new vector<weight_t>();
			vector<weight_t>* input = new vector<weight_t>();
	      	vector<string> tokens;
	      	string sep = std::string(" ");
	        split(line,sep,tokens);

	       // cout << "tokens: " << tokens.size() << " line:" << line << endl;

	        if (tokens.size() == 0) {
	        	continue;
	        }

	        if (tokens.size() != in+out) {
	        	warning("Could not read line in input file! Some data were skipped!");
	        	continue;
	        }

	        for (unsigned int i=0; i < in; i++) {
	        	input->push_back( atof(tokens[i].c_str()) );
	        }
	        for (unsigned int i=in; i < in+out; i++) {
	        	target->push_back( atof(tokens[i].c_str()) );
	        }


	        //cout << "..."<<endl;

			cur_seq->add(input, target);
			sset->add_sequence(cur_seq);

	        //cout << "..."<< endl;

			linecount++;

			if (linecount == training_examples) {
				sset = new SequenceSet();
				dset->validation_set = sset;
			}
			if (linecount == training_examples+validation_examples) {
				sset = new SequenceSet();
				dset->test_set = sset;
			}


		}

		if (linecount != examples)
		{
			error("mismatch between data and header!");
		}

		/*cout << dset->training_set->size() << " " << training_examples << endl;
		cout << dset->validation_set->size() << " " << validation_examples << endl;
		cout << dset->test_set->size() << " " << test_examples << endl;
		*/

		for (unsigned int i=0; i < dset->training_set->size(); i++)
		{
			dset->training_set->get(i)->training_begin=0;
			dset->training_set->get(i)->training_end=dset->training_set->get(i)->size();
			dset->training_set->get(i)->validation_begin=0;
			dset->training_set->get(i)->validation_end=0;
			dset->training_set->get(i)->test_begin=0;
			dset->training_set->get(i)->test_end=0;

		}
		for (unsigned int i=0; i < dset->validation_set->size(); i++)
		{
			dset->validation_set->get(i)->training_begin=0;
			dset->validation_set->get(i)->training_end=0;
			dset->validation_set->get(i)->validation_begin=0;
			dset->validation_set->get(i)->validation_end=dset->validation_set->get(i)->size();
			dset->validation_set->get(i)->test_begin=0;
			dset->validation_set->get(i)->test_end=0;
		}
		for (unsigned int i=0; i < dset->test_set->size(); i++)
		{
			dset->test_set->get(i)->training_begin=0;
			dset->test_set->get(i)->training_end=0;
			dset->test_set->get(i)->validation_begin=0;
			dset->test_set->get(i)->validation_end=0;
			dset->test_set->get(i)->test_begin=0;
			dset->test_set->get(i)->test_end=dset->test_set->get(i)->size();
		}

		assert( dset->training_set->size() == training_examples);
		assert( dset->validation_set->size() == validation_examples);
		assert( dset->test_set->size() == test_examples);



		return dset;
	}

	void SequenceSet::shuffle()	{
		std::random_shuffle( set.begin(), set.end() );
	}

	void SequenceSet::load_from_file(string filename)
	{
		unsigned int seq_count = 1;
		element_count = 0;
		
		ifstream ifs(filename.c_str());
		string line;
	
		if (!ifs)
		{
			//throw FILE_NOT_FOUND_EXCEPTION;
			stringstream errormsg;
			errormsg << "Could not find SequenceSet file '" << filename << "'"<<endl;
			error(errormsg.str());
			return;
		}
	
		Sequence* cur_seq = new Sequence();
	
		target_size = -1;
		input_size = -1;
	
		unsigned int linecount = 0;
	
		while (!ifs.eof()) {
			getline(ifs, line);
			linecount++;
						
			if ((line == "") || (line.substr(0,1)=="!")) continue;
						
			if (line.substr(0,1) == "#") {
				string seq_name = line.substr(1, line.size()-1);
				cur_seq->name = seq_name;
				set.push_back( cur_seq );
				cur_seq = new Sequence();
				seq_count++;
				continue;
			} 
			

			//cout << "read line " << "#" << element_count << " : "  << line << endl;
						
			// read input
			vector<weight_t>* input = new vector<weight_t>;
// 			istringstream iss( line );
//			copy( istream_iterator<weight_t>( iss ), istream_iterator<weight_t>(), back_inserter( *input ) );
 	      	vector<string> tokensi;
 	      	string sep = std::string("\t");
 	        split(line,sep,tokensi);
 	        for (unsigned int i=0; i < tokensi.size(); i++)
 	        {
 	        	if (tokensi[i].substr(0,1)==UNDEF_CHARACTER) {
 	        		input->push_back( 0 );
 	        	} else {
 	        		input->push_back( atof(tokensi[i].c_str()) );
 	        	}
 	        }
			
			// read target
			getline(ifs, line); linecount++;
			vector<weight_t>* target = new vector<weight_t>();
// 			istringstream iss2( line );
//			copy( istream_iterator<weight_t>( iss2 ), istream_iterator<weight_t>(), back_inserter( *target ) );
 	      	vector<string> tokens;
 	      	//string sep = std::string("\t");
 	        split(line,sep,tokens);
 	        for (unsigned int i=0; i < tokens.size(); i++)
 	        {
 	        	if (tokens[i].substr(0,1)==UNDEF_CHARACTER) {
 	        		target->push_back( NAN );
 	        	} else {
 	        		target->push_back( atof(tokens[i].c_str()) );
 	        	}
 	        }

			if ((target_size == -1) || (input_size == -1))
			{
				target_size = target->size();
				input_size = input->size();	
			}
			
			if ((target_size != target->size()) || (input_size != input->size()))
			{
				stringstream stream;
				stream << "bad line in Sequence file in line " << (linecount-1) << endl;
				error( stream.str() );
			}
			
			// add to sequence
			cur_seq->add(input, target);
			
			element_count++;
			
		}
		
		if (cur_seq->size() != 0)
			set.push_back( cur_seq );
		
		cout << "Loaded sequences #" << seq_count << " and elements #" << element_count << " in-size: "
		 << set[0]->get_input(0)->size() << " target-size: "<<set[0]->get_target(0)->size()<<endl;
	
		//target_size = set[0]->get_target(0)->size();
		//input_size = set[0]->get_input(0)->size();
	

				
		ifs.close();


		find_minmax();
	}

	void SequenceSet::add_sequence(Sequence* sequence)
	{
		set.push_back( sequence );
		element_count += sequence->size();




		if (input_size == 0)
		{
			input_size = sequence->get_input_size();
			target_size = sequence->get_target_size();
		} else {
			if (input_size != sequence->get_input_size()){
				error("Cannot add Sequence to SequenceSet. Input Size mismatch!");
			}
			if (target_size != sequence->get_target_size()){
				error("Cannot add Sequence to SequenceSet. Target Size mismatch!");
			}
		}

		find_minmax();	//TODO not efficient!

	}
	
	void SequenceSet::add_sequence_set(SequenceSet* sequence_set)
	{
		for (unsigned int i=0; i < sequence_set->size(); i++)
		{
			this->add_sequence(sequence_set->get(i));
		}

	}

	void SequenceSet::print_sequences()
	{
	 	 for (unsigned int i=0; i < size(); i++)
	 	 {
	 	 	for (unsigned int j=0; j < set[i]->size(); j++)
	 	 	{
	 	 		for (unsigned int k = 0; k < set[i]->get_input(j)->size(); k++)
	 	 		{
	 	 			cout << (*set[i]->get_input(j))[k] << "\t";
	 	 		}
	 	 		cout<< endl;
	 	 		for (unsigned int k = 0; k < set[i]->get_target(j)->size(); k++)
	 	 		{

	 	 			cout << (*set[i]->get_target(j))[k] << "\t";

	 	 		}
	 	 		cout << endl;
	 	 	}
	 	 	cout << "#" << endl;
	 	 }
	}

	void SequenceSet::save_to_file(string filename)
	{
	 ofstream myfile;
 	 myfile.open ( filename.c_str() );
 	 
 	 myfile << "!BNNlib (VERSION="<< __BNNLIB_VERSION <<")" << endl;
 	 time_t rawtime;
   	 time ( &rawtime ); 	 
  	 myfile << "!time: " << ctime(&rawtime)	;
  	 myfile << "!input-min:" << input_min << endl;
  	 myfile << "!input-max:" << input_max << endl;
  	 myfile << "!target-min:" << target_min << endl;
  	 myfile << "!target-max:" << target_max << endl;
  	 myfile << "!original-input-min:" << original_input_min << endl;
  	 myfile << "!original-input-max:" << original_input_max << endl;
  	 myfile << "!original-target-min:" << original_target_min << endl;
  	 myfile << "!original-target-max:" << original_target_max << endl;
  	 //for (unsigned int )
  	 if (input_means != NULL) {
  	 myfile << "!input-means:";
  	 for (unsigned int i=0; i < input_means->size(); i++)
  		 myfile << (*input_means)[i] << "\t";
  	 myfile << endl;
  	 myfile << "!input-stds:";
  	 for (unsigned int i=0; i < input_stds->size(); i++)
  		 myfile << (*input_stds)[i] << "\t";
  	 myfile << endl;
  	 myfile << "!target-means:";
  	 for (unsigned int i=0; i < target_means->size(); i++)
  		 myfile << (*target_means)[i] << "\t";
  	 myfile << endl;
  	 myfile << "!target-stds:";
  	 for (unsigned int i=0; i < target_stds->size(); i++)
  		 myfile << (*target_stds)[i] << "\t";
  	 myfile << endl;
  	 }

  	 myfile << "!input_size:" << input_size << endl;
  	 myfile << "!target_size:" << target_size << endl;

 	 for (unsigned int i=0; i < size(); i++)
 	 {
 	 	for (unsigned int j=0; j < set[i]->size(); j++)
 	 	{
 	 		for (unsigned int k = 0; k < set[i]->get_input(j)->size(); k++)
 	 		{
 	 			myfile << (*set[i]->get_input(j))[k] << "\t";
 	 		}
 	 		myfile << endl;
 	 		for (unsigned int k = 0; k < set[i]->get_target(j)->size(); k++)
 	 		{
 	 			if (isnan((*set[i]->get_target(j))[k])) {
 	 				myfile << UNDEF_CHARACTER << "\t";
 	 			}else{
 	 				myfile << (*set[i]->get_target(j))[k] << "\t";
 	 			}
 	 		}
 	 		myfile << endl; 	 		
 	 	}	 	 	
 	 	myfile << "#" << endl;
 	 }
 	 
 	 myfile.close();		
		
	}	
	
	void SequenceSet::normalize()
	{
		normalize(true, true);
	}

	void SequenceSet::normalize(bool input, bool target)
	{
		assert(set.size() > 0);
		assert(set[0]->size() > 0);

		// initialize vectors for data storage
		//unsigned int input_size = this->set[0]->get_input(0)->size();
		//unsigned int target_size = this->set[0]->get_input(0)->size();
		this->input_means = new vector<weight_t>( input_size );
		this->input_stds = new vector<weight_t>( input_size );
		this->target_means = new vector<weight_t>( target_size );
		this->target_stds = new vector<weight_t>( target_size );

	//	std::cout << input_size << "/" << target_size << endl;
		
		// calculate means
		unsigned int input_n=0;

		vector<unsigned int> target_n( target_size );
		for (unsigned int i=0; i < target_size; i++)
		{
			target_n[i]=0;
		}

		for (unsigned int i=0; i < this->size(); i++)
		{
			for (unsigned int j=0; j < this->set[i]->size(); j++)
			{
				for (unsigned int k=0; k < input_size; k++) 
					(*input_means)[k] += (*set[i]->get_input(j))[k];
				input_n++;
				//	cout << "::" << (*set[i]->get_input(j))[k] << endl; }
				for (unsigned int k=0; k < target_size; k++) {
					if (!isnan((*set[i]->get_target(j))[k])) {
					 (*target_means)[k] += (*set[i]->get_target(j))[k];
					 target_n[k]++;
					}
				}

			}	
			
		}
		
		for (unsigned int k=0; k < input_size; k++)
					(*input_means)[k] /= input_n;
		for (unsigned int k=0; k < target_size; k++)
					(*target_means)[k] /= target_n[k];
		
		/*cout << "****" << endl;
		cout << input_means->size() << endl;		
		cout << target_means->size() << endl;	
		*/
		// calculate standard deviation
		//unsigned int n=0;
		for (unsigned int i=0; i < this->size(); i++)
		{
			for (unsigned int j=0; j < this->set[i]->size(); j++)
			{
				for (unsigned int k=0; k < input_size; k++) 
					(*input_stds)[k] += ( (*set[i]->get_input(j))[k] - (*input_means)[k]) * ( (*set[i]->get_input(j))[k] - (*input_means)[k]);
				//	cout << "::" << (*set[i]->get_input(j))[k] << endl; }
				for (unsigned int k=0; k < target_size; k++) {
					if (!isnan((*set[i]->get_target(j))[k]))
					(*target_stds)[k] += ( (*set[i]->get_target(j))[k] - (*target_means)[k]) * ( (*set[i]->get_target(j))[k] - (*target_means)[k]);
				}
			}	
			
		}		

		for (unsigned int k=0; k < input_size; k++)
					(*input_stds)[k] = sqrt( (*input_stds)[k]/(input_n-1) );
		for (unsigned int k=0; k < target_size; k++)
					(*target_stds)[k] = sqrt( (*target_stds)[k]/(target_n[k]-1) );

	/*	cout << "****" << endl;
		cout << input_stds->size() << endl;		
		cout << target_stds->size() << endl;
		*/
		input_is_normalized = input;
		target_is_normalized = target;

		if (!input) {
			for (unsigned int k=0; k < input_size; k++)
			{
				(*input_stds)[k] = 1.0;
				(*input_means)[k] = 0.0;
			}

		}

		if (!target) {
			for (unsigned int k=0; k < target_size; k++)
			{
				(*target_stds)[k] = 1.0;
				(*target_means)[k] = 0.0;
			}
		}

		// apply changes		
		apply_normalization(input_means, input_stds, target_means, target_stds);
		
		// output		
	/*
		std::cout << "input means : ";
		std::ostream_iterator< weight_t > output( cout, " " );		
		std::copy( (*input_means).begin(), (*input_means).end(), output );
		std::cout << endl << "target means : ";
		std::copy( (*target_means).begin(), (*target_means).end(), output );
		std::cout << endl;
		std::cout << "input stds : ";	
	//	std::ostream_iterator< weight_t > output( cout, " " );		
		std::copy( (*input_stds).begin(), (*input_stds).end(), output );
		std::cout << endl << "target stds : ";
		std::copy( (*target_stds).begin(), (*target_stds).end(), output );
		std::cout << endl;
*/

		find_minmax();
	}	
	
	SequenceSet* SequenceSet::create_subset(unsigned int index1, unsigned int index2)
	{
		assert(index2 <= this->size());
		assert(index1 >= 0);
		assert(index1 < index2);

		SequenceSet* set = new SequenceSet();

		for (unsigned int i=index1; i < index2; i++)
		{
			set->add_sequence( this->get(i) );
		}

		return set;
	}

	SequenceSet* SequenceSet::create_subset_without(unsigned int index1, unsigned int index2)
	{
		assert(index2 <= this->size());
		assert(index1 >= 0);
		assert(index1 < index2);

		SequenceSet* set = new SequenceSet();

		for (unsigned int i=0; i < index1; i++)
		{
			set->add_sequence( this->get(i) );
		}
		for (unsigned int i=index2; i < this->size(); i++)
		{
			set->add_sequence( this->get(i) );
		}

		return set;
	}


	void SequenceSet::apply_normalization(vector<weight_t>* input_means, vector<weight_t>* input_stds, vector<weight_t>* target_means, vector<weight_t>* target_stds)
	{
		assert(set.size() > 0);
		assert(set[0]->size() > 0);
		
		assert(input_means->size() == input_size);
		assert(input_stds->size() == input_size);
		assert(target_means->size() == target_size);
		assert(target_stds->size() == target_size);

		
		// apply transformation		
		
		for (unsigned int i=0; i < this->size(); i++)
		{
			//cout << "Sequence #" <<i << " length: "<< this->set[i]->size() <<endl;
			for (unsigned int j=0; j < this->set[i]->size(); j++)
			{	
				//cout << " t=" << j << " " << set[i]->get_input(j)->size() << " " << (*set[i]->get_target(j)).size() << endl;
				for (unsigned int k=0; k < input_size; k++) 
					(*set[i]->get_input(j))[k] = ( (*set[i]->get_input(j))[k] - (*input_means)[k] ) / (*input_stds)[k];
				for (unsigned int k=0; k < target_size; k++) 
					(*set[i]->get_target(j))[k] = ( (*set[i]->get_target(j))[k] - (*target_means)[k] ) / (*target_stds)[k];
			
			}
		}		
	}	

	void SequenceSet::apply_denormalization(vector<weight_t>* input_means, vector<weight_t>* input_stds, vector<weight_t>* target_means, vector<weight_t>* target_stds)
	{
		assert(set.size() > 0);
		assert(set[0]->size() > 0);

		assert(input_means->size() == input_size);
		assert(input_stds->size() == input_size);
		assert(target_means->size() == target_size);
		assert(target_stds->size() == target_size);


		// apply transformation

		for (unsigned int i=0; i < this->size(); i++)
		{
			//cout << "Sequence #" <<i << " length: "<< this->set[i]->size() <<endl;
			for (unsigned int j=0; j < this->set[i]->size(); j++)
			{
				//cout << " t=" << j << " " << set[i]->get_input(j)->size() << " " << (*set[i]->get_target(j)).size() << endl;
				for (unsigned int k=0; k < input_size; k++)
					(*set[i]->get_input(j))[k] = ( (*set[i]->get_input(j))[k] * (*input_stds)[k]) + (*input_means)[k] ;
				for (unsigned int k=0; k < target_size; k++)
					(*set[i]->get_target(j))[k] = ( (*set[i]->get_target(j))[k] * (*target_stds)[k]) + (*target_means)[k] ;

			}
		}
	}


void SequenceSet::denormalize()
{

	apply_denormalization(input_means, input_stds, target_means, target_stds);
}

void SequenceSet::denormalize(SequenceSet* other_set)
{
	assert(other_set->input_is_normalized == true or other_set->target_is_normalized);

	this->apply_denormalization(other_set->input_means, other_set->input_stds,
		other_set->target_means, other_set->target_stds);
}

void SequenceSet::normalize_scale_and_clip(unsigned int standard_deviations)
{
	normalize();

	//clip and scale
	for (unsigned int i=0; i < this->size(); i++)
	{
		//cout << "Sequence #" <<i << " length: "<< this->set[i]->size() <<endl;
		for (unsigned int j=0; j < this->set[i]->size(); j++)
		{
			//cout << " t=" << j << " " << set[i]->get_input(j)->size() << " " << (*set[i]->get_target(j)).size() << endl;
			for (unsigned int k=0; k < input_size; k++)
				(*set[i]->get_input(j))[k] = fmin( (*set[i]->get_input(j))[k], standard_deviations ) / standard_deviations;
			for (unsigned int k=0; k < target_size; k++)
				(*set[i]->get_target(j))[k] = fmin( (*set[i]->get_target(j))[k], standard_deviations ) / standard_deviations;

		}
	}

}

SequenceSet* SequenceSet::embed(unsigned int n)
{
	SequenceSet* set = new SequenceSet();

	for (unsigned int i=0; i < this->set.size(); i++)
	{
		Sequence* embeded_sequence = this->get(i)->embed(n);
		set->add_sequence(embeded_sequence);
	}

	return set;
}

DataSet* SequenceSet::toBalancedDataSet(double precent_training, double percent_validation)
{
	this->shuffle();

	DataSet* dataset = new DataSet();

	dataset->training_set = new SequenceSet();
	dataset->validation_set = new SequenceSet();
	dataset->test_set = new SequenceSet();


	dataset->training_set->name = "Training Set";
	dataset->test_set->name = "Test Set";
	dataset->validation_set->name = "Validation Set";



	// find all keys
	vector<string> keys;
	for (unsigned int i=0; i < size(); i++)
	{
		string key = this->get(i)->name;
		keys.push_back(key);
	}

	map<string,unsigned int> index_map;
	for (unsigned int i=0; i < keys.size();i++)
	{
		index_map[keys[i]] = 0;
	}


	for (unsigned int i=0; i < size(); i++)
	{
		string key = this->get(i)->name;
		index_map[key]++;
	}

	cout << "Found " << index_map.size() << " elements" << endl;
	// split sets according to keys
	//for (unsigned int k=0; k < index_map.size() ;k++)
	for (map<string,unsigned int>::iterator it = index_map.begin(); it != index_map.end(); it++ )
	{
		string key = (*it).first;//keys[k];
		unsigned int cur_size = index_map[key];
		unsigned int training_size = round(cur_size*precent_training);
		unsigned int validation_size = round(cur_size*percent_validation);
		unsigned int test_size = cur_size-training_size-validation_size;
		cout <<"- splitting " << key << " with " << index_map[key] << " entries into " <<training_size<<"/"<<validation_size<<"/"<<test_size  << endl;

		unsigned int counter=0;
		for (unsigned int i=0; i < size(); i++)
		{
			if (get(i)->name != key)
				continue;

			if (counter < training_size)
				dataset->training_set->add_sequence( get(i) );
			else if (counter < training_size+validation_size)
				dataset->validation_set->add_sequence( get(i) );
			else
				dataset->test_set->add_sequence( get(i) );

			counter++;
		}
	}

	for (unsigned int i=0; i < dataset->training_set->size(); i++)
	{
		(*dataset->training_set)[i]->training_begin = 0;
		(*dataset->training_set)[i]->training_end = (*dataset->training_set)[i]->size();
		(*dataset->training_set)[i]->validation_begin = 0;
		(*dataset->training_set)[i]->validation_end = 0;
		(*dataset->training_set)[i]->test_begin = 0;
		(*dataset->training_set)[i]->test_end = 0;
	}

	for (unsigned int i=0; i < dataset->validation_set->size(); i++)
	{
		(*dataset->validation_set)[i]->training_begin = 0;
		(*dataset->validation_set)[i]->training_end = 0;
		(*dataset->validation_set)[i]->validation_begin = 0;
		(*dataset->validation_set)[i]->validation_end = (*dataset->validation_set)[i]->size();
		(*dataset->validation_set)[i]->test_begin = 0;
		(*dataset->validation_set)[i]->test_end = 0;
	}

	for (unsigned int i=0; i < dataset->test_set->size(); i++)
	{
		(*dataset->test_set)[i]->training_begin = 0;
		(*dataset->test_set)[i]->training_end = 0;
		(*dataset->test_set)[i]->validation_begin = 0;
		(*dataset->test_set)[i]->validation_end = 0;
		(*dataset->test_set)[i]->test_begin = 0;
		(*dataset->test_set)[i]->test_end = (*dataset->test_set)[i]->size();
	}

	return dataset;

}

DataSet* SequenceSet::toDataSet(double precent_training, double percent_validation)
{
	return toDataSet(precent_training, percent_validation, true);
}

DataSet* SequenceSet::toDataSet(double precent_training, double percent_validation, bool shuffle)
{
	if (shuffle) {
		this->shuffle();
	}

	DataSet* dataset = new DataSet();

	unsigned int training_size = round(this->size()*precent_training);
	unsigned int validation_size = round(this->size()*percent_validation);
	//unsigned int test_size = this->size() - training_size-validation_size;

	dataset->training_set = this->create_subset(0, training_size);
	cout << "createt dataset->training_set" << endl;
	dataset->validation_set = this->create_subset(training_size, training_size+validation_size);
	cout << "createt dataset->validation_set" << endl;
	dataset->test_set = this->create_subset( training_size+validation_size, this->size() );
	cout << "createt dataset->test_set" << endl;

	dataset->training_set->name = "Training Set";
	dataset->test_set->name = "Test Set";
	dataset->validation_set->name = "Validation Set";

	for (unsigned int i=0; i < dataset->training_set->size(); i++)
	{
		(*dataset->training_set)[i]->training_begin = 0;
		(*dataset->training_set)[i]->training_end = (*dataset->training_set)[i]->size();
		(*dataset->training_set)[i]->validation_begin = 0;
		(*dataset->training_set)[i]->validation_end = 0;
		(*dataset->training_set)[i]->test_begin = 0;
		(*dataset->training_set)[i]->test_end = 0;
	}

	for (unsigned int i=0; i < dataset->validation_set->size(); i++)
	{
		(*dataset->validation_set)[i]->training_begin = 0;
		(*dataset->validation_set)[i]->training_end = 0;
		(*dataset->validation_set)[i]->validation_begin = 0;
		(*dataset->validation_set)[i]->validation_end = (*dataset->validation_set)[i]->size();
		(*dataset->validation_set)[i]->test_begin = 0;
		(*dataset->validation_set)[i]->test_end = 0;
	}

	for (unsigned int i=0; i < dataset->test_set->size(); i++)
	{
		(*dataset->test_set)[i]->training_begin = 0;
		(*dataset->test_set)[i]->training_end = 0;
		(*dataset->test_set)[i]->validation_begin = 0;
		(*dataset->test_set)[i]->validation_end = 0;
		(*dataset->test_set)[i]->test_begin = 0;
		(*dataset->test_set)[i]->test_end = (*dataset->test_set)[i]->size();
	}

	return dataset;
}

void SequenceSet::denormalize_prediction(vector<vector<weight_t>*>* prediction)
{

	for (unsigned int i=0; i < prediction->size(); i++)
	{
		for (unsigned int j=0; j < (*prediction)[i]->size(); j++)
		{
				//(*[i]->get_target(j))[k] = ( (*set[i]->get_target(j))[k] * (*target_stds)[k]) + (*target_means)[k] ;
				(*(*prediction)[i])[j] = ( (*(*prediction)[i])[j]
				        *  (*target_stds)[j]) + (*target_means)[j];
		}
	}

}

