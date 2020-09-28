#' @export
plotTrainingerror <- function(trainer)
{
  
  values <- .Call('R_swig_toValue',  Trainer_error_train_get(trainer), package="bnnlib") 
  
  library(ggplot2)
  gplot <- ggplot2::ggplot(data.frame(x=1:length(values),values),aes(x=x,y=values))+geom_point()+
    geom_line()+geom_smooth()
  
  return(gplot)
  invisible()
}

#' @export
getTrainingerror <- function(trainer)
{
  .Call('R_swig_toValue',  Trainer_error_train_get(trainer), package="bnnlib") 
}