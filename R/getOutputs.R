
#' Get activations of all output nodes for a given \code{sequence}
#' 
#' @param network A network
#' @param x A \code{Sequence}
#' 
#' @export
getOutputs <- function(network, x, index=NULL, ...) {
  
  if (inherits(x,"_p_Sequence"))
    seq <- x
  else
    stop("Unknown format")
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
