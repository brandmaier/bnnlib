#'
#'
#' @export
plotActivations <- function(net, seq, node.names=NULL) {
  
num_nodes = Network_get_num_nodes(net)
x = getActivations(net, seq)
nms <- rep(NA, num_nodes)
for (i in 1:num_nodes)
  nms[i] = Network_get_node_name(net, i-1)

nms <- make.unique(nms)

acts <- getActivations(net, seq)
acts.long <- reshape2::melt(acts)
acts.long$Var2 <- factor(acts.long$Var2, labels = nms)

if (!is.null(node.names))
  acts.long <- acts.long[acts.long$Var2 %in% node.names,]

library(ggplot2)
ggplot(acts.long, aes(x=Var1, y=value,group=Var2, col=Var2))+geom_line()

}

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
