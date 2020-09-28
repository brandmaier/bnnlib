
#' @export
getNodeNames <- function(net)
{
  num_nodes = Network_get_num_nodes(net)
  nms <- rep(NA, num_nodes)
  for (i in 1:num_nodes)
    nms[i] = Network_get_node_name(net, i-1)
  
  nms <- make.unique(nms)
  return(nms)
}