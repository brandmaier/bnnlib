#' Conversion to Sequence format
#'
#' Converts a data.frame to a \code{Sequence}.
#'
#' @param x a \code{"data.frame"}
#' @param input column id(s) of input parameters
#' @param target column id(s) of target parameters
#'
#' @return Sequence
#'
#'
#' @export
toSequence <- function(x, input, target, sequence=NULL, ...)
{
  if (inherits(x,"data.frame")) {
    
    
    if (is.null(sequence))
      seq <- Sequence()
    else
      seq <- sequence
    
    for (i in 1:nrow(x)) {
      input_vals <- as.numeric( x[i,input] )
      target_vals <- as.numeric( x[i,target] )
      #cat("Inp: ",input_vals," Target: ", target_vals, " with ",length(input)," and ",length(target),"\n")
      Sequence_add_from_array(seq, input_vals, target_vals, length(input), length(target))
    }
  
    return(seq)
    
  } else {
    stop("Not supported. Trya data.frame!")
  }
  
}

#setClass("_p_std__vectorT_std__vectorT_double_std__allocatorT_double_t_t_p_std__allocatorT_std__vectorT_double_std__allocatorT_double_t_t_p_t_t", contains = 'ExternalReference')

#' Get the inputs from a \code{Sequence}
#'
#' @export
getInputs <- function(x, ...) {
  if (inherits(x,"_p_Sequence"))
    seq <- x
  else
    stop("Unknown format!")
  
  slen <- Sequence_size(seq)
  df <- matrix(NA, nrow=slen, ncol=Sequence_get_input_size(seq))
  for (i in 1:slen) {
    df[i,] <-  .Call('R_swig_toValue',   Sequence_get_input(seq, i-1), package="bnnlib")
  }
  return(df)
}

#' Obtain activations of all nodes
#' 
#' Retrieves activations of all nodes for a given \code{Sequence}.
#' 
#' @param network A network
#' @param sequence A sequence
#' 
#' @export
getActivations <- function(network, sequence) {
  x <- Network_activate_and_return_activations(network, sequence)
  num_nodes <- Network_get_num_nodes(network);
  ln <- Sequence_size(sequence)
  datfr <- matrix(NA, nrow=ln, ncol=num_nodes)
  for (i in 1:num_nodes) {
    rowdat<-  getRow(x,i-1)
    datfr[,i] <- rowdat
  }
  return(datfr)
}

#' Conversion to SequenceSet format
#'
#' Converts a data.frame to a \code{SequenceSet}.
#'
#' @param x a \code{"data.frame"}
#' @param input column id(s) of input parameters
#' @param target column id(s) of target parameters
#'
#' @return SequenceSet
#'
#'
#' @export
toSequenceSet <- function(x, input, target)
{
  set <- SequenceSet()
  seq <- toSequence(x, input ,target)
  SequenceSet_add_copy_of_sequence(set, seq)
  return(set)
}