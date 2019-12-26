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
    stop("Not supported")
  }
  
}

toDataFrame <- function(x, ...) {
  
  if (inherits(x,"_p_Sequence"))
    seq <- x
  #seq1<-SequenceSet_get(testset,0)
  slen <- Sequence_size(seq)
  Network_activate(network, seq)
  data = Network_get_outputs_until(network, slen)
  df <- matrix(NA, nrow=slen, ncol=Sequence_get_target_size(seq))
  for (i in 1:slen) {
    df[i, ] = getRow(data, i-1)
  }
  return(df)
}