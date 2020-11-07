
#' function that looks up whether a letter from the alphabet is in x, gives back True or False 
#' (internal)
where_in_alphabet_ <- function(x, alphabet = myletters) {alphabet %in% x}

#' 0-1 Matrix with option to return True-False Matrix when setting numeric = FALSE 
#' @export
onehot <- function(x, alphabet = letters, numeric = TRUE){
  out <- t(apply(matrix(x), 1, where_in_alphabet_, alphabet = alphabet))
  colnames(out) <- alphabet
  rownames(out) <- x
  if (numeric) out <- out*1
  out
}

#' @export
where_in_alphabet <- function(x, alphabet = letters, numeric = TRUE){
  out <- t(apply(matrix(x), 1, where_in_alphabet_, alphabet = alphabet))
  colnames(out) <- alphabet
  rownames(out) <- x
  if (numeric) out <- out*1
  out
}


#' @export
add_text_to_sequenceset <- function(text, letters, target_vals, seqset=NULL, split_at=NA) {
  
tfmatrix = where_in_alphabet(text, letters)

seq = Sequence()

ln <- ncol(tfmatrix)

for (i in 1:(min(nrow(tfmatrix)-1,80000))  ) {
  input_vals = tfmatrix[i,]
  
  
  Sequence_add_from_array(seq, input_vals, target_vals, ln, length(target_vals))
  
  if (!is.na(split_at)) {
   if (i %% split_at==0) {
    if (!is.null(seqset)) {
      SequenceSet_add_copy_of_sequence(seqset, seq)
      seq = Sequence()
    }
   }
  }
}

return(seq)
}