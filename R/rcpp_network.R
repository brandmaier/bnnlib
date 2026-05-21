#' Create a feedforward network object backed by a C++ pointer
#'
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

#' Get node names of a network pointer
#' @param network A `bnn_network_ptr`.
#' @return Character vector with node names.
#' @export
bnn_network_node_names <- function(network) {
  stopifnot(inherits(network, "bnn_network_ptr"))
  .Call("bnn_network_node_names", network$ptr)
}

#' Extract one node from a network pointer
#' @param network A `bnn_network_ptr`.
#' @param index_one_based Node index (1-based).
#' @return An object of class `bnn_node_ptr`.
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
