#' Create a feedforward network object backed by a C++ pointer
#' @param in_size Number of input units.
#' @param hid_size Number of hidden units.
#' @param out_size Number of output units.
#' @return An object of class `bnn_network_ptr`.
#' @export
bnn_create_feedforward_network <- function(in_size, hid_size, out_size) {
  ptr <- .Call("bnn_create_feedforward_network", as.integer(in_size), as.integer(hid_size), as.integer(out_size))
  structure(list(ptr = ptr), class = "bnn_network_ptr")
}

#' @export
print.bnn_network_ptr <- function(x, ...) {
  n <- .Call("bnn_network_num_nodes", x$ptr)
  cat("<bnn_network_ptr>", n, "nodes\n")
  invisible(x)
}

#' @export
bnn_network_node_names <- function(network) {
  stopifnot(inherits(network, "bnn_network_ptr"))
  .Call("bnn_network_node_names", network$ptr)
}

#' @export
bnn_network_get_node <- function(network, index_one_based) {
  stopifnot(inherits(network, "bnn_network_ptr"))
  ptr <- .Call("bnn_network_get_node", network$ptr, as.integer(index_one_based))
  structure(list(ptr = ptr), class = "bnn_node_ptr")
}

#' @export
print.bnn_node_ptr <- function(x, ...) {
  name <- .Call("bnn_node_name", x$ptr)
  nin <- .Call("bnn_node_num_incoming", x$ptr)
  nout <- .Call("bnn_node_num_outgoing", x$ptr)
  cat("<bnn_node_ptr>", name, sprintf("(in=%d, out=%d)", nin, nout), "\n")
  invisible(x)
}

#' @export
bnn_sequence_create <- function() {
  structure(list(ptr = .Call("bnn_sequence_create")), class = "bnn_sequence_ptr")
}

#' @export
bnn_sequence_add <- function(sequence, input, target) {
  stopifnot(inherits(sequence, "bnn_sequence_ptr"))
  .Call("bnn_sequence_add", sequence$ptr, as.numeric(input), as.numeric(target))
  invisible(sequence)
}

#' @export
bnn_sequence_size <- function(sequence) {
  stopifnot(inherits(sequence, "bnn_sequence_ptr"))
  .Call("bnn_sequence_size", sequence$ptr)
}

#' @export
bnn_sequence_get_input <- function(sequence, index_one_based) {
  stopifnot(inherits(sequence, "bnn_sequence_ptr"))
  .Call("bnn_sequence_get_input", sequence$ptr, as.integer(index_one_based))
}

#' @export
bnn_sequence_get_target <- function(sequence, index_one_based) {
  stopifnot(inherits(sequence, "bnn_sequence_ptr"))
  .Call("bnn_sequence_get_target", sequence$ptr, as.integer(index_one_based))
}

#' @export
bnn_sequenceset_create <- function() {
  structure(list(ptr = .Call("bnn_sequenceset_create")), class = "bnn_sequenceset_ptr")
}

#' @export
bnn_sequenceset_add_sequence <- function(sequenceset, sequence) {
  stopifnot(inherits(sequenceset, "bnn_sequenceset_ptr"), inherits(sequence, "bnn_sequence_ptr"))
  .Call("bnn_sequenceset_add_sequence", sequenceset$ptr, sequence$ptr)
  invisible(sequenceset)
}

#' @export
bnn_sequenceset_size <- function(sequenceset) {
  stopifnot(inherits(sequenceset, "bnn_sequenceset_ptr"))
  .Call("bnn_sequenceset_size", sequenceset$ptr)
}

#' @export
bnn_sequenceset_get_sequence <- function(sequenceset, index_one_based) {
  stopifnot(inherits(sequenceset, "bnn_sequenceset_ptr"))
  ptr <- .Call("bnn_sequenceset_get_sequence", sequenceset$ptr, as.integer(index_one_based))
  structure(list(ptr = ptr), class = "bnn_sequence_ptr")
}

#' @export
bnn_create_trainer <- function(network, trainer_type = c("backprop", "adam", "rmsprop", "rprop", "myrprop")) {
  stopifnot(inherits(network, "bnn_network_ptr"))
  trainer_type <- match.arg(trainer_type)
  ptr <- .Call("bnn_create_trainer", trainer_type, network$ptr)
  structure(list(ptr = ptr, type = trainer_type), class = "bnn_trainer_ptr")
}

#' @export
bnn_trainer_name <- function(trainer) {
  stopifnot(inherits(trainer, "bnn_trainer_ptr"))
  .Call("bnn_trainer_name", trainer$ptr)
}

#' @export
bnn_trainer_train <- function(trainer, sequenceset, iterations) {
  stopifnot(inherits(trainer, "bnn_trainer_ptr"), inherits(sequenceset, "bnn_sequenceset_ptr"))
  .Call("bnn_trainer_train", trainer$ptr, sequenceset$ptr, as.integer(iterations))
  invisible(trainer)
}

#' @export
print.bnn_trainer_ptr <- function(x, ...) {
  cat("<bnn_trainer_ptr>", .Call("bnn_trainer_name", x$ptr), "\n")
  invisible(x)
}
