#' @param network A network
#' @param seq1 A sequence
#'
#' @export
plotPredictions <- function(network, seq1) {
  
  Network_activate(network, seq1)
  ln <-Sequence_size(seq1)
  lf <- Sequence_get_target_size(seq1)
  vals <-  matrix(NA, nrow=ln, ncol=lf)
  trt <- matrix(NA, nrow=ln, ncol=lf)
  inp <- rep(NA, ln)
  for (i in 1:ln) {
    outp <- Network_get_output(network,i-1)
    vals[i,] <- .Call('R_swig_toValue',  outp, package="bnnlib") 
    outp <- Sequence_get_target(seq1, i-1)
    trt[i, ] <- .Call('R_swig_toValue',  outp, package="bnnlib") 
    inpp <- Sequence_get_input(seq1, i-1)
    inp[i] <- .Call('R_swig_toValue',  inpp, package="bnnlib") 
  }
 
  
  melted <- reshape2::melt(vals)
  melted$Var2 <- factor(melted$Var2)
  #melted <- melted[1:1000,]
  p1 <- ggplot(data=melted, aes(x=Var1,y=value,group=Var2,color=Var2))+geom_point()+geom_line()+
    theme_minimal()+ theme(legend.position="bottom")+ ylab("Value")+xlab("Time")+ ggtitle("Predictions")
  
  
  melted <- reshape2::melt(trt)
  melted$Var2 <- factor(melted$Var2)  
  p2<- ggplot(data=melted, aes(x=Var1,y=value,group=Var2,color=Var2))+geom_point()+geom_line()+
    theme_minimal()+ theme(legend.position="bottom")+ ylab("Value")+xlab("Time")+ ggtitle("Target")
  
  p3 <- ggplot(data=data.frame(x=1:length(inp),y=inp), aes(x=x,y=y))+geom_line()+ theme_minimal()+
    ylab("Value")+xlab("Time")+ggtitle("Input")
  
  g_legend<-function(a.gplot){
    tmp <- ggplot_gtable(ggplot_build(a.gplot))
    leg <- which(sapply(tmp$grobs, function(x) x$name) == "guide-box")
    legend <- tmp$grobs[[leg]]
    return(legend)
  }
  
  mylegend<-g_legend(p1)
  
  
  
  
  pl <- gridExtra::grid.arrange(gridExtra::arrangeGrob(p1 + theme(legend.position="none"),
                                 p2 + theme(legend.position="none"), p3,
                                 nrow=3),
                     mylegend, nrow=2,heights=c(10, 1))
  
#  return(pl)
  invisible(pl)
}