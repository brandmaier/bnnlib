generateSpikeDelayTask <- function(  num_seq = 20, seq_len = 50, delay = 30) {

  if (delay+1 > seq_len ) stop("Incompatible parameters")
  
  seq <- SequenceSet()

  for (i in 1:num_seq) {
  
    # create a sequence from scratch
    input <- rep(0,len)
    output <- rep(0,len)
    
    pos <- sample(1:(seq_len-delay-1),1)
    input[pos]<-1
    output[pos+delay]<-1
    
    seq1<-Sequence(input,output,seq_len)
    
    SequenceSet_add_copy_of_sequence(seq,seq1)
  }  
    return(seq)
}