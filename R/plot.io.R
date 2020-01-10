plot.io <- function(seq) {
  input <- getInputs(seq)
  target <- getTargets(seq)
  ln <- length(input)
  longdat <- reshape2::melt(data.frame(time=1:ln,input,target),id.vars="time")
  ggplot(data=longdat, aes(x=time,y=value,group=variable,col=variable))+geom_line()+
    theme_minimal()+xlab("Time")+ylab("Activation")
}

