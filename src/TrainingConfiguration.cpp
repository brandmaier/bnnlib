
#include "Network.h"
#include "ensembles/Ensemble.h"
#include "ensembles/LSTMEnsemble.h"
#include "nodes/Node.h"
#include "functions.h"
#include "ensembles/FeedforwardEnsemble.h"
#include "GnuplotGenerator.h"
#include "TrainingConfiguration.h"
#include "trainer/BackpropTrainer.h"
#include "trainer/RPropTrainer.h"
#include "trainer/Trainer.h"
#include "callbacks/ReportCallback.h"
#include "trainer/ImprovedRPropTrainer.h"
#include "callbacks/ProgressDot.h"
#include "trainer/MyRPropTrainer.h"
#include <sys/stat.h>

	TrainingConfiguration::TrainingConfiguration()
	{
		init();
	}
	
	TrainingConfiguration::TrainingConfiguration(string filename)
	{
		init();
		load(filename);
	}
	
	void TrainingConfiguration::init()
	{
		comment = "";
		scale = 1.0;
		normalize=false;
		save_every = 100;
		report_every = 100;
		randomize = false;
		epochs = 0;
		training_filename = "";
		test_filename = "";
		save_filename = "";
		network_filename = "";
		randomize = false;
		randomize_bias_weights = true;
		scale_min = 0;
		scale_max = 0;
		directory = ".";
		method = -1;
		dots = true;
		embedded = false;
		embedding_dimension = 1;
	}
	
	void TrainingConfiguration::save(string filename)
	{
	 ofstream myfile;
 	 myfile.open ( filename.c_str() );
 	 
 	 myfile << "BNNlib" << __BNNLIB_VERSION << endl;
 	 myfile << "[comment]" << endl;
 	 
 	 myfile << "[parameters]" << endl;

 	 myfile << "training filename: "<< training_filename << endl;
 	 myfile << "test filename: "<< test_filename << endl;

 
 	 myfile.close();		
	}
	
void TrainingConfiguration::load(string filename)
{
  string line;
  ifstream myfile (filename.c_str());
  int current_section = 0;
  if (myfile.is_open())
  {
    while (! myfile.eof() )
    {
      getline (myfile,line);
      //std::cout << line <<endl;
      if (line.length() == 0) continue;
     
      if (line.substr(0,1) == "[") {
      	if (line=="[comment]") { current_section = 4; }

      	if (line=="[parameters]") { current_section = 3; }
      } else {
      	if (current_section == 3)
      	{
      	 vector<string> tokens;
      	 string sep = std::string(":");
         split(line,sep,tokens);  
         
   
         if (tokens[0]=="training filename") {
         	stripLeadingAndTrailingBlanks( tokens[1] );
         	training_filename = tokens[1];	
         }
         if (tokens[0]=="test filename") {
         	stripLeadingAndTrailingBlanks( tokens[1] );
         	test_filename =  tokens[1] ;	
         }
         if (tokens[0]=="save filename") {
         	stripLeadingAndTrailingBlanks( tokens[1] );
         	save_filename =  tokens[1] ;	
         }         
          if (tokens[0]=="network filename") {
         	stripLeadingAndTrailingBlanks( tokens[1] );
         	network_filename =  tokens[1] ;	
         }
          if (tokens[0]=="directory") {
           	stripLeadingAndTrailingBlanks( tokens[1] );
           	directory =  tokens[1] ;
          }
         if (tokens[0]=="epochs")
         {
         	epochs = atoi(tokens[1].c_str() );	
         }   
         if (tokens[0]=="scale")
         {
         	scale = atof(tokens[1].c_str() );	
         }
         if (tokens[0]=="scale-min")
         {
         	scale_min = atof(tokens[1].c_str() );
         }
         if (tokens[0]=="scale-max")
         {
         	scale_max = atof(tokens[1].c_str() );
         }
         if (tokens[0]=="normalize")
         {
         	normalize = (atoi(tokens[1].c_str()) == 1);
         }
         if (tokens[0]=="trainingmethod")
         {
         	method = atoi(tokens[1].c_str());
         }
         if (tokens[0]=="randomize") {
        	 randomize = atoi(tokens[1].c_str()) == 1;
         }
         if (tokens[0]=="randomize_bias_weights") {
        	 randomize_bias_weights = atoi(tokens[1].c_str()) == 1;
         }
         if (tokens[0]=="dot") {
        	 dots = atoi(tokens[1].c_str()) == 1;
         }
         if (tokens[0]=="report_every") {
        	 report_every = atoi(tokens[1].c_str());
         }
         if (tokens[0]=="save_every") {
        	 save_every = atoi(tokens[1].c_str());
         }

         if (tokens[0]=="method") {
        	 method = atoi(tokens[1].c_str());
         }
         if (tokens[0]=="embedded") {
        	 embedded = atoi(tokens[1].c_str()) == 1;
         }
         if (tokens[0]=="embedding_dimension") {
        	 embedding_dimension = atoi(tokens[1].c_str());
         }
      	}
 
      }
      
      // --- //
    } // eof
  } else {
  		error("Could not open configuration file!");
  }
		
}
	
	void TrainingConfiguration::execute(bool train)
	{
		// log
		stringstream log;

		// create time string
 		time_t rawtime;
   	 	time ( &rawtime ); 	 
  	 	string time = ctime(&rawtime);   	   	 		
		
  	 	log << time;

		// create network
		network = new Network();
		
		network->load( network_filename );
		
		log << "Network: " << network_filename << endl;

		if (!network->sanity_check(false)) {
			error("Network did not pass sanity check!");
		}

		if (randomize) {
			cout << "randomize" << endl;
			network->randomize(randomize_bias_weights);
		}


		Trainer* trainer = NULL;
		if (method==0) {
			trainer = new BackpropTrainer(network);
			cout << "using backprop"<<endl;
		} else if (method == 1) {
			trainer = new RPropTrainer(network);
			cout << "using rprop"<<endl;
		} else if (method == 2) {
			trainer = new ImprovedRPropTrainer(network);
			cout << "using iRprop" << endl;
		} else if (method == 3) {
			trainer = new MyRPropTrainer(network);
			cout << "using Myprop" << endl;
		} else {
			stringstream st;
			st << "Unknown or no training algorithm specified: " << method;
			error( st.str() );
		}

		cout << "learning-rate: " << trainer->learning_rate << endl;
		cout << "learning-rate-discount: " << trainer->learning_rate_discount << endl;
		cout << "momentum: " << trainer->momentum << endl;
		


		// load inputfile
		std::cout << "loading '" << training_filename << "'" << endl;
		training_sequence = new SequenceSet();
		training_sequence->load_from_file( training_filename );
		
		if (normalize) {
			training_sequence->normalize();	
		}
		
		if (scale != 1) {
			cout << "scale: " << scale;
			training_sequence->scale(scale);	
		}
		
		if ((scale_min != 0) and (scale_max != 0))
		{
			training_sequence->scale_to_minmax(scale_min, scale_max);
			cout << "scaling sequence to " << scale_min << "-" << scale_max << endl;
 		}

		if (embedded) {

			SequenceSet* embedded_set = training_sequence->embed(embedding_dimension);
			//delete training_sequence;
			training_sequence = embedded_set;
		}

		if (dots)
			trainer->add_callback( new ProgressDot(), 1);

		// sanity checks
		if (training_sequence->target_size != network->out_size)
		{
			stringstream errormsg;
			errormsg << "Target size of dataset ("<<  training_sequence->target_size
					<<") does not match number of output nodes of network ("
					<< network->out_size << ")";
			error(errormsg.str());
		} else {
			cout << "network output size and dataset target size match!";
		}



		// training

		if (train) {
		 std::cout << "training " << endl;
		 // start training
		 trainer->add_callback( this, save_every);
		 trainer->add_callback( new ReportCallback(), report_every);
		 trainer->train(training_sequence, training_sequence, epochs);
		 vector<weight_t>* errors = &trainer->error_train;
		 std::cout << "finished" << endl;
		 GnuplotGenerator::plot_vector("Error", errors);
		 delete errors;
		}
		
		network->test_sequence(training_sequence, true);
		
		network->save(save_filename);
		



		// cleanup

		delete network;
		delete training_sequence;
		
	}
	
	void TrainingConfiguration::callback(Network* net, Trainer* trainer, SequenceSet* set)
	{

		// does directory exist?
		struct stat stFileInfo;
		int intStat = stat(directory.c_str(),&stFileInfo);
		if (intStat != 0) {
			warning("Output directory did not exist! Created it!");
			//mkdir(directory.c_str(),0711); # Linux/Unix
			mkdir(directory.c_str()); # Windows TODO: make this platform-independent
		}

		std::cout << "Callback" << endl;	
		stringstream stream;
		cout << "saving network" << endl;
		stream << directory << "/temp-" << trainer->total_iterations << ".bnn";
		network->save( stream.str() );

		stream << " network saved";
		message(stream.str());
		//GnuplotGenerator::plot_all_activations(network, (*training_sequence)[0]);
	}
	
	void TrainingConfiguration::save_as_image()
	{
		// create network
		network = new Network();

		network->load( network_filename );
		//network->sort_nodes();
		network->export_to_dot_graph("network");

				
	}
