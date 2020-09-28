
#' get targets of a given sequence
#' 
#' @param x A \code{Sequence}.
#' 
#' @return A matrix of targets
#' 
#' @export
getTargets <- function(x, ...) {
  if (inherits(x,"_p_Sequence"))
    seq <- x
  else
    stop("Unknown format!")
  
  slen <- Sequence_size(seq)
  df <- matrix(NA, nrow=slen, ncol=Sequence_get_target_size(seq))
  for (i in 1:slen) {
    df[i,] <-  .Call('R_swig_toValue',   Sequence_get_target(seq, i-1), package="bnnlib")
  }
  return(df)
}
