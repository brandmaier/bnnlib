bnn <- function() {
  
  # create object as empty list
  x <- list()
  x$trainer <- NULL
  x$network <- NULL
  x$sequenceset <- NULL
    
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
  if (!is.null(x$network)) return (TRUE);
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
    
    if (is.null(y$network)) {
      y$network = Network()
    }
    
    Network_ensembles_get(y$network, Network_get_ensembles_size(y$network))
    Network_add_ensemble()
    
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


