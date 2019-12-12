toSequence <- function(x, input, target, ...)
{
  if (inherits(x,"data.frame")) {
    
    
    seq <- Sequence()
    for (i in 1:nrow(x)) {
      input_vals <- as.numeric( x[i,input] )
      target_vals <- as.numeric( x[i,target] )
      Sequence_add_from_array(seq, input_vals, target_vals, length(input), length(target))
    }
    
    return(seq)
    
  } else {
    stop("Not supported")
  }
  
}