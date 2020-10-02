In this document, we will compare the convergence of different training
algorithms for a given dataset and network architecture. The data will
be simulated from a simple toy problem, in which there are four i.i.d.
Gaussian predictors and a single continuous outcome.

Load Library
------------

First, we load `bnnlib`’s shared library.

    library(bnnlib)
    library(tidyverse)

Generate Data
-------------

Next, we simulate some data in a `data.frame` and convert this to bnn’s
Sequence format:

    N <- 100 # number of rows
    M <- 5 # number of columns
    simdata <- data.frame(matrix(data=rnorm(N*M),nrow = N))
    simdata[,M] <- simdata[,1]+simdata[,2]-ifelse(simdata[,3]>0,2,0)

    set <- SequenceSet()
    seq <- toSequence(simdata,1:4,5)
    SequenceSet_add_copy_of_sequence(set, seq)

    ## NULL

This is a sanity check, whether the input and output size is correct.

    Sequence_get_target_size(seq)

    ## [1] 1

    Sequence_get_input_size(seq)

    ## [1] 4

Create Network
--------------

Now, we create a feed-forward network with a single hidden layer of 10
neurons.

    net <- NetworkFactory_createFeedForwardNetwork(M-1,10,1,bnnlib::LINEAR_NODE)

Trainer
-------

Let’s generate a list of different training algorithms and run them each
for 200 steps. Save the results in `err.data` for later plotting. The
different training algorithms are

-   Backpropagation (BP)
-   Resilient Propagation (RProp)
-   Improved Resilient Propagation (IRPROP)
-   Simulated Annealing + RProp (SARPROP)

<!-- -->

    sarp <- SARPropTrainer(net)
    SARPropTrainer_temperature_set(sarp, .9)

    ## NULL

    cg <- CGTrainer(net)
    epochs <- 200

    trainer <- list( BackpropTrainer(net), ImprovedRPropTrainer(net),
    ARPropTrainer(net) ,  RPropTrainer(net), sarp, cg)

    result <- sapply(trainer, function(x){ Network_reinitialise(net); Trainer_initialize(x); Trainer_train2(x, set, epochs); 
      getTrainingerror(x) } )

    trainer_names <- sapply(trainer, Trainer_get_name)

    result <- data.frame(t(result), trainer=trainer_names)
    names(result)[1:epochs]<-formatC(1:epochs,width=3,flag="0")

Plot the errors of the different training algorithms on top of each
other.

    pivot_longer(result,cols =  1:200,names_to = "step",values_to="error") %>%
      mutate(time=parse_number(step)) %>% 
    ggplot(aes(x=time,y=error,group=trainer,col=trainer))+
      geom_point()+
      geom_line()+
      theme_minimal()+
      xlab("Step")+
      ylab("Error")+
      scale_y_log10()+
      ggtitle("Training Error (log scale)")

![](trainer_files/figure-markdown_strict/unnamed-chunk-6-1.png)
