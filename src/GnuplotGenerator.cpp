#include "GnuplotGenerator.h"
#include "ensembles/LSTMEnsemble.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "stdlib.h"
#include <typeinfo>
#include "ensembles/LSTMPlusEnsemble.h"
#include "functions.h"
#include <cassert>
#include <unistd.h> //for chdir

bool GnuplotGenerator::grid = false;
bool GnuplotGenerator::output_screen = true;
string GnuplotGenerator::temp_directory = "./temp";
string GnuplotGenerator::pretext= "";

void GnuplotGenerator::clear_temp_directory()
{
	stringstream path;
	path << temp_directory << "/*.gp";
	remove(path.str().c_str());
}

void GnuplotGenerator::plot_lstm_activations(Network* network, LSTMEnsemble* lstmensemble, Sequence* sequence)
{
	network->activate(sequence);

	vector<string>* titles = new vector<string>();
	titles->push_back("input gate");
	titles->push_back("output gate");
	titles->push_back("output");	
	titles->push_back("seq input");
	titles->push_back("seq target");
	
	stringstream sout;
	for (unsigned int i=0; i < sequence->size(); i++)
	 sout << lstmensemble->get_inputgate()->actbuf[i] << "\t" 
	 	<<  lstmensemble->get_outputgate()->actbuf[i] << "\t"
	 	<< lstmensemble->get_output()->actbuf[i] << "\n"
;	 	//<< (*sequence->get_input(i))[0] << "\t"
	// 	<< (*sequence->get_target(i))[0] << "\n";
	
	plot(&sout, titles, "LSTM activation");
}

void GnuplotGenerator::plot_lstm_activations(Network* network, LSTMForgetEnsemble* lstmensemble, Sequence* sequence)
{
	network->activate(sequence);

	vector<string>* titles = new vector<string>();
	titles->push_back("input gate");
	titles->push_back("forget gate");
	titles->push_back("output gate");
	titles->push_back("output");	
	titles->push_back("seq input");
	titles->push_back("seq target");
	
	stringstream sout;
	for (unsigned int i=0; i < sequence->size(); i++)
	 sout << lstmensemble->get_inputgate()->actbuf[i] << "\t" 
	 	<<  lstmensemble->get_forgetgate()->actbuf[i] << "\t"
	 	<<  lstmensemble->get_outputgate()->actbuf[i] << "\t"
	 	<< lstmensemble->get_output()->actbuf[i] << "\n"
;//	 	<< sequence->get_input(i)[0] << "\t"
	// 	<< sequence->get_target(i)[0] << "\n";
	
	plot(&sout, titles, "LSTM activation");
}

void GnuplotGenerator::plot_all_cec_activations(Network* network, Sequence* sequence)
{
	std::vector<Node*> nodes;
	
	for (unsigned int i=0; i < network->size; i++)
	{
		if (network->nodes[i]->name == "CEC")
		{
			nodes.push_back(network->nodes[i]);
		}	
	}
	
	if (nodes.size() == 0) {
		warning( "Could not plot CEC activations! There are no CECs in the network" );
		return;	
	}
	
	std::vector<string>* titles = new vector<string>();
		for (unsigned int j=0; j < nodes.size(); j++)
		{
				if (nodes[j]->name == "CEC") {
					titles->push_back("CEC");
				}
		}
	
	stringstream sout;
	
	network->activate(sequence);

	for (unsigned int i=0; i < sequence->size(); i++)
	{
		for (unsigned int j=0; j < nodes.size(); j++)
		{
				if (nodes[j]->name == "CEC") {
					sout << nodes[j]->actbuf[i] << "\t";
				}
		}	
		sout << "\n";
	}	
	
	plot(&sout, titles, "CEC activations");
}

void GnuplotGenerator::plot_shaded_curve(string title, vector<vector<weight_t>*>* wave, vector<unsigned int>* positions, vector<string>* shades)
{

	stringstream shading_code;
	for (unsigned int i=0; i < positions->size(); i++) {

		unsigned int pos_from = (*positions)[i];
		stringstream pos_to;
		//cout << (positions->size()-1)
		if (i+1 < positions->size()) {
			pos_to << (*positions)[i+1];
		} else {
			//cout << "***"<<endl;
			pos_to << "graph 1";
		}

		shading_code <<
		"set obj "<<(i+1)<<" rect from " << pos_from <<", graph 0 to "<<
		pos_to.str()
		<< ", graph 1 fs solid noborder 0.15 fc rgb \""<< (*shades)[i]
		<<"\" behind" << endl;
		//shading_code << "set title " << s
	}

	pretext = shading_code.str();

	plot_vector(title, wave);

	pretext = "";

}

void GnuplotGenerator::plot(stringstream* sstream, vector<string>* titles, string title)
{
	if (titles->size() == 0)
	{
		warning("GnuplotGenerator: No titles");
		return;	
	}
	
	unsigned int unique = (unsigned int) (rand()%10000000 );

	// export data
	stringstream fname;
	fname << temp_directory << "/" << "__temp"<<unique <<"__.dat";
	std::ofstream out(fname.str().c_str());
	out << sstream->str();	
	out.close();
	// export gnu-script
	stringstream fname2;
	fname2 << temp_directory << "/" << "__temp"<<unique<<"__.gp";
	std::ofstream out_script(fname2.str().c_str());
	if (!output_screen) {
	 out_script << "set terminal png\n";
	 out_script << "set output '__temp__.png'";	
	}
	out_script << "set title \"" << title << "\"" << endl;
	out_script << "set xlabel 'time'\nset ylabel 'activation'\n";
	out_script << "set key below" <<endl;
	out_script << pretext << endl;
	if (grid)
	 out_script << "set grid\nset xtics 1\n";
	out_script << "plot ";
	unsigned int maxsize = min(20,(int)titles->size());
	for (unsigned int i=0; i < maxsize; i++) {
		out_script << "'__temp"<<unique<<"__.dat' using " << (i+1) << " with l title '"<< (*titles)[i]<<"'";
		if ( i < maxsize-1)
			out_script << ", \\\n ";
		else
			out_script << "\n";
	}
//	out_script << "pause -1\n";
	out_script.close();
	
	chdir(temp_directory.c_str());
	stringstream filename;
	filename << "gnuplot -persist __temp"<<unique<<"__.gp";
	system(filename.str().c_str());
	chdir("../");

	//remove("__temp__.gp");
	//remove("__temp__.dat");

	// clean up
	delete titles;
}

void GnuplotGenerator::plot_activations(Network* network, Sequence* sequence, vector<Node*>* nodes)
{
	network->activate(sequence);
	std::stringstream sout("");
	vector<string>* titles = new vector<string>();
	titles->reserve(network->size);
	for (unsigned int j=0; j < nodes->size(); j++)
	{
		titles->push_back( (*nodes)[j]->name );
	}
	for (unsigned int i=0; i < sequence->size(); i++)
	{
		for (unsigned int j=0; j < nodes->size(); j++)
		{
			sout << (*nodes)[j]->actbuf[i] << "\t";
		}
		sout << "\n";
	}

	GnuplotGenerator::plot( &sout, titles, "Activations" );

}

void GnuplotGenerator::plot_all_activations(Network* network, Sequence* sequence)
{
	network->activate(sequence);

	std::stringstream sout("");
	vector<string>* titles = new vector<string>();
	titles->reserve(network->size);
	for (unsigned int j=0; j < network->nodes.size(); j++)
	{
		titles->push_back( network->nodes[j]->name );
	}
	for (unsigned int i=0; i < sequence->size(); i++)
	{
		for (unsigned int j=0; j < network->nodes.size(); j++)
		{
			sout << network->nodes[j]->actbuf[i] << "\t";
			//titles->push_back( network->nodes[j]->name );
		}	
		sout << "\n";
	}	
	
	GnuplotGenerator::plot( &sout, titles, "All Activations" );	
}

void GnuplotGenerator::plot_all_lstm_activations(Network* network, Sequence* sequence)
{
	network->activate(sequence);

	// find all LSTM-ensembles
	vector<LSTMForgetEnsemble*> lstm_ensembles;
	//std::cout << "Ensembles: " << network->ensembles.size() << endl;
	for (unsigned int i=0; i < network->ensembles.size(); i++)
	{
		LSTMForgetEnsemble* temp = dynamic_cast<LSTMForgetEnsemble*>(network->ensembles[i]);
		
		if (temp != NULL)
		{
			lstm_ensembles.push_back( temp );			
		}
		

	}	
	

	std::stringstream sout("");
	vector<string>* titles = new vector<string>();	
	for (unsigned int j=0; j < lstm_ensembles.size(); j++)
	{
		stringstream name;
		name << "output_gate" << j ;
		titles->push_back( name.str()  ); 	
		stringstream name2;
		name2 << "out" << j ;
		titles->push_back( name2.str()  ); 			
		stringstream name3;
		name3 << "forget_gate" << j ;
		titles->push_back( name3.str()  ); 			
		stringstream name4;
		name4 << "input_gate" << j ;
		titles->push_back( name4.str()  ); 		
	}	

	
	// generate title
	for (unsigned int i=0; i < sequence->size(); i++)
	{	
	 for (unsigned int j=0; j < lstm_ensembles.size(); j++)
	 {
			sout << lstm_ensembles[j]->get_outputgate()->actbuf[i] << "\t";
			sout << lstm_ensembles[j]->get_output()->actbuf[i] << "\t";	
			sout << lstm_ensembles[j]->get_forgetgate()->actbuf[i] << "\t";	
			sout << lstm_ensembles[j]->get_inputgate()->actbuf[i] << "\t";					
	 }
	 sout << "\n";
	}

	
	plot(&sout, titles, "LSTM output gate activation");
}

void GnuplotGenerator::plot_all_lstmplus_activations(Network* network, Sequence* sequence)
{
	network->activate(sequence);

	// find all LSTM-ensembles
	vector<LSTMPlusEnsemble*> lstm_ensembles;
	//std::cout << "Ensembles: " << network->ensembles.size() << endl;
	for (unsigned int i=0; i < network->ensembles.size(); i++)
	{
		LSTMPlusEnsemble* temp = dynamic_cast<LSTMPlusEnsemble*>(network->ensembles[i]);
		
		if (temp != NULL)
		{
			lstm_ensembles.push_back( temp );			
		}
		

	}	
	
	std::stringstream sout("");
	vector<string>* titles = new vector<string>();	
	for (unsigned int j=0; j < lstm_ensembles.size(); j++)
	{
		stringstream name;
		name << "act" << j ;
		titles->push_back( name.str()  ); 	
		stringstream name2;
		name2 << "out" << j ;
		titles->push_back( name2.str()  ); 			
	}	

	
	// generate title
	for (unsigned int i=0; i < sequence->size(); i++)
	{	
	 for (unsigned int j=0; j < lstm_ensembles.size(); j++)
	 {
			sout << lstm_ensembles[j]->get_outputgate()->actbuf[i] << "\t";
			sout << lstm_ensembles[j]->get_output()->actbuf[i] << "\t";			
	 }
	 sout << "\n";
	}

	
	plot(&sout, titles, "LSTM output gate activation");
}

void GnuplotGenerator::plot_keep_ensemble(Network* network, Sequence* sequence, KeepEnsemble* keep)
{
	std::stringstream sout("");
	vector<string>* titles = new vector<string>();	
	
	
	
	GnuplotGenerator::plot( &sout, titles, "Keep Ensemble Activation" );
}

void GnuplotGenerator::plot_output_target_difference(Network* network, Sequence* sequence)
{
	stringstream s;

	network->activate(sequence);

	unsigned int timesteps = network->timestep;

	vector<vector<weight_t>*>* out = network->get_last_outputs(timesteps);

	for (unsigned int i=0; i < sequence->size(); i++)
	{
		//cout << (timesteps-sequence->size()+i) << " - " << i << endl;
		for (unsigned int j=0; j < network->out_size; j++)
		{
		 s << ((*(*out)[timesteps-sequence->size()+i])[j] -
				 (*sequence->get_target(i))[j]) ;
		 if (j < (network->out_size-1)) { s << "\t"; }
		}
		s << "\n";
	}

	vector<string>* names = new vector<string>();
	for (unsigned int j=0; j < network->out_size; j++)
	{
		names->push_back(string("difference"));
	}

	plot(&s, names, "Prediction/Target Difference");
}


void GnuplotGenerator::plot_input_output_target(Network* network, Sequence* sequence)
{
	stringstream s;
	
	network->activate(sequence);
	
	vector<vector<weight_t>*>* out = network->get_last_outputs(sequence->size());
	
	for (unsigned int i=0; i < sequence->size(); i++)
	{
		s << (*(*out)[i])[0] << "\t" << (*sequence->get_input(i))[0] << "\t" << (*sequence->get_target(i))[0] << "\n";
	}
	
	vector<string>* names = new vector<string>();
	names->push_back("output");
	names->push_back("input");
	names->push_back("target");
	
	plot(&s, names, "Input/Output/Target");
}

void GnuplotGenerator::plot_input_target(Network* network, Sequence* sequence)
{
	stringstream s;

	network->activate(sequence);

	//vector<vector<weight_t>*>* out = network->get_last_outputs(sequence->size());

	for (unsigned int i=0; i < sequence->size(); i++)
	{
		s << (*sequence->get_target(i))[0]<< "\t" << (*sequence->get_input(i))[0] << "\n";
	}

	vector<string>* names = new vector<string>();
	names->push_back("target");
	names->push_back("input");


	plot(&s, names, "Input/Output/Target");
}


void GnuplotGenerator::plot_vector(string title, vector<weight_t>* v1)
{
	stringstream s;
	
	for (unsigned int i=0; i < v1->size(); i++)
	{
		s << (*v1)[i] << "\n";
	}	
	
	vector<string>* names = new vector<string>();
	names->push_back(title);

	
	plot(&s, names, title);	
}

void GnuplotGenerator::plot_vector(string title, vector<vector<weight_t>*>* v1)
{
	stringstream s;

	for (unsigned int i=0; i < v1->size(); i++)
	{
		for (unsigned int j=0; j < (*v1)[i]->size(); j++ )
			s << (*(*v1)[i])[j] << "\t";
		s << "\n";
	}

	vector<string>* names = new vector<string>();
	for (unsigned int j=0; j < (*v1)[0]->size(); j++ )
		names->push_back(title);


	plot(&s, names, title);

}

void GnuplotGenerator::plot_vector(string title, vector<vector<weight_t>*>* v1, string t1,vector<vector<weight_t>*>* v2, string t2 )
{
	//assert(v1->size()==v2->size());

	unsigned int size = min(v1->size(), v2->size());

	stringstream s;

	for (unsigned int i=0; i < size; i++)
	{
		for (unsigned int j=0; j < (*v1)[i]->size(); j++ )
			s << (*(*v1)[i])[j] << "\t";
		for (unsigned int j=0; j < (*v2)[i]->size(); j++ )
			s << (*(*v2)[i])[j] << "\t";
		s << "\n";
	}

	vector<string>* names = new vector<string>();
	for (unsigned int j=0; j < (*v1)[0]->size(); j++ )
		names->push_back(t1);
	for (unsigned int j=0; j < (*v2)[0]->size(); j++ )
		names->push_back(t2);


	plot(&s, names, title);

}



void GnuplotGenerator::plot_vectors(string title, vector<weight_t>* v1, string n1, vector<weight_t>* v2, string n2)
{
	
//	assert(v1->size()==v2->size());

	unsigned int max_length = max( v1->size(), v2->size() );

	for (unsigned int i=v1->size(); i < max_length; i++)
		v1->push_back( 0 );
	for (unsigned int i=v2->size(); i < max_length; i++)
		v2->push_back( 0 );

	stringstream s;
	
	for (unsigned int i=0; i < v1->size(); i++)
	{
		s << (*v1)[i] << "\t" << (*v2)[i] << "\n";
	}	
	
	vector<string>* names = new vector<string>();
	names->push_back(n1);
	names->push_back(n2);
	
	plot(&s, names, title);
}

void GnuplotGenerator::animate(vector<vector<weight_t>*>* data)
{
	animate(data, NULL, 1);
}


void GnuplotGenerator::animate(vector<vector<weight_t>*>* data, vector<vector<weight_t>*>* curve, unsigned int rate)
{
	unsigned int delay = 1;

	//unsigned int rate = 10;

	unsigned int limit_iterations = data->size()- rate;

	double ymin = min(data);
	double ymax = max(data);


	unsigned int unique = (unsigned int) (rand()%10000000 );
	unsigned int unique2 = unique+1;
	assert(data->size() > 0);

	stringstream sstream;

	for (unsigned int j=0; j < (*data)[0]->size(); j++ ) {
		for (unsigned int i=0; i < data->size(); i++)
		{

			sstream << (*(*data)[i])[j] << "\t";

		}
		sstream << "\n";
	}

	// export data
	stringstream fname;
	fname << temp_directory << "/" << "__temp"<<unique <<"__.dat";
	std::ofstream out(fname.str().c_str());
	out << sstream.str();
	//cout << sstream.str();
	out.close();




	bool plot_curve = (curve != NULL);
	if (plot_curve)
	{

		stringstream sstream;

			for (unsigned int i=0; i < curve->size(); i++)
			{
				for (unsigned int j=0; j < (*curve)[0]->size(); j++ ) {

				sstream << (*(*curve)[i])[j] << "\t";

			}
			sstream << "\n";
		}


			// export data


			stringstream fname;
			fname << temp_directory << "/" << "__temp"<<unique2 <<"__.dat";
			std::ofstream out(fname.str().c_str());
			out << sstream.str();
			//cout << sstream.str();
			out.close();


	}


	// write the animator file

	stringstream animator;
	animator << "iteration_count=iteration_count+1"<<endl;

	animator << "if (iteration_count <=limit_iterations) \\"<<endl;
	animator << "load \"plot.gp\"; "<<endl;
	animator << "if (iteration_count <=limit_iterations) \\"<<endl;
	animator << "i=i+"<<rate<<"; \\"<<endl;
	animator << "reread"<<endl;

	stringstream fname2;
	fname2 << temp_directory << "/" << "animator.gp";
	std::ofstream out3(fname2.str().c_str());

	//std::ofstream out3("animator.gp");

	out3 << animator.str();

	out3.close();
	// write the animation file
	stringstream animation;

	stringstream fname3;
	fname3 << temp_directory << "/" << "animation.gp";
	std::ofstream out2(fname3.str().c_str());

	//std::ofstream out2("animation.gp");
	out2 << "set term gif animate transparent opt delay "<< delay <<" size 600,300 x000000" << endl;

	out2 << "unset title\nunset key\n";
	//out2 <<	"unset xtics\n";
//	out2 << "unset ytics"<<endl;
//	out2 << "set size square"<<endl;
	out2 << "set border 1"<<endl;
	out2 << "set grid"<<endl;
	out2 << "set style line 1 lt 1 lw 50" <<endl;

	out2 << "i=0" << endl;
	out2 << "iteration_count = 0"<<endl;
	out2 << "limit_iterations = "<< limit_iterations<<endl;

	// here goes actual animation code

	stringstream plotcode;

	if (plot_curve) {
		plotcode << "set size 1.0, 1.0"<<endl;
		plotcode << "set origin .0, .0"<<endl;
		plotcode << "set multiplot"<<endl;
	}

	if (plot_curve) {
		plotcode << "set size 1.0, .5" << endl;
		plotcode << "set origin 0.,0.5"<<endl;
//		out2 << "set yrange ["<< ymin <<":"<<ymax<<"]"<<endl;
		plotcode << "set yrange [-1:+1]" << endl;
		plotcode <<
		"set obj 1 rect from i, graph 0 to (i+3), graph 1 fs solid noborder 0.15 fc rgb '#444444' behind" << endl;
		plotcode << "plot \"__temp"<<unique2<<"__.dat\" using 1 with l" << endl;
		plotcode << "unset obj 1" << endl;
		plotcode << "set size 1.0, .5" << endl;
		plotcode << "set origin 0, 0" <<endl;
		plotcode << "unset yrange" << endl;

	}

	//plotcode << "set yticks nomirror" << endl;
	//plotcode << "set yticks 0,0.2"<<endl;
	plotcode << "set yrange ["<< ymin <<":"<<ymax<<"]"<<endl;

	plotcode << "plot \"__temp"<<unique<<"__.dat\" using i with i ls 1" << endl;


	if (plot_curve)
		plotcode << "unset multiplot"<<endl;

	out2 << "load \"animator.gp\"" << endl;

	out2.close();

	// generate plotcode file
	stringstream fname4;
	fname4 << temp_directory << "/" << "plot.gp";
	std::ofstream out4(fname4.str().c_str());
	out4 << plotcode.str();
	out4.close();

	// generate animation
	chdir(temp_directory.c_str());
	stringstream filename;
	filename << "gnuplot animation.gp > test.gif";
	system(filename.str().c_str());
	chdir("../");
}



void GnuplotGenerator::plot_jacobian(Jacobian* jacobian, unsigned int output_node, unsigned int output_time, bool _3d)
{
	stringstream data;
	for (unsigned int t=0; t < jacobian->t; t++)
	{
		for (unsigned int i=0; i < jacobian->in_size; i++)
		{
			data << i << "\t" << t << "\t" << jacobian->get_derivative_of_output_to_input(output_node, output_time, i, t) << endl;
		}
	}


	unsigned int unique = (unsigned int) (rand()%10000000 );

	// export data
	stringstream fname;
	fname << temp_directory << "/" << "__temp"<<unique <<"__.dat";
	std::ofstream out(fname.str().c_str());
	out << data.str();
	out.close();

	// export code

	//stringstream code;

	// export gnu-script
	stringstream fname2;
	fname2 << temp_directory << "/" << "__temp"<<unique<<"__.gp";
	std::ofstream out_script(fname2.str().c_str());
	if (!output_screen) {
	 out_script << "set terminal png\n";
	 out_script << "set output '__temp__.png'";
	}

	if (_3d) {
			out_script << "set data style lines\n";
			out_script << "set contour base\n";
			out_script << "set surface\n";
			out_script << "set view 50,10,1.0,1.0\n";
			out_script << "set dgrid3d 40,51,3\n";
			out_script << "set xlabel 'Input Nodes'\n";
			out_script << "set ylabel 'Time'\n";
			out_script << "show contour\n";
			out_script << "splot " << "'__temp"<<unique <<"__.dat'";
	} else {
		out_script << "set view map" << endl;
		out_script << "set pm3d map" << endl;
		out_script << "plot "<< "'__temp"<<unique <<"__.dat' with image";
	}


	out_script.close();

	// execute
	chdir(temp_directory.c_str());
	stringstream filename;
	filename << "gnuplot -persist __temp"<<unique<<"__.gp";
	system(filename.str().c_str());
	chdir("../");
}
