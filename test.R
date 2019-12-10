dyn.load(paste("bnnlib", .Platform$dynlib.ext, sep=""))
source("bnnlib.R")
cacheMetaData(1)


# create a network using the Factory class
net<-NetworkFactory_createFeedForwardNetwork(1,6,1)

#bp <- BackpropTrainer(net)
bp <- CGTrainer(net)

# access properties of a network
Network_in_size_get(net)

# create a sequence and load data from file
seq <- SequenceSet()
#SequenceSet_load_from_file(seq, "sequence.dat.R") 

# create a sequence from scratch
seq1<-Sequence(c(1,2,3,4,5,6),c(0,0,0,1,1,1),6)
SequenceSet_add_sequence(seq,seq1)

SequenceSet_element_count_get(seq)
SequenceSet_print_sequences(seq)

Trainer_sequenceset_set(bp, seq)


#Trainer_train(bp, seq, 1)

allweights <- Network_get_all_weights(net)
Network_get_mean_weight(net)

#Trainer_train(bp, seq, 1)
#Trainer_train__SWIG_0(bp, seq, 10000)
#Trainer_train(bp, seq, 1000)
Network_get_mean_weight(net)

#Network_calculate_SSE(net)

Network_analysis(net)

Trainer_learning_rate_set(bp, .1)

#CallbackHandler* rp = new ReportCallback();
#trainer->add_callback(rp, 100);
rp = ReportCallback()
Trainer_add_callback(bp, rp,10)

iterations <- 20
err <- rep(NA,iterations)
for (i in 1:iterations) {
  cat("Training the network for some steps\n")
 Trainer_train__SWIG_0(bp, seq, 100)
 print(Network_evaluate_training_error(net, seq))
 err[i] <- Network_evaluate_training_error(net, seq)
}

plot(err,type="l",main = "Training Error")

#std::vector<weight_t>* get_input(unsigned int index);
sequence1 <- SequenceSet_get(seq,0)
Sequence_get_input(sequence1,1)
Sequence_get_target(sequence1,1)

.Call('R_swig_toValue', Sequence_get_input(sequence1,1), FALSE, PACKAGE='bnnlib')
.Call('R_swig_toValue', Sequence_get_target(sequence1,1), FALSE, PACKAGE='bnnlib')
#.Call('R_swig_toValue', allweights, FALSE, PACKAGE='bnnlib')

allweights <- Network_get_all_weights(net)
.Call('R_swig_toValue', allweights, FALSE, PACKAGE='bnnlib')

#.Call('R_swig_toValue', Sequence_get_input(sequence1,2), FALSE, PACKAGE='bnnlib')

# create a sequence

# generate Gnuplot:
#GnuplotGenerator_plot_all_activations(net, sequence1)
#act <- Network_activate_and_return_activations(net, sequence1)
Network_activate(net, sequence1)
x<-Network_get_output(net,1)
.Call('R_swig_toValue', x, FALSE, PACKAGE='bnnlib')
