#' @export
getOutgoingWeights <- function(e1, e2=NULL, simplify=TRUE) {

  num_nodes <- Ensemble_get_num_nodes(e1)
  cat("Nodes ",num_nodes,"\n")
  weights <- rep(list(NA), num_nodes)
  for (j in 1:num_nodes) {
    node <- Ensemble_get_node(e1, j-1)
    # iterate through all outgoing connections
    num_conn <- Node_get_num_outgoing_connections(node)
    temp <- rep(NA, num_conn)
    for (i in 1:num_conn) {
      conn <- Node_get_outgoing_connection(node,i-1)
      node_to <- Connection_to_get(conn)
      if (!is.null(e2)) {
        if (!Ensemble_contains_node(e2, node_to)) { next;}
      }
      temp[i] <- Trainable_weight_get(conn)
    }
    weights[[j]] <- temp
  }
  
  if (simplify)
    weights <- simplify2array(weights)
  
  return(weights)
}