
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
  
  x$last_ensemble <- NULL
    
  class(x) <- "banana"
  
  return(x)
}

print.banana <- function(x, ...) {
  if (!inherits(x,"banana")) { ui_stop("Wrong class type.") }
  ui_message("Banana Object (bnnlib)\n")
}

#' @export
has.network <- function(x) {
  if (!inherits(x,"banana")) { ui_stop("Not a banana object.") }
  if (!is.null(x$network)) return (TRUE)
}

#' @export
has.trainer <- function(x) {
  if (!inherits(x,"banana")) { ui_stop("Not a banana object.") }
  if (!is.null(x$trainer)) return (TRUE)  
}

#'
#' test whether a banana object has an associated sequenceset
#' 
#' @export
has.sequenceset <- function(x) {
  if (!inherits(x,"banana")) { stop("Not a banana object.") }
  if (!is.null(x$sequenceset)) return (TRUE)
}

#
# internal function
# 
add_to_banana <- function(target, x) {

  if (!is(target,"banana")) { ui_stop("Incompatible type!") }
  
  if (inherits(x,"_p_Network")) target$network <- x
  else if (inherits(x,"_p_Trainer")) target$trainer <- x
  else if (inherits(x,"_p_ErrorFunction")) target$errorfunction <- x
  else if (inherits(x, "data.frame")) {
    ui_message("Converting data.frame to SequenceSet")
    x <- toSequence(x)
    target$sequenceset <- x
  } else if (inherits(x,"_p_Ensemble")) {
      
      network <- target$network

      # add ensemble
      Network_add_ensemble(network,x)
      
      if (!is.null(target$last_ensemble)) {
        ui_ok("Connecting ensembles")
        Network_connect_ensembles(network,target$last_ensemble,x, TRUE)
      } else {
        Network_input_ensemble_set(network, x)
      }
      
      Network_output_ensemble_set(network, x)
        

      if (!is.null(target$last_ensemble)) {
        Network_sort_nodes(nn)
      }
      
      target$last_ensemble = x
      
  } else {
    ui_fail("Cannot add ",class(x), " to banana object. Unknown type.")
  }
  return(target)
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

  #
  # if source is not a banana, convert it to banana object
  # and call this function
  #
  if (!inherits(x,"banana"))
  {
    ui_ok("Initializing banana object")
    target <- bnn() # call banana constructor
    target <- add_to_banana(target, x)
  } else {
    target <- x
  } 
  
  #  
  # proceed assuming x is a banana object
  # decide actions depending on type of x
  #
 
    result <- add_to_banana(target,y)
    ui_message("Number of nodes: ", Network_get_num_nodes(result$network),
               " Number of trainable weights ",Network_get_num_trainable_weights(result$network))
    
  
  
  
  return(result)
  
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


