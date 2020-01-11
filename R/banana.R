
# global constants from Node.h
GENERIC_NODE = 0
TANH_NODE = 1
SIGMOID_NODE = 2
LINEAR_NODE = 3
 LSTM_NODE = 4
 PI_NODE = 5
 BIAS_NODE = 6
 PRODUCT_NODE = 7
 THRESHOLD_NODE = 8
 LN_NODE = 9
 CONDITIONAL_BIAS_NODE = 10
 SCALED_TANH_NODE = 11
 SCALED_SIGMOID_NODE = 12
 STEEP_SIGMOID_NODE = 13
 PRODUCTBIASED_NODE = 14
 SIN_NODE = 15
 TSIN_NODE = 16
 UNIFORM_NOISE_NODE = 17
 GAUSSIAN_NODE = 18
 ONE_SHOT_BIAS_NODE = 19
 ALTERNATING_BIAS_NODE = 20
 GAUSSIAN_NOISE_NODE = 21
 PSEUDO_OUTPUT_NODE = 22
 MATCHING_OUTPUT_NODE = 23
 RELU_NODE = 24

setClass('_p_FeedforwardEnsemble', contains = c('ExternalReference','_p_Ensemble'))

bnn <- function() {
  
  # create object as empty list
  x <- list()
  x$trainer <- NULL
  x$network <- NULL
  x$sequenceset <- NULL
  x$verbose <- TRUE
    
  class(x) <- "banana"
  
  return(x)
}

print.banana <- function(x, ...) {
  if (!inherits(x,"banana")) ui_stop("Wrong class type.")
  ui_message("Banana Object (bnnlib)\n")
}

#' @export
has.network <- function(x) {
  if (!inherits(x,"banana")) ui_stop("Not a banana object.")
  if (!is.null(x$network)) return (TRUE)
}

#' @export
has.trainer <- function(x) {
  if (!inherits(x,"banana")) ui_stop("Not a banana object.")
  if (!is.null(x$trainer)) return (TRUE)  
}

#'
#' test whether a banana object has an associated sequenceset
#' 
#' @export
has.sequenceset <- function(x) {
  if (!inherits(x,"banana")) stop("Not a banana object.")
  if (!is.null(x$sequenceset)) return (TRUE)
}


#'
#' pipe command to add elements to a banana object
#' 
#' Typical usage is:
#' 
#' network <- Network() %>% FeedforwardEnsemble(TANH, 1) %>% FeedforwardEnsemble(TANH, 10) %>% 
#' FeedforwardEnsemble(TANH, 10) %>% BackpropTrainer() %>% SquaredErrorFunction()
#'
#' @export
#' 
`%>%` <- function(x,y) {
  
  if (inherits(x,"_p_Trainer")) {
    y$trainer <- x
  
  } else if (inherits(x, "data.frame")) {
    x <- toSequence(x)
    y$sequenceset <- x
  } else if (inherits(x, "_p_Network")) {
    y$network <- x
  } else if (inherits(x,"_p_Ensemble")) {
    
    # if x and y are ensembles:
    if (inherits(y,"_p_Ensemble")) {
    
      Network_add_ensemble(network,y)
      
      bnn <- banana()
      
        
    # if x is an ensemble and y a banana
    } else if (inherits(y,"banana")) {
      if (is.null(y$network)) {
        ui_message("Creating new network")
        y$network = Network()
      }
      network = y$network
    } else {
      ui_stop("Incompatible types.")
    }
    

    
    
    
    # add ensemble
    Network_add_ensemble(y$network,x)
    
    Network_connect_ensembles__SWIG_0(nn,ens1,ens2, TRUE)
    y$last_ensemble = x
    
    Network_sort_nodes(nn)
    
    ui_message("Number of nodes: ", Network_get_num_nodes(nn)," Number of trainable weights ",Network_get_num_trainable_weights(nn))
    
    if (y$verbose) {
      
    } 
  } else {
    
    ui_fail("Unknown types. Could not combine them.")
    return(NULL)
    
  }
  
  return(y)
  
}

train <- function(x, steps=1) {
  
  if (!has.trainer(x)) {
    stop("No Trainer specified")
  }
  
  Trainer_train_sequenceset(x, x$sequenceset, steps)
}

#' predict
#' 
#' Implementation of generic \code{predict} function
#' 
#' @param x A bnn object
#' 
#' @export
  
predict.bnn <- function(x, newdata=NULL, index=NULL, ...) {
  
  stopifnot(inherits("x","banana"))
  
  network <- x$network
  
  if (inherits(newdata,"Sequence")) {
    sq <- newdata
  } else if (inherits(newdata, "SequenceSet")) {
    sq<-SequenceSet_get(newdata, index-1)
  
  } 
  
  Network_activate(network, sq)
  ouputs <- Network_get_outputs_until(network, Sequence_size(sq))
  
  return(outputs)
  
}


