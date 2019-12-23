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
  if (!inherits(x,"banana")) stop("Wrong class type.")
  cat("Banana Object (bnnlib)\n")
}

has.network <- function(x) {
  if (!inherits(x,"banana")) stop("Not a banana object.")
  if (!is.null(x$network)) return TRUE
}

has.trainer <- function(x) {
  if (!inherits(x,"banana")) stop("Not a banana object.")
  if (!is.null(x$trainer)) return TRUE  
}

has.sequenceset <- function(x) {
  if (!inherits(x,"banana")) stop("Not a banana object.")
  if (!is.null(x$sequenceset)) return TRUE
}


`%>%` <- function(x,y) {
  
  
  if (inherits(x,"_p_Trainer")) {
    y$trainer <- x
  
  } else if (inherits(x, "data.frame")) {
    x <- toSequence(x)
    y$sequenceset <- x
  } else if (inherits(x, "_p_Network")) {
    y$network <- x
  }
  
  return(y)
  
}

train <- function(x, steps=1) {
  
  if (!has.trainer(x)) {
    stop("No Trainer specified")
  }
  
  Trainer_train_sequenceset(x, x$sequenceset, steps)
}

predict.bnn <- function(x, newdata=NULL, ...) {
  
}