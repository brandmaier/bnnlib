
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
